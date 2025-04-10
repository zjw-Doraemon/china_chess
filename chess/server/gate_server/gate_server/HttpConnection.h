#pragma once
#include "global.h"

class HttpConnection : public std::enable_shared_from_this<HttpConnection> {  // 少了个public，纠错废了老半天劲
public:
	HttpConnection(boost::asio::io_context& ioc);
	boost::asio::ip::tcp::socket& get_socket();
	void Start();
	void handle_http();
	~HttpConnection();   // 使用 shared指针管理，必须共有 shared 调用是需要有权限
	
private:
	
	void WriteResponse(std::unique_ptr<http::response<http::dynamic_body>>);
	boost::asio::ip::tcp::socket _socket;
	// 异步读所需参数
	beast::flat_buffer _buffer;
	http::request<http::dynamic_body> _request;//请求包
	

};