#include "logic.h"
#include "mysql.h"
#include "email.h"
void logic::create_response(std::unique_ptr<http::response<http::dynamic_body>>& response, boost::json::object &json_data)
{
	// 状态行
	response->result(http::status::ok); // 设置状态码
	response->version(11);// 设置版本
	//响应体
	std::string data = boost::json::serialize(json_data);
	beast::ostream(response->body()) << data;

	//响应头
	response->set(http::field::content_type, "application/json");
	response->content_length(response->body().size());  // 设置回包长度 //需要在添加包的数据之后
}
std::unique_ptr<http::response<http::dynamic_body>> logic::handle_request(
	std::shared_ptr<HttpConnection> connect,
	http::request<http::dynamic_body>& request)
{
	std::unique_ptr<http::response<http::dynamic_body>> _response(new http::response<http::dynamic_body>()); // 创建一个回包
	// 解析请求包 json 数据，查看需要什么服务。
	std::string body = beast::buffers_to_string(request.body().data());
	boost::json::object json_data = boost::json::parse(body).as_object();
	//std::cout << json_data["action"].as_string().c_str() << std::endl;
	logic_map[std::string(json_data["action"].as_string().c_str())](request, _response); // 有默认的隐式转换 c 字符串构造 string 类型  调用 逻辑处理
	return _response;
}

void logic::init_logic() {
	logic_map["register"] = [this](http::request<http::dynamic_body>& request,
		std::unique_ptr<http::response<http::dynamic_body>>& response)->void {
			// 解析请求包 json 数据 
			std::string body = beast::buffers_to_string(request.body().data());
			boost::json::object json_data = boost::json::parse(body).as_object();
			std::string name = std::string(json_data["name"].as_string().c_str());
			std::string email = std::string(json_data["email"].as_string().c_str());
			std::string pwd = std::string(json_data["pass_word"].as_string().c_str());


			//验证码是否正确
			std::string varify_code = std::string(json_data["varify_code"].as_string().c_str());
			// 验证码不正确回包
			if (_varify_code != varify_code) {
				std::cout << "Invalid verification code.";
				// 状态行
				response->result(http::status::ok); // 设置状态码
				response->version(request.version());// 设置版本
				//响应体
				boost::json::object data;
				data["error_code"] = (int)ErrorCodes::varify_faild;
				beast::ostream(response->body()) << data;

				//响应头
				response->set(http::field::content_type, "application/json");
				response->content_length(response->body().size());  // 设置回包长度 //需要在添加包的数据之后
				
				
				
			}
			mysql_mgr::get_instance().register_user(name,email,pwd);
			// 成功注册回应
			boost::json::object data;
			data["error_code"] = static_cast<int>(ErrorCodes::success);
			create_response(response,data);
		};
	logic_map["get_varify_code"] = [this](http::request<http::dynamic_body>& request,
		std::unique_ptr<http::response<http::dynamic_body>>& response)->void {
			std::cout << "send email code" << std::endl;
			std::string body = beast::buffers_to_string(request.body().data());
			boost::json::object json_data = boost::json::parse(body).as_object();
			std::string email = std::string(json_data["email"].as_string().c_str());
			std::string varify_code = email::get_instance().send_email_verify_code(email);
			if (varify_code == "") {
				boost::json::object data;
				data["error_code"] = static_cast<int>(ErrorCodes::success);
				create_response(response,data);
			}
			_varify_code = varify_code;
			// 成功发送回应
			boost::json::object data;
			data["error_code"] = static_cast<int>(ErrorCodes::success);
			create_response(response, data);
			
		};
}

logic::logic()
{
	init_logic();
}
