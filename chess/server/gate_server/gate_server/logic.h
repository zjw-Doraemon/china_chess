#include "global.h"
#include "HttpConnection.h"
class logic : public single_class<logic>
{
	friend class single_class<logic>;
	// 如果值传递 就是将对象的生命周期交给 传递到的函数管理 如果右值引用就是原来的函数管理
	using handle = std::function
		<void(http::request<http::dynamic_body>& request,
		std::unique_ptr<http::response<http::dynamic_body>>&)>;
	
public:
	// 将json包设置到回应包中
	void create_response(std::unique_ptr<http::response<http::dynamic_body>>& response, boost::json::object&);
	std::unique_ptr<http::response<http::dynamic_body>> handle_request(
		std::shared_ptr<HttpConnection>,
		http::request<http::dynamic_body>&);
private:
	std::string _varify_code;
	void init_logic();
	std::map<std::string, handle> logic_map;
	logic();

};