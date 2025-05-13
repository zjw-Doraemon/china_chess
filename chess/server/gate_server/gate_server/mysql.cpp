#include "mysql.h"

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

	// �������ݿ�
	try {
		// ���������ļ�
		boost::property_tree::ptree pt;
		boost::property_tree::ini_parser::read_ini("config.ini", pt);
		std::string ip = pt.get<std::string>("Mysql.ip");
		std::string user = pt.get<std::string>("Mysql.User");
		std::string pwd = pt.get<std::string>("Mysql.Passwd");
		std::string Schema = pt.get<std::string>("Mysql.Schema");


		_sql = driver->connect(ip, user, pwd);
		_sql->setSchema(Schema); // ���ò��������ݿ�
		beat_check(); // ������ʼ
		return true;
	}
	catch (sql::SQLException& e) {
		std::cout << "sql connect error" << std::endl;
		std::cout << "The following is the error-specific error message" << std::endl;
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
	initDataBase();
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
		std::cout << "MySQL connection failed" << std::endl;
		return;
	}
	try {
		// �����Ƿ���ڣ�ʹ��׼ȷ������
		std::unique_ptr<sql::Statement> stmt(con->get_connect()->createStatement());
		stmt->executeQuery("SELECT 1 FROM users LIMIT 1");
		stmt->executeQuery("SELECT 1 FROM friendships LIMIT 1");

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
					"    `password` VARCHAR(255) NOT NULL COMMENT '�����桿��ʱ�������������룬�����������ϣ����',\n"  // �޸����� + ��ȫ����
					"    `created_at` TIMESTAMP DEFAULT CURRENT_TIMESTAMP COMMENT '����ʱ��',\n"
					"    `updated_at` TIMESTAMP DEFAULT CURRENT_TIMESTAMP ON UPDATE CURRENT_TIMESTAMP COMMENT '������ʱ��',\n"
					"    `is_verified` TINYINT(1) DEFAULT 0 COMMENT '������֤״̬',\n"
					"    UNIQUE KEY `unique_email` (`email`),\n"
					"    UNIQUE KEY `unique_name` (`name`),\n"
					"    INDEX `idx_email` (`email`)\n"
					") ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_unicode_ci COMMENT='�û���Ϣ��';";  // �ϲ��ַ�������

				std::string create_friendship_table_sql =
					"CREATE TABLE IF NOT EXISTS `friendships` (\n"
					"    `id` INT UNSIGNED AUTO_INCREMENT PRIMARY KEY COMMENT '���ѹ�ϵΨһ��ʶ',\n"  // ������� ID ������������
					"    `user_name` VARCHAR(50) NOT NULL COMMENT '�û�����',\n"
					"    `friend_name` VARCHAR(50) NOT NULL COMMENT '��������',\n"
					"    `status` ENUM('pending', 'accepted', 'rejected') DEFAULT 'pending' COMMENT '���ѹ�ϵ״̬',\n"
					"    `created_at` TIMESTAMP DEFAULT CURRENT_TIMESTAMP COMMENT '��ϵ����ʱ��',\n"
					"    `updated_at` TIMESTAMP DEFAULT CURRENT_TIMESTAMP ON UPDATE CURRENT_TIMESTAMP COMMENT '��ϵ����ʱ��',\n"
					"    FOREIGN KEY (`user_name`) REFERENCES `users`(`name`) ON DELETE CASCADE ON UPDATE CASCADE,\n"
					"    FOREIGN KEY (`friend_name`) REFERENCES `users`(`name`) ON DELETE CASCADE ON UPDATE CASCADE,\n"
					"    UNIQUE KEY `unique_friendship` (`user_name`, `friend_name`) COMMENT '��ֹ�ظ���Ӻ���',\n"  // ��ȷע��
					"    INDEX `idx_user_name` (`user_name`),\n"
					"    INDEX `idx_friend_name` (`friend_name`)\n"
					") ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_unicode_ci COMMENT='�û����ѹ�ϵ��';";


				create_stmt->execute(create_table_sql);
				create_stmt->execute(create_friendship_table_sql);
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

bool mysql_mgr::search_friend(const std::string& name)
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

		// ��ѯ�û��Ƿ����
		std::unique_ptr<sql::PreparedStatement> pstmt(con->get_connect()->prepareStatement
		("SELECT name from users where name = ?"));
		pstmt->setString(1, name);
		// ִ��
		std::unique_ptr<sql::ResultSet> res(pstmt->executeQuery());

		if (res->next() == false) {
			return false;
		}

		return true;
	}
	catch (sql::SQLException& e) {
		std::cout << "search friend error" << std::endl;
		std::cerr << "SQLException: " << e.what();
		std::cerr << " (MySQL error code: " << e.getErrorCode();
		std::cerr << ", SQLState: " << e.getSQLState() << " )" << std::endl;
		return false;

	}
}

boost::json::array mysql_mgr::get_friend_list(const std::string& name)
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
		return boost::json::array();
	}
	try {

		// ��ѯ�û��Ƿ����
		std::unique_ptr<sql::PreparedStatement> pstmt(con->get_connect()->prepareStatement
		("SELECT friend_name from friendships where user_name = ? and status = 'accepted'"));
		pstmt->setString(1, name);
		// ִ��
		std::unique_ptr<sql::ResultSet> res(pstmt->executeQuery());
		boost::json::array json_array{};
		while (res->next()) {
			json_array.push_back(boost::json::value(res->getString("friend_name").asStdString()));
		}
		return json_array;

		
	}
	catch (sql::SQLException& e) {
		std::cout << "search friend error" << std::endl;
		std::cerr << "SQLException: " << e.what();
		std::cerr << " (MySQL error code: " << e.getErrorCode();
		std::cerr << ", SQLState: " << e.getSQLState() << " )" << std::endl;
		return boost::json::array();

	}
}

std::string mysql_mgr::get_email(const std::string& name)
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
		return "";
	}
	try {

		// ��ѯ�û��Ƿ����
		std::unique_ptr<sql::PreparedStatement> pstmt(con->get_connect()->prepareStatement
		("SELECT email from users where name = ? "));
		pstmt->setString(1, name);
		// ִ��
		std::unique_ptr<sql::ResultSet> res(pstmt->executeQuery());
		
		if (!res->next()) {
			return "";
		}
		return res->getString("email").asStdString();

		
	}
	catch (sql::SQLException& e) {
		std::cout << "search friend error" << std::endl;
		std::cerr << "SQLException: " << e.what();
		std::cerr << " (MySQL error code: " << e.getErrorCode();
		std::cerr << ", SQLState: " << e.getSQLState() << " )" << std::endl;
		return "";

	}
}
