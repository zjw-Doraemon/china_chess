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
	/*���� 2��ԭʼ���������
	ԭʼ����ֱ�ӽ����� _response ���ƶ�����Ȩ��
	http::async_write(_socket, *_response, [self, response = std::move(_response)] { ... });
	������ֵ˳��δ���壺C++ ����֤������������ֵ˳�򡣿����ڽ����� *_response ǰ��_response �ѱ��ƶ��� Lambda�����½����ÿ�ָ�롣
	δ������Ϊ����� _response �ڽ�����ǰ���ƶ���*_response �ᴥ��������
	���� 3����������Ϊ�ΰ�ȫ
	������Ĵ��������ָ���ȡ������Ȩת�ƣ�
	auto* resp_ptr = _response.get();  // 1. �Ȼ�ȡԭʼָ�루��ʱ _response δ�ƶ���
	http::async_write(
		_socket, 
		*resp_ptr,                     // 2. ʹ��ԭʼָ�������
		[self, resp = std::move(_response)] { ... }  // 3. ���ƶ�����Ȩ
	);
	�ؼ��㣺
	��ȡԭʼָ��ʱ������Ч��
	�� resp_ptr = _response.get() ʱ��_response ��δ�ƶ���resp_ptr ָ����Ч����

	����Ȩת���ڽ�����֮��
	ͨ����ȷ˳��async_write ʹ�� *resp_ptr ʱ���������� _response ����δ���ƶ���

	Lambda ��������Ȩ��
	resp = std::move(_response) ������Ȩת�Ƹ� Lambda��ȷ���������첽�����ڼ��*/
	// response = std::move(_response)  �Զ�����һ���µı������Զ��Ƶ����ͣ�Ȼ��_responseת�ƽ�ȥ
	// �Ȼ�ȡԭʼָ�룬���ƶ�����Ȩ
	auto* resp_ptr = _response.get();
	http::async_write(_socket, *resp_ptr, [self, response = std::move(_response)](beast::error_code ec,
		std::size_t bytes_transferred) {
			self->_socket.shutdown(tcp::socket::shutdown_send, ec);
			std::cout << "reply success !" << std::endl;
		});
}