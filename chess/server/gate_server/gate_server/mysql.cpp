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
	// 如果为空就等待
	cv.wait(lock, [this] {
		// 如果停止了就放开等待，让他走下去直接返回nullptr，使用方需要注意对空指针的处理。
		if (stop) {
			return true;
		}
		return !mysql_que.empty();
		});
	if (stop) {
		return nullptr;
	}
	/*对于 std::queue 容器，其 front() 成员函数返回的是队列首元素的左值引用，所以先转换为右值
	2. return 左值时编译器的行为
	当 return 语句返回一个左值时，编译器通常会尝试复制该对象。不过，在某些情况下，编译器会进行优化，
	采用返回值优化（RVO）或命名返回值优化（NRVO）技术，避免不必要的复制。
	无优化情况
		如果没有优化，编译器会调用对象的复制构造函数来创建一个副本并返回。例如对于一个自定义类：
	有优化情况
		在现代编译器中，通常会进行 RVO 或 NRVO。例如上述代码在开启优化的情况下，可能不会调用复制构造函数，直接将 obj 作为返回值，避免了复制开销。
	当 return 语句返回一个右值时，编译器会尝试调用对象的移动构造函数来进行资源的转移，而不是复制对象。右值通常是临时对象或者通过 std::move 转换得到的对象。
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
	
	isConnected = create_connect(); // 连接结果

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

	// 连接数据库
	try {
		// 解析配置文件
		boost::property_tree::ptree pt;
		boost::property_tree::ini_parser::read_ini("config.ini", pt);
		std::string ip = pt.get<std::string>("Mysql.ip");
		std::string user = pt.get<std::string>("Mysql.User");
		std::string pwd = pt.get<std::string>("Mysql.Passwd");
		std::string Schema = pt.get<std::string>("Mysql.Schema");


		_sql = driver->connect(ip, user, pwd);
		_sql->setSchema(Schema); // 设置操作的数据库
		beat_check(); // 心跳开始
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
			std::this_thread::sleep_for(std::chrono::seconds(5)); // 延时
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
	defer tmp_defer([this,&con] () mutable {  // 匿名函数表达式是const，所以要添加mutable，不然不能使用std::move(new_con)。
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
		// 检查表是否存在（使用准确表名）
		std::unique_ptr<sql::Statement> stmt(con->get_connect()->createStatement());
		stmt->executeQuery("SELECT 1 FROM users LIMIT 1");
		stmt->executeQuery("SELECT 1 FROM friendships LIMIT 1");

		// 表存在则直接返回
		
		return;
	}
	catch (const sql::SQLException& e) {
		// 明确捕获表不存在错误（Error code 1146）
		if (e.getErrorCode() == 1146) { // ER_NO_SUCH_TABLE
			std::cout << "Table not found, starting to create the user table..." << std::endl;
			try {
				std::unique_ptr<sql::Statement> create_stmt(con->get_connect()->createStatement());

				std::string create_table_sql =
					"CREATE TABLE IF NOT EXISTS `users` (\n"
					"    `id` INT UNSIGNED AUTO_INCREMENT PRIMARY KEY COMMENT '用户唯一标识',\n"
					"    `name` VARCHAR(50) NOT NULL COMMENT '用户名',\n"
					"    `email` VARCHAR(100) NOT NULL COMMENT '电子邮箱',\n"
					"    `password` VARCHAR(255) NOT NULL COMMENT '【警告】临时测试用明文密码，生产环境需哈希加密',\n"  // 修复逗号 + 安全警告
					"    `created_at` TIMESTAMP DEFAULT CURRENT_TIMESTAMP COMMENT '创建时间',\n"
					"    `updated_at` TIMESTAMP DEFAULT CURRENT_TIMESTAMP ON UPDATE CURRENT_TIMESTAMP COMMENT '最后更新时间',\n"
					"    `is_verified` TINYINT(1) DEFAULT 0 COMMENT '邮箱验证状态',\n"
					"    UNIQUE KEY `unique_email` (`email`),\n"
					"    UNIQUE KEY `unique_name` (`name`),\n"
					"    INDEX `idx_email` (`email`)\n"
					") ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_unicode_ci COMMENT='用户信息表';";  // 合并字符集设置

				std::string create_friendship_table_sql =
					"CREATE TABLE IF NOT EXISTS `friendships` (\n"
					"    `id` INT UNSIGNED AUTO_INCREMENT PRIMARY KEY COMMENT '好友关系唯一标识',\n"  // 添加自增 ID 主键（更合理）
					"    `user_name` VARCHAR(50) NOT NULL COMMENT '用户名称',\n"
					"    `friend_name` VARCHAR(50) NOT NULL COMMENT '好友名称',\n"
					"    `status` ENUM('pending', 'accepted', 'rejected') DEFAULT 'pending' COMMENT '好友关系状态',\n"
					"    `created_at` TIMESTAMP DEFAULT CURRENT_TIMESTAMP COMMENT '关系创建时间',\n"
					"    `updated_at` TIMESTAMP DEFAULT CURRENT_TIMESTAMP ON UPDATE CURRENT_TIMESTAMP COMMENT '关系更新时间',\n"
					"    FOREIGN KEY (`user_name`) REFERENCES `users`(`name`) ON DELETE CASCADE ON UPDATE CASCADE,\n"
					"    FOREIGN KEY (`friend_name`) REFERENCES `users`(`name`) ON DELETE CASCADE ON UPDATE CASCADE,\n"
					"    UNIQUE KEY `unique_friendship` (`user_name`, `friend_name`) COMMENT '防止重复添加好友',\n"  // 明确注释
					"    INDEX `idx_user_name` (`user_name`),\n"
					"    INDEX `idx_friend_name` (`friend_name`)\n"
					") ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_unicode_ci COMMENT='用户好友关系表';";


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

				
				throw; // 重新抛出异常让上层处理
			}
		}
		else {
			// 其他类型错误处理
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
	// 捕获unique指针的引用
	defer tmp_defer([this, &con]() mutable {  // 匿名函数表达式是const，所以要添加mutable，不然不能使用std::move(new_con)。
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
		
		// 创建一个 PreparedStatement 对象
		std::unique_ptr<sql::PreparedStatement> pstmt(con->get_connect()->prepareStatement
		("INSERT INTO users (name, email, password) VALUES (?, ?, ?)"));
		pstmt->setString(1, name);
		pstmt->setString(2, email);
		pstmt->setString(3, pwd);
		// 执行插入操作
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
	// 捕获unique指针的引用
	defer tmp_defer([this, &con]() mutable {  // 匿名函数表达式是const，所以要添加mutable，不然不能使用std::move(new_con)。
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

		// 查询密码是否正确
		std::unique_ptr<sql::PreparedStatement> pstmt(con->get_connect()->prepareStatement
		("SELECT password from users where email = ?"));
		pstmt->setString(1, email);
		// 执行
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
	// 捕获unique指针的引用
	defer tmp_defer([this, &con]() mutable {  // 匿名函数表达式是const，所以要添加mutable，不然不能使用std::move(new_con)。
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

		// 查询用户是否存在
		std::unique_ptr<sql::PreparedStatement> pstmt(con->get_connect()->prepareStatement
		("SELECT name from users where name = ?"));
		pstmt->setString(1, name);
		// 执行
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
	// 捕获unique指针的引用
	defer tmp_defer([this, &con]() mutable {  // 匿名函数表达式是const，所以要添加mutable，不然不能使用std::move(new_con)。
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

		// 查询用户是否存在
		std::unique_ptr<sql::PreparedStatement> pstmt(con->get_connect()->prepareStatement
		("SELECT friend_name from friendships where user_name = ? and status = 'accepted'"));
		pstmt->setString(1, name);
		// 执行
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
	// 捕获unique指针的引用
	defer tmp_defer([this, &con]() mutable {  // 匿名函数表达式是const，所以要添加mutable，不然不能使用std::move(new_con)。
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

		// 查询用户是否存在
		std::unique_ptr<sql::PreparedStatement> pstmt(con->get_connect()->prepareStatement
		("SELECT email from users where name = ? "));
		pstmt->setString(1, name);
		// 执行
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
