#include "global.h"
#include "user.h"
#include <queue>
class mysql_connect {
public:
	mysql_connect(sql::mysql::MySQL_Driver*);
	~mysql_connect();
	sql::Connection* get_connect();
	bool isConnected;  // 是否连接成功，需要告知后续代码，防止崩溃
private:
	
	bool create_connect();
	void beat_check();
	sql::Connection* _sql;
	//std::chrono::system_clock::time_point last_time = std::chrono::system_clock::now();
	std::atomic<bool> is_check_connect{true};
	sql::mysql::MySQL_Driver* driver; // 驱动指针 创建连接用的 只有一个 存在于连接池中 这个只是传入让连接类使用
	std::thread t; // 心跳线程
};


class mysql_pool {
public:
	mysql_pool(uint8_t);
	~mysql_pool();
	std::unique_ptr<mysql_connect> get_connect();
	void return_connect(std::unique_ptr<mysql_connect>);
private:
	bool is_all_connect = true; // 是否所有的连接都连接成功
	std::mutex mtx;
	bool stop = false;
	std::condition_variable cv;
	sql::mysql::MySQL_Driver* driver = sql::mysql::get_mysql_driver_instance();
	std::queue<std::unique_ptr<mysql_connect>> mysql_que;
};

class mysql_mgr : public single_class<mysql_mgr>{
	
public:
	friend class single_class<mysql_mgr>;
	void initDataBase();
	bool register_user(const std::string& name, const std::string& email, const std::string& pwd);
	bool login(const std::string& email, const std::string& pwd);
	UserInfo get_user_info(std::string name);
	bool apply_friend(std::string& name , std::string& friend_name);
	bool auth_apply_friend(std::string& status,std::string& from_name, std::string& to_name);
	
private:
	mysql_mgr();
	mysql_pool connection_pool;
};