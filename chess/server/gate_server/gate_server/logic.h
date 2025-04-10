#include "global.h"
#include "HttpConnection.h"
class logic : public single_class<logic>
{
	friend class single_class<logic>;
	// ���ֵ���� ���ǽ�������������ڽ��� ���ݵ��ĺ������� �����ֵ���þ���ԭ���ĺ�������
	using handle = std::function
		<void(http::request<http::dynamic_body>& request,
		std::unique_ptr<http::response<http::dynamic_body>>&)>;
	
public:
	// ��json�����õ���Ӧ����
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