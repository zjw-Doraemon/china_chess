#include "user_data.h"


UserInfo::UserInfo() 
{

}

UserInfo::UserInfo(QString name, QString email, bool is_effective):
	name(name), email(email), is_effective(is_effective)
{

}



UserInfo UserMgr::get_me_user()
{
	return me_user;
}

void UserMgr::set_me_user(UserInfo& user)
{
	me_user = user;
}

void UserMgr::add_friend(QString name, QString email)
{
	UserInfo user(name,email);
	friend_list.push_back(user);
}
