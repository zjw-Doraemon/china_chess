#pragma once
#include <boost/asio.hpp>  // ��Ҫ�� Asio ����
#include <boost/beast.hpp> // http ����

// json
//#include <boost/json/src.hpp>
#include <boost/json.hpp>
// �����ļ� ini
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/ini_parser.hpp>
//mysqlconnection ��
#include <mysql_connection.h>
#include <mysql_driver.h>
#include <cppconn/driver.h>
#include <cppconn/prepared_statement.h>
#include <cppconn/resultset.h>
#include <cppconn/statement.h>
#include <cppconn/exception.h>


// ͨ��ͷ�ļ�
#include "SingleClass.h"
#include <iostream>
#include <functional>
#include <map>
#include <vector>
#include <queue>
#include <chrono>
#include <thread>


// ʱ��ͷ�ļ�
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
	register_failed = 1001, // ע�����
	send_varify_failed = 1002, // ������֤�����
	varify_faild = 1003,
	login_faild = 1004,
};

using defer_function = std::function<void()>;
class defer {
public:
	defer(defer_function);
	~defer();
private:
	defer_function end;  // ���Ը�Ϊ��uniqueָ����У�δ��
};

std::string get_now();
