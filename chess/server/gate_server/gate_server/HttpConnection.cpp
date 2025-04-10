#include "HttpConnection.h"
#include "HttpConnection.h"
#include "logic.h"

HttpConnection::HttpConnection(boost::asio::io_context& ioc):_socket(ioc) {

}

boost::asio::ip::tcp::socket& HttpConnection::get_socket()
{
	return _socket;
}

void HttpConnection::Start()
{
	auto _self = shared_from_this();
	http::async_read(_socket, _buffer, _request, [_self,this](beast::error_code ec,
		std::size_t bytes_transferred) {
			if (ec) {
				
				std::cout << " http recieve error" << std::endl;
				std::cerr << "  Error code value: " << ec.value() << std::endl;
				std::cerr << "  Error category: " << ec.category().name() << std::endl;
				std::cerr << "  Error message: " << ec.message() << std::endl;
			}
			handle_http();
			
		});
	


}

void HttpConnection::handle_http()
{
	std::unique_ptr<http::response<http::dynamic_body>>  response;
	
	if (_request.method() == http::verb::get) {
		std::cout << "recieve get request" << std::endl;
	}
	if (_request.method() == http::verb::post) {
		response = logic::get_instance().handle_request(shared_from_this(),_request);
		
		
	}
	std::cout << beast::buffers_to_string(response->body().data()) << std::endl;
	WriteResponse(std::move(response));
}


HttpConnection::~HttpConnection()
{
	std::cout << "Destruct an HTTP connection  " << std::endl;
}



void HttpConnection::WriteResponse(std::unique_ptr<http::response<http::dynamic_body>>  _response)
{
	auto self = shared_from_this();
	/*步骤 2：原始代码的问题
	原始代码直接解引用 _response 并移动所有权：
	http::async_write(_socket, *_response, [self, response = std::move(_response)] { ... });
	参数求值顺序未定义：C++ 不保证函数参数的求值顺序。可能在解引用 *_response 前，_response 已被移动到 Lambda，导致解引用空指针。
	未定义行为：如果 _response 在解引用前被移动，*_response 会触发崩溃。
	步骤 3：修正代码为何安全
	修正后的代码分离了指针获取和所有权转移：
	auto* resp_ptr = _response.get();  // 1. 先获取原始指针（此时 _response 未移动）
	http::async_write(
		_socket, 
		*resp_ptr,                     // 2. 使用原始指针解引用
		[self, resp = std::move(_response)] { ... }  // 3. 后移动所有权
	);
	关键点：
	获取原始指针时对象有效：
	在 resp_ptr = _response.get() 时，_response 尚未移动，resp_ptr 指向有效对象。

	所有权转移在解引用之后：
	通过明确顺序，async_write 使用 *resp_ptr 时，对象仍由 _response 管理，未被移动。

	Lambda 捕获所有权：
	resp = std::move(_response) 将所有权转移给 Lambda，确保对象在异步操作期间存活。*/
	// response = std::move(_response)  自动创建一个新的变量，自动推导类型，然后将_response转移进去
	// 先获取原始指针，再移动所有权
	auto* resp_ptr = _response.get();
	http::async_write(_socket, *resp_ptr, [self, response = std::move(_response)](beast::error_code ec,
		std::size_t bytes_transferred) {
			self->_socket.shutdown(tcp::socket::shutdown_send, ec);
			std::cout << "reply success !" << std::endl;
		});
}