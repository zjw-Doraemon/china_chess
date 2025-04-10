#include "HttpServer.h"
#include "AsioIOServicePool.h"
#include "HttpConnection.h"
HttpServer::HttpServer(boost::asio::io_context& ioc, uint16_t port):
	_ioc(ioc), _acceptor(ioc, boost::asio::ip::tcp::endpoint(boost::asio::ip::tcp::v4(), port))
{
	

}

void HttpServer::Start() {
	auto self = shared_from_this();
	auto ioc_context =  AsioIOServicePool::get_instance().GetIOService();
	// 创建一个 http 连接类
	std::shared_ptr<HttpConnection> new_con = std::make_shared<HttpConnection>(*ioc_context);
	boost::asio::ip::tcp::socket& socket = new_con->get_socket();
	_acceptor.async_accept(socket,[self,new_con](const boost::system::error_code& ec){
		if (ec) {
			
			self->Start();
			return;
		}
		
		new_con->Start();
		self->Start();
	});
}