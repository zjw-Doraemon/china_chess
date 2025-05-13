#include "user.h"



UserInfo::UserInfo(bool is_effective):is_effective(is_effective)
{
}

UserInfo::UserInfo(std::string name, std::string email, std::shared_ptr<tcp_connection> connection, bool is_effective):
	name(name),email(email),_connection(connection), is_effective(is_effective)
{
}
void UserInfo::set_connection(const std::shared_ptr<tcp_connection>& connection) {
	_connection = connection;
}
UserInfo::~UserInfo()
{
	
}
UserInfo UserMgr::find_user(const std::string& name)
{
	std::shared_lock<std::shared_mutex> mtx(rw_mtx); // 读锁允许多个线程共同读
	try{
		return user_map[name];
	}
	catch(...){
		std::cout << "not find user" << std::endl;
		return UserInfo();
	}
	
	
}

std::string UserMgr::get_name_from_con(std::shared_ptr<tcp_connection> con)
{
	std::unique_lock<std::shared_mutex> mtx(rw_mtx);
	for (auto& c : user_map) {
		if (c.second._connection == con) {
			std::string name = c.first;
			
			
			return name;
		}
	}
}




void UserMgr::add_user(const UserInfo& user)
{
	std::unique_lock<std::shared_mutex> mtx(rw_mtx);
	user_map[user.name] = user;
}

void UserMgr::delete_user(const std::string& name)
{
	std::unique_lock<std::shared_mutex> mtx(rw_mtx);
	user_map.erase(name);
}

uint64_t UserMgr::get_user_num()
{
	return user_map.size();
}

void UserMgr::delete_user_from_connection(std::shared_ptr<tcp_connection> con)
{
	std::unique_lock<std::shared_mutex> mtx(rw_mtx);
	for (auto& c : user_map) {
		if (c.second._connection == con) {
			std::string name = c.first;
			user_map.erase(name);
			std::cout << name << " have offline" << std::endl;
			return;
		}
	}
}

UserMgr::UserMgr()
{
}


