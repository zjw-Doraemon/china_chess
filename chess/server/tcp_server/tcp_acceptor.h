#include "global.h"
#include <iostream>

class tcp_server: public std::enable_shared_from_this<tcp_server> {
public:
	tcp_server(boost::asio::io_context& ioc, uint16_t port);
	void Start();
private:
	boost::asio::ip::tcp::acceptor _acceptor;
	boost::asio::io_context& _ioc;
};