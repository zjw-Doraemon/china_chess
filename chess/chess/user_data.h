#include <iostream>
#include "global.h"
#include "tcp.h"
class UserInfo  {
public: // 具有高耦合性 其他地方有直接使用到成员

	UserInfo();
	UserInfo(QString name, QString email, bool is_effective = true);
	QString name;
	QString email;
	bool is_effective = false;
	
};

class UserMgr : public single_class<UserMgr> {
public:
	UserInfo get_me_user();
	void set_me_user(UserInfo& user);
	void add_friend(QString name, QString email);
private:
	UserInfo me_user;
	std::vector<UserInfo> friend_list;

	
};
