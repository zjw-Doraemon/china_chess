
#include "global.h"

class HttpServer : public std::enable_shared_from_this<HttpServer> {

public:
	HttpServer(boost::asio::io_context& ioc, uint16_t port);
	void Start();
private:
	boost::asio::ip::tcp::acceptor _acceptor;
	boost::asio::io_context& _ioc;
};