#pragma once
#include "global.h"

class HttpConnection : public std::enable_shared_from_this<HttpConnection> {  // ���˸�public����������ϰ��쾢
public:
	HttpConnection(boost::asio::io_context& ioc);
	boost::asio::ip::tcp::socket& get_socket();
	void Start();
	void handle_http();
	~HttpConnection();   // ʹ�� sharedָ��������빲�� shared ��������Ҫ��Ȩ��
	
private:
	
	void WriteResponse(std::unique_ptr<http::response<http::dynamic_body>>);
	boost::asio::ip::tcp::socket _socket;
	// �첽���������
	beast::flat_buffer _buffer;
	http::request<http::dynamic_body> _request;//�����
	

};