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
	logic_map["register"] = [this](http::request<http::dynamic_body>& request,
		std::unique_ptr<http::response<http::dynamic_body>>& response)->void {
			// ��������� json ���� 
			std::string body = beast::buffers_to_string(request.body().data());
			boost::json::object json_data = boost::json::parse(body).as_object();
			std::string name = std::string(json_data["name"].as_string().c_str());
			std::string email = std::string(json_data["email"].as_string().c_str());
			std::string pwd = std::string(json_data["pass_word"].as_string().c_str());


			//��֤���Ƿ���ȷ
			std::string varify_code = std::string(json_data["varify_code"].as_string().c_str());
			// ��֤�벻��ȷ�ذ�
			if (_varify_code != varify_code) {
				std::cout << "Invalid verification code.";
				// ״̬��
				response->result(http::status::ok); // ����״̬��
				response->version(request.version());// ���ð汾
				//��Ӧ��
				boost::json::object data;
				data["error_code"] = (int)ErrorCodes::varify_faild;
				beast::ostream(response->body()) << data;

				//��Ӧͷ
				response->set(http::field::content_type, "application/json");
				response->content_length(response->body().size());  // ���ûذ����� //��Ҫ����Ӱ�������֮��
				
				
				
			}
			mysql_mgr::get_instance().register_user(name,email,pwd);
			// �ɹ�ע���Ӧ
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
			// �ɹ����ͻ�Ӧ
			boost::json::object data;
			data["error_code"] = static_cast<int>(ErrorCodes::success);
			create_response(response, data);
			
		};
}

logic::logic()
{
	init_logic();
}
