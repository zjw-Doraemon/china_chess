#include "global.h"
#include "SingleClass.h"
#include <map>
#include "tcp_connection.h"
class logic_system :public SingleClass<logic_system>{
public:
	// 移动语义，减少复制
	friend class SingleClass<logic_system>;
	std::pair<uint32_t,boost::json::object > handle_json
	(std::pair<uint32_t, boost::json::object >&& json, std::shared_ptr<tcp_connection> con);
private:
	logic_system();
	void init_logic_map();
	std::map<std::string , std::function<boost::json::object(boost::json::object& json, std::shared_ptr<tcp_connection> con)>> logic_map;
};