#pragma once
#include <iostream>
#include "SingleClass.h"
#include "tcp_connection.h"
#include <shared_mutex>
class UserInfo {
public:
	UserInfo(bool is_effective = false);
	UserInfo(std::string name,std::string email,std::shared_ptr<tcp_connection> connection = nullptr, bool is_effective = true);
	void set_connection(const std::shared_ptr<tcp_connection>&);
	bool is_effective;
	std::string name;
	std::string email;
	std::shared_ptr<tcp_connection> _connection;
	~UserInfo();
};
class UserMgr : public single_class<UserMgr>
{
	friend class single_class<UserMgr>;
public:
	UserInfo find_user(const std::string& name);
	std::string get_name_from_con(std::shared_ptr<tcp_connection> con);
	void add_user(const UserInfo& user);
	void delete_user(const std::string& name);
	uint64_t get_user_num();
	void delete_user_from_connection(std::shared_ptr<tcp_connection> con);
private:
	std::shared_mutex rw_mtx;
	UserMgr();
	std::unordered_map<std::string, UserInfo> user_map;
	
};