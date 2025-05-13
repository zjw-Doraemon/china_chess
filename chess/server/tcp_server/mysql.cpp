#include "mysql.h"
#include <thread>
mysql_pool::mysql_pool(uint8_t num)
{
	
	for (int i = 0; i < num; i++) {
		std::unique_ptr<mysql_connect> tmp ( new mysql_connect(driver));
		if (!tmp->isConnected) {
			is_all_connect = false;
		}
		mysql_que.push(std::move(tmp));
	}
}

mysql_pool::~mysql_pool()
{
	stop = true;
	cv.notify_all();
	delete driver;
}

std::unique_ptr<mysql_connect> mysql_pool::get_connect()
{
	if (!is_all_connect) {
		return nullptr;
	}
	std::unique_lock<std::mutex> lock(mtx);
	// ���Ϊ�վ͵ȴ�
	cv.wait(lock, [this] {
		// ���ֹͣ�˾ͷſ��ȴ�����������ȥֱ�ӷ���nullptr��ʹ�÷���Ҫע��Կ�ָ��Ĵ���
		if (stop) {
			return true;
		}
		return !mysql_que.empty();
		});
	if (stop) {
		return nullptr;
	}
	/*���� std::queue �������� front() ��Ա�������ص��Ƕ�����Ԫ�ص���ֵ���ã�������ת��Ϊ��ֵ
	2. return ��ֵʱ����������Ϊ
	�� return ��䷵��һ����ֵʱ��������ͨ���᳢�Ը��Ƹö��󡣲�������ĳЩ����£�������������Ż���
	���÷���ֵ�Ż���RVO������������ֵ�Ż���NRVO�����������ⲻ��Ҫ�ĸ��ơ�
	���Ż����
		���û���Ż�������������ö���ĸ��ƹ��캯��������һ�����������ء��������һ���Զ����ࣺ
	���Ż����
		���ִ��������У�ͨ������� RVO �� NRVO���������������ڿ����Ż�������£����ܲ�����ø��ƹ��캯����ֱ�ӽ� obj ��Ϊ����ֵ�������˸��ƿ�����
	�� return ��䷵��һ����ֵʱ���������᳢�Ե��ö�����ƶ����캯����������Դ��ת�ƣ������Ǹ��ƶ�����ֵͨ������ʱ�������ͨ�� std::move ת���õ��Ķ���
	*/
	std::unique_ptr<mysql_connect> tmp_mysql_connection = std::move(mysql_que.front());
	mysql_que.pop();
	return tmp_mysql_connection;


}

void mysql_pool::return_connect(std::unique_ptr<mysql_connect>  unique_connection)
{
	std::unique_lock<std::mutex> lock(mtx);
	if (stop) {
		return;
	}
	mysql_que.push(std::move(unique_connection));
	cv.notify_one();
}








mysql_connect::mysql_connect(sql::mysql::MySQL_Driver* driver) :driver(driver)
{
	
	isConnected = create_connect(); // ���ӽ��

}

mysql_connect::~mysql_connect()
{
	delete _sql;
	is_check_connect.store(false);
	t.join();

}

sql::Connection* mysql_connect::get_connect()
{
	return _sql;
}

bool mysql_connect::create_connect()
{
	boost::property_tree::ptree pt;
	boost::property_tree::ini_parser::read_ini("config.ini", pt);
	std::string ip = pt.get<std::string>("Mysql.ip");
	std::string user = pt.get<std::string>("Mysql.User");
	std::string pwd = pt.get<std::string>("Mysql.Passwd");
	std::string Schema = pt.get<std::string>("Mysql.Schema");
	// �������ݿ�
	try {
		_sql = driver->connect(ip, user, pwd);
		_sql->setSchema(Schema); // ���ò��������ݿ�
		beat_check(); // ������ʼ
		return true;
	}
	catch (sql::SQLException& e) {

		std::cerr << "SQLException: " << e.what();
		std::cerr << " (MySQL error code: " << e.getErrorCode();
		std::cerr << ", SQLState: " << e.getSQLState() << " )" << std::endl;
		return false;
	}
}

void mysql_connect::beat_check()
{
	t = std::thread([this]() {
		while(1){
			std::this_thread::sleep_for(std::chrono::seconds(5)); // ��ʱ
			if (this->is_check_connect == false) {
				return;
			}
			try {
				std::unique_ptr<sql::Statement> stmt(_sql->createStatement());
				
				stmt->executeQuery("SELECT 1");
			}
			catch (sql::SQLException& e) {
				std::cerr << "SQLException: " << e.what();
				std::cout << "attemp reconnect" << std::endl;
				bool is_connect = create_connect();
				if (is_connect == true) {
					return;
				}


				
			}

		}
		});
	t.detach();
}





mysql_mgr::mysql_mgr():connection_pool(5)
{
	//initDataBase(); //��ʼ������
}

void mysql_mgr::initDataBase() {
	
	auto con = connection_pool.get_connect();
	defer tmp_defer([this,&con] () mutable {  // �����������ʽ��const������Ҫ���mutable����Ȼ����ʹ��std::move(new_con)��
		if (con == nullptr) {
			return;
		}
		connection_pool.return_connect(std::move(con));
		});
	if (con == nullptr) {
		std::cout << "get MySQL connection failed" << std::endl;
		return;
	}
	try {
		// �����Ƿ���ڣ�ʹ��׼ȷ������
		std::unique_ptr<sql::Statement> stmt(con->get_connect()->createStatement());
		std::unique_ptr<sql::ResultSet> res(stmt->executeQuery("SELECT 1 FROM users LIMIT 1"));

		// �������ֱ�ӷ���
		
		return;
	}
	catch (const sql::SQLException& e) {
		// ��ȷ��������ڴ���Error code 1146��
		if (e.getErrorCode() == 1146) { // ER_NO_SUCH_TABLE
			std::cout << "Table not found, starting to create the user table..." << std::endl;
			try {
				std::unique_ptr<sql::Statement> create_stmt(con->get_connect()->createStatement());

				std::string create_table_sql =
					"CREATE TABLE IF NOT EXISTS `users` (\n"
					"    `id` INT UNSIGNED AUTO_INCREMENT PRIMARY KEY COMMENT '�û�Ψһ��ʶ',\n"
					"    `name` VARCHAR(50) NOT NULL COMMENT '�û���',\n"
					"    `email` VARCHAR(100) NOT NULL COMMENT '��������',\n"
					"    `password` VARCHAR(255) NOT NULL COMMENT '����',\n"  // ���������ֶ�
					"    `created_at` TIMESTAMP DEFAULT CURRENT_TIMESTAMP COMMENT '����ʱ��',\n"
					"    `updated_at` TIMESTAMP DEFAULT CURRENT_TIMESTAMP ON UPDATE CURRENT_TIMESTAMP COMMENT '������ʱ��',\n"
					"    `is_verified` TINYINT(1) DEFAULT 0 COMMENT '������֤״̬',\n"
					"    UNIQUE KEY `unique_email` (`email`),\n"
					"    UNIQUE KEY `unique_name` (`name`),\n"
					"    INDEX `idx_email` (`email`)\n"
					") ENGINE=InnoDB \n"
					"  DEFAULT CHARSET=utf8mb4 \n"
					"  COLLATE=utf8mb4_unicode_ci \n"
					"  COMMENT='�û���Ϣ����ʱ���԰棬���������룩';";  // ��Ӿ���ע��

				create_stmt->execute(create_table_sql);
				std::cout << "User table created successfully" << std::endl;

				
				
				return;
			}
			catch (const sql::SQLException& create_e) {
				std::cerr << "[Critical Error] Failed to create table: \n"
					//<< "SQL: " << create_table_sql << "\n"
					<< "Error code: " << create_e.getErrorCode() << "\n"
					<< "SQL state: " << create_e.getSQLState() << "\n"
					<< "Details: " << create_e.what() << std::endl;

				
				throw; // �����׳��쳣���ϲ㴦��
			}
		}
		else {
			// �������ʹ�����
			std::cerr << "Database initialization failed: \n"
				<< "Error code: " << e.getErrorCode() << "\n"
				<< "SQL state: " << e.getSQLState() << "\n"
				<< "Details: " << e.what() << std::endl;
			
			throw;
		}
	}
	catch (const std::exception& e) {
		
		std::cerr << "System exception: " << e.what() << std::endl;
		throw;
	}
	
}

bool mysql_mgr::register_user(const std::string& name, const std::string& email, const std::string& pwd)
{
	auto con = connection_pool.get_connect();
	// ����uniqueָ�������
	defer tmp_defer([this, &con]() mutable {  // �����������ʽ��const������Ҫ���mutable����Ȼ����ʹ��std::move(new_con)��
		if (con == nullptr) {
			return;
		}
		connection_pool.return_connect(std::move(con));
		});
	if (con == nullptr) {
		std::cout << "mysql connect failed" << std::endl;
		return false;
	}
	try {
		
		// ����һ�� PreparedStatement ����
		std::unique_ptr<sql::PreparedStatement> pstmt(con->get_connect()->prepareStatement
		("INSERT INTO users (name, email, password) VALUES (?, ?, ?)"));
		pstmt->setString(1, name);
		pstmt->setString(2, email);
		pstmt->setString(3, pwd);
		// ִ�в������
		int updateCount = pstmt->executeUpdate();
		if (updateCount == 0) {
			return false;
		}
		
		
	}
	catch (sql::SQLException& e) {
		
		std::cerr << "SQLException: " << e.what();
		std::cerr << " (MySQL error code: " << e.getErrorCode();
		std::cerr << ", SQLState: " << e.getSQLState() << " )" << std::endl;
		return false;
		
	}
	
	return true;
}

bool mysql_mgr::login(const std::string& email, const std::string& pwd)
{
	auto con = connection_pool.get_connect();
	// ����uniqueָ�������
	defer tmp_defer([this, &con]() mutable {  // �����������ʽ��const������Ҫ���mutable����Ȼ����ʹ��std::move(new_con)��
		if (con == nullptr) {
			return;
		}
		connection_pool.return_connect(std::move(con));
		});
	if (con == nullptr) {
		std::cout << "mysql connect failed" << std::endl;
		return false;
	}
	try {

		// ��ѯ�����Ƿ���ȷ
		std::unique_ptr<sql::PreparedStatement> pstmt(con->get_connect()->prepareStatement
		("SELECT password from users where email = ?"));
		pstmt->setString(1, email);
		// ִ��
		std::unique_ptr<sql::ResultSet> res(pstmt->executeQuery());
		
		if (res->next() == false) {
			return false;
		}
		if (res->getInt("password") != stol(pwd)) {
			return false;
		}

		return true;
	}
	catch (sql::SQLException& e) {
		
		std::cerr << "SQLException: " << e.what();
		std::cerr << " (MySQL error code: " << e.getErrorCode();
		std::cerr << ", SQLState: " << e.getSQLState() << " )" << std::endl;
		return false;

	}

	
}

UserInfo mysql_mgr::get_user_info(std::string email)
{
	auto con = connection_pool.get_connect();
	// ����uniqueָ�������
	defer tmp_defer([this, &con]() mutable {  // �����������ʽ��const������Ҫ���mutable����Ȼ����ʹ��std::move(new_con)��
		if (con == nullptr) {
			return ;
			// ��ֵ�bug  ��� ����д��return UserInfo()  ����� free(): invalid pointer�� �������if�����Զ���������Ҳ����˵���return��Զ����ִ�С�
			// ���������� return UserInfo(); �Ͳ������� ���� ֱ�Ӳ�������д�� return UserInfo() Ҳ���ᱨ��.
			// ���룺ʵ���� ���return Userinfo() ��ʹ�� ���������Ƶ��᷵��һ��userinfo���� ���� if����Զ���ᴥ����ʵ����û�з��أ�
			// ���Ǳ�������Ϊ�����ˣ���֪�����ĵ��õ��������� ������һ����Чָ�룬�ѵ����������������þ���ʵ������ɶҲû���أ��͵�������������
			// ʵ���������ʵ��©���˷��أ������δ������Ϊ�� ���Ժ������أ�һ��Ҫ����һ�����ͣ���ȻĪ��������������������һ����ָ�룬����
			/* �������ȴ�ɹ���  ��vs�������� ִ�е�  ģ���������
			* class test {
				public:
					int a = 0;
					~test() {
						std::cout << "destruct" << std::endl;
					}
				};
				int main() {
					auto a = []()  {
						if (0) {
							return test();
						}
						};
					a();
				}

			*/
		}
		connection_pool.return_connect(std::move(con));
		//return UserInfo();
		});
	if (con == nullptr) {
		std::cout << "mysql connect failed" << std::endl;
		return UserInfo();
	}
	try {

		
		std::unique_ptr<sql::PreparedStatement> pstmt(con->get_connect()->prepareStatement
		("SELECT name ,email  from users where email = ?"));
		pstmt->setString(1, email);
		// ִ��
		std::unique_ptr<sql::ResultSet> res(pstmt->executeQuery());

		if (res->next() == false) {
			return UserInfo();
		}
		std::string name = res->getString("name");
		std::string email = res->getString("email");
		UserInfo usr(name,email);

		return usr;
	}
	catch (sql::SQLException& e) {

		std::cerr << "SQLException: " << e.what();
		std::cerr << " (MySQL error code: " << e.getErrorCode();
		std::cerr << ", SQLState: " << e.getSQLState() << " )" << std::endl;
		return UserInfo();

	}
}

bool mysql_mgr::apply_friend(std::string& name, std::string& friend_name)
{
	auto con = connection_pool.get_connect();
	// ����uniqueָ�������
	defer tmp_defer([this, &con]() mutable {  // �����������ʽ��const������Ҫ���mutable����Ȼ����ʹ��std::move(new_con)��
		
		connection_pool.return_connect(std::move(con));
		});
	if (con == nullptr) {
		std::cout << "mysql connect failed" << std::endl;
		return false;
	}
	try {

		// 1. �ر��Զ��ύ����������
		con->get_connect()->setAutoCommit(false);

		// 2. Ԥ���뵥��������䣨˫���ϵ��ִ�����Σ�
		std::unique_ptr<sql::PreparedStatement> pstmt(con->get_connect()->prepareStatement(
			"INSERT INTO friendships (user_name, friend_name) VALUES (?, ?)"
		));

		// 3. ���������ϵ��A��B��
		pstmt->setString(1, name);
		pstmt->setString(2, friend_name);
		pstmt->executeUpdate();

		// 4. ���뷴���ϵ��B��A��
		pstmt->setString(1, friend_name);
		pstmt->setString(2, name);
		pstmt->executeUpdate();

		// 5. �ύ�������β�����ɹ����ύ��
		con->get_connect()->commit();

		return true;
	}
	catch (sql::SQLException& e) {
		con->get_connect()->rollback();  // �κδ��󶼻ع�
		std::cerr << "SQLException: " << e.what();
		std::cerr << " (MySQL error code: " << e.getErrorCode();
		std::cerr << ", SQLState: " << e.getSQLState() << " )" << std::endl;
		return false;

	}

}

bool mysql_mgr::auth_apply_friend(std::string& status ,std::string& from_name, std::string& to_name)
{
	auto con = connection_pool.get_connect();
	// ����uniqueָ�������
	defer tmp_defer([this, &con]() mutable {  // �����������ʽ��const������Ҫ���mutable����Ȼ����ʹ��std::move(new_con)��

		connection_pool.return_connect(std::move(con));
		});
	if (con == nullptr) {
		std::cout << "mysql connect failed" << std::endl;
		return false;
	}
	try {
		con->get_connect()->setAutoCommit(false);  // ��������

		// Ԥ���������䣨˫�����ʹ��ͬһԤ�������
		std::unique_ptr<sql::PreparedStatement> pstmt(con->get_connect()->prepareStatement(
			"UPDATE friendships "
			"SET status = ? "
			"WHERE user_name = ? AND friend_name = ?"
		));

		// 1. ���������ϵ��A��B��user_name��friend_name��
		pstmt->setString(1, status);
		pstmt->setString(2, to_name);
		pstmt->setString(3, from_name);
		pstmt->executeUpdate();

		// 2. ���·����ϵ��B��A��friend_name��user_name��
		pstmt->clearParameters();  // ������в�������ѡ��setString�Ḳ�ǵ�������ʽ�����
		pstmt->setString(1, status);
		pstmt->setString(2, from_name);
		pstmt->setString(3, to_name);
		pstmt->executeUpdate();

		// ������θ����Ƿ񶼳ɹ�����Ӱ���������ٸ�1�У�
		// ע�⣺executeUpdate()�����ؾ�����������ͨ��������ʽ����������ԭ����
		con->get_connect()->commit();  // �������¶��ɹ����ύ

		return true;

	}
	catch (const sql::SQLException& e) {
		con->get_connect()->rollback();  // �κδ��󶼻ع�
		std::cerr << "SQLException: " << e.what();
		std::cerr << " (MySQL error code: " << e.getErrorCode();
		std::cerr << ", SQLState: " << e.getSQLState() << " )" << std::endl;
		return false;
	}
}
