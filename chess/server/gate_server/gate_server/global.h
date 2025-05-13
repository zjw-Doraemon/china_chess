#pragma once
#include <boost/asio.hpp>  // 主要的 Asio 功能
#include <boost/beast.hpp> // http 功能

// json
//#include <boost/json/src.hpp>
#include <boost/json.hpp>
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


// 通用头文件
#include "SingleClass.h"
#include <iostream>
#include <functional>
#include <map>
#include <vector>
#include <queue>
#include <chrono>
#include <thread>


// 时间头文件
#include <iomanip>
#include <ctime>
#include <chrono>
#include <sstream>
#include <cstdlib>
namespace beast = boost::beast;         // from <boost/beast.hpp>
namespace http = beast::http;           // from <boost/beast/http.hpp>
namespace net = boost::asio;            // from <boost/asio.hpp>
using tcp = boost::asio::ip::tcp;       // from <boost/asio/ip/tcp.hpp>
enum class ErrorCodes {
	success = 0,
	register_failed = 1001, // 注册错误
	send_varify_failed = 1002, // 发送验证码错误
	varify_faild = 1003,
	login_faild = 1004,
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
