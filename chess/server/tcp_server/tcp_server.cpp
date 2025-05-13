// tcp_server.cpp: 定义应用程序的入口点。
//

#include "tcp_server.h"
#include "global.h"
#include "tcp_acceptor.h"
using namespace std;

int main()
{
	boost::asio::io_context ioc{ 1 }; // 创建 io_context，并发提示值为 1
    // 获取监听 端口 
    boost::property_tree::ptree pt;
    boost::property_tree::ini_parser::read_ini("config.ini", pt);
    std::string portNum = pt.get<std::string>("tcp_server.Port");
    std::cout << "server start listen " << portNum << std::endl;
    unsigned long num = std::stoul(portNum); // 字符串转换为数字类型
    uint16_t port = (uint16_t)num;
    //创建服务器对象 准备监听
    std::shared_ptr<tcp_server> server = std::make_shared<tcp_server>(ioc, port);
    // 优雅退出服务器
    boost::asio::signal_set signals(ioc, SIGINT, SIGTERM);
    signals.async_wait(
        [&ioc](const boost::system::error_code& error, int signal_number) {
            if (error) {
                return;
            }
            ioc.stop();
        });
    // 服务器 开！
    server->Start();
    // ioc 事件循环开

    ioc.run();
}
