#include "logic.h"
#include "mysql.h"
#include "email.h"
void logic::create_response(std::unique_ptr<http::response<http::dynamic_body>>& response, boost::json::object &json_data)
{
	// ״̬��
	response->result(http::status::ok); // ����״̬��
	response->version(11);// ���ð汾
	//��Ӧ��
	std::string data = boost::json::serialize(json_data);
	beast::ostream(response->body()) << data;

	//��Ӧͷ
	response->set(http::field::content_type, "application/json");
	response->content_length(response->body().size());  // ���ûذ����� //��Ҫ����Ӱ�������֮��
}
std::unique_ptr<http::response<http::dynamic_body>> logic::handle_request(
	std::shared_ptr<HttpConnection> connect,
	http::request<http::dynamic_body>& request)
{
	std::unique_ptr<http::response<http::dynamic_body>> _response(new http::response<http::dynamic_body>()); // ����һ���ذ�
	// ��������� json ���ݣ��鿴��Ҫʲô����
	std::string body = beast::buffers_to_string(request.body().data());
	boost::json::object json_data = boost::json::parse(body).as_object();
	//std::cout << json_data["action"].as_string().c_str() << std::endl;
	
	logic_map[std::string(json_data["action"].as_string().c_str())](request, _response); // ��Ĭ�ϵ���ʽת�� c �ַ������� string ����  ���� �߼�����
	return _response;
}

void logic::init_logic() {
	logic_map["login"] = [this](http::request<http::dynamic_body>& request,
		std::unique_ptr<http::response<http::dynamic_body>>& response)->void {
			try {// ��������� json ���� 
				std::string body = beast::buffers_to_string(request.body().data());
				boost::json::object json_data = boost::json::parse(body).as_object();
				std::string email = std::string(json_data["email"].as_string().c_str());
				std::string pwd = std::string(json_data["pass_word"].as_string().c_str());

				bool is_success = mysql_mgr::get_instance().login( email, pwd);
				if (is_success == false) {
					boost::json::object data;
					data["error_code"] = static_cast<int>(ErrorCodes::register_failed);
					create_response(response, data);
					return;
				}
			}
			catch (...) {
				std::cout << "json parse faild" << std::endl;
				boost::json::object data;
				data["error_code"] = static_cast<int>(ErrorCodes::register_failed);
				create_response(response, data);
				return;
			}
			// �ɹ���Ӧ
			boost::property_tree::ptree pt;
			boost::property_tree::ini_parser::read_ini("config.ini", pt);
			std::string portNum = pt.get<std::string>("tcp_server_1.Port");
			unsigned long num = std::stoul(portNum); // �ַ���ת��Ϊ��������
			uint16_t port = (uint16_t)num; // �˿�
			std::string host = pt.get<std::string>("tcp_server_1.ip"); // ip


			boost::json::object data;
			data["error_code"] = static_cast<int>(ErrorCodes::success);
			data["host"] = host;
			data["port"] = port;
			create_response(response, data);

		};




	logic_map["register"] = [this](http::request<http::dynamic_body>& request,
		std::unique_ptr<http::response<http::dynamic_body>>& response)->void {
			try{// ��������� json ���� 
				std::string body = beast::buffers_to_string(request.body().data());
				boost::json::object json_data = boost::json::parse(body).as_object();
				std::string name = std::string(json_data["name"].as_string().c_str());
				std::string email = std::string(json_data["email"].as_string().c_str());
				std::string pwd = std::string(json_data["pass_word"].as_string().c_str());
				

				//��֤���Ƿ���ȷ
				std::string varify_code = std::string(json_data["varify_code"].as_string().c_str());
				// ��֤�벻��ȷ�ذ�
				if (_varify_code != varify_code) {
					std::cout << "Invalid verification code" << std::endl;
					boost::json::object data;
					data["error_code"] = static_cast<int>(ErrorCodes::send_varify_failed);
					create_response(response, data);
					return;
				}
				bool is_success = mysql_mgr::get_instance().register_user(name, email, pwd);
				if (is_success == false) {
					boost::json::object data;
					data["error_code"] = static_cast<int>(ErrorCodes::register_failed);
					create_response(response, data);
					return;
				}
			}
			catch (...) {
				std::cout << "json parse faild" << std::endl;
				boost::json::object data;
				data["error_code"] = static_cast<int>(ErrorCodes::register_failed);
				create_response(response, data);
				return;
			}
			
			
			// �ɹ�ע���Ӧ
			boost::json::object data;
			data["error_code"] = static_cast<int>(ErrorCodes::success);
			create_response(response,data);
		};
	logic_map["get_varify_code"] = [this](http::request<http::dynamic_body>& request,
		std::unique_ptr<http::response<http::dynamic_body>>& response)->void {
			try{
				//std::cout << "send email code" << std::endl;
				std::string body = beast::buffers_to_string(request.body().data());
				boost::json::object json_data = boost::json::parse(body).as_object();
				std::string email = json_data["email"].as_string().c_str();
				std::string varify_code = email::get_instance().send_email_verify_code(email);
				if (varify_code == "") {
					boost::json::object data;
					data["error_code"] = static_cast<int>(ErrorCodes::send_varify_failed);
					create_response(response, data);
					return;
				}
				_varify_code = varify_code;
				// �ɹ����ͻ�Ӧ
				boost::json::object data;
				data["error_code"] = static_cast<int>(ErrorCodes::success);
				create_response(response, data);
			}
			catch (...) {
				std::cout << "get_varify_code error" << std::endl;
				boost::json::object data;
				data["error_code"] = static_cast<int>(ErrorCodes::send_varify_failed);
				create_response(response, data);
			}
			
			
		};
	logic_map["search_friend"] = [this](http::request<http::dynamic_body>& request,
		std::unique_ptr<http::response<http::dynamic_body>>& response)->void {
			try {
				std::string body = beast::buffers_to_string(request.body().data());
				boost::json::object json_data = boost::json::parse(body).as_object();
				std::string friend_name = json_data["friend_name"].as_string().data();
				std::string request = json_data["request"].as_string().data();
				bool success = mysql_mgr::get_instance().search_friend(friend_name);
				if (!success) {
					boost::json::object data;
					data["error_code"] = static_cast<int>(ErrorCodes::send_varify_failed);
					create_response(response, data);
				}
				// �ɹ����ͻ�Ӧ
				boost::json::object data;
				data["error_code"] = static_cast<int>(ErrorCodes::success);
				data["request"] = request;
				data["friend_name"] = friend_name;
				create_response(response, data);
			}
			catch (...) {
				std::cout << "search friend error" << std::endl;
				boost::json::object data;
				data["error_code"] = static_cast<int>(ErrorCodes::send_varify_failed);
				create_response(response, data);
			}
		};
	
	logic_map["get_friend_list"] = [this](http::request<http::dynamic_body>& request,
		std::unique_ptr<http::response<http::dynamic_body>>& response)->void {
			try {
				std::string body = beast::buffers_to_string(request.body().data());
				boost::json::object json_data = boost::json::parse(body).as_object();
				std::string request = json_data["request"].as_string().data();
				std::string name = json_data["name"].as_string().data();
				boost::json::array array_json = mysql_mgr::get_instance().get_friend_list(name);
				if (array_json.empty()) {
					boost::json::object data;
					data["error_code"] = static_cast<int>(ErrorCodes::success);
					data["request"] = request;
					create_response(response, data);
				}
				// ���������Ϣ
				boost::json::array all_user_data;
				for (auto c : array_json) {
					boost::json::object user;
					user["email"] = mysql_mgr::get_instance().get_email(c.as_string().data());
					user["name"] = c.as_string().data();
					all_user_data.push_back(boost::json::value(user));
				}
				// �ɹ����ͻ�Ӧ
				boost::json::object data;
				data["error_code"] = static_cast<int>(ErrorCodes::success);
				data["request"] = request;
				data["friend_array"] = all_user_data;
				create_response(response, data);
			}
			catch (...) {
				std::cout << "search friend error" << std::endl;
				boost::json::object data;
				data["error_code"] = static_cast<int>(ErrorCodes::send_varify_failed);
				create_response(response, data);
			}
		};
}

logic::logic()
{
	init_logic();
}
