#include "tcp_acceptor.h"
#include "AsioIOServicePool.h"
#include "tcp_connection.h"
#include "mysql.h"
tcp_server::tcp_server(boost::asio::io_context& ioc, uint16_t port) :
	_ioc(ioc), _acceptor(ioc, boost::asio::ip::tcp::endpoint(boost::asio::ip::tcp::v4(), port))
{


}

void tcp_server::Start() {
	auto self = shared_from_this();
	auto ioc_context = AsioIOServicePool::get_instance().GetIOService();
	// 创建一个 tcp 连接类
	std::shared_ptr<tcp_connection> new_con = std::make_shared<tcp_connection>(*ioc_context);
	boost::asio::ip::tcp::socket& socket = new_con->get_socket();
	_acceptor.async_accept(socket, [self, new_con](const boost::system::error_code& ec) {
		if (ec) {
			self->Start();
			return;
		}
		// 添加服务器连接的用户
		/*UserInfo usr = mysql_mgr::get_instance().get_user_info();
		usr.set_connection(new_con);
		UserMgr::get_instance().add_user(usr);*/

		
		new_con->Start();
		//std::cout << "new connection" << std::endl;
		self->Start();
		});
}