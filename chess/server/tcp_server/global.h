#pragma once
// boost
#include <boost/asio.hpp>  // 主要的 Asio 功能
#include <boost/beast.hpp> // http 功能


// json
//#include <boost/json/src.hpp>
#include <boost/json.hpp>
namespace beast = boost::beast;         // from <boost/beast.hpp>
namespace http = beast::http;           // from <boost/beast/http.hpp>
namespace net = boost::asio;            // from <boost/asio.hpp>
using tcp = boost::asio::ip::tcp;       // from <boost/asio/ip/tcp.hpp>

// 配置文件 ini
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/ini_parser.hpp>

//mysqlconnection 库
#include <mysql_connection.h>
#include <mysql_driver.h>
#include <cppconn/driver.h>
#include <cppconn/prepared_statement.h>
#include <cppconn/resultset.h>
#include <cppconn/statement.h>
#include <cppconn/exception.h>

// 时间头文件
#include <iomanip>
#include <ctime>
#include <chrono>
#include <sstream>
#include <cstdlib>

enum class ErrorCode{
	success = 0,
	login_faild = 1004,
	no_find_request = 1005,
	add_friend_faild = 1006, //添加好友失败
	friend_offline = 1007,
};

using defer_function = std::function<void()>;
class defer {
public:
	defer(defer_function);
	~defer();
private:
	defer_function end;  // 试试改为用unique指针持有，未来
};

std::string get_now();