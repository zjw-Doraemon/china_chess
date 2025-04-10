// gate_server.cpp: 定义应用程序的入口点。
//
#include "global.h"
#include "gate_server.h"
#include <unistd.h>  // getcwd
#include <limits.h>  // PATH_MAX
#include "HttpServer.h"
int main() {
    boost::asio::io_context ioc{1}; // 要求 单线程模式运行 
    // 获取监听 端口 
    boost::property_tree::ptree pt;
    boost::property_tree::ini_parser::read_ini("config.ini", pt);
    std::string portNum = pt.get<std::string>("Gateserver.Port");
    std::cout << "server start listen " << portNum << std::endl;
    unsigned long num = std::stoul(portNum); // 字符串转换为数字类型
    uint16_t port = (uint16_t)num;
    //创建服务器对象 准备监听
    std::shared_ptr<HttpServer> server = std::make_shared<HttpServer>(ioc,port);
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
    ioc.run();
    
}
    