#include "tcp.h"
#include <iostream>
tcp::tcp(QObject *parent): QObject{parent}
{
    
    
    // 接收数据
    // 信号触发有可能缓冲区会粘包，导致缓冲区不只有一个包的数据。
    connect(&_socket, &QTcpSocket::readyRead, this, [this] {
        recieve_buffer.append(_socket.readAll());
        
        //std::cout << "recieve data size " << recieve_buffer.size() << std::endl;
        //std::cout <<"buffer  data "<< recieve_buffer.toStdString() << std::endl;
        while (1) {
            
            if (body_size == 0) {
                if (recieve_buffer.size() < head_size) {
                    break;
                }
                
                //std::cout << recieve_buffer.size() << std::endl;
                // 针对变长数据类型会自行添加前缀 例如字符串等
                QDataStream stream(&recieve_buffer, QIODevice::ReadWrite);
                // 设置字节序为大端字节序（网络字节序）
                stream.setByteOrder(QDataStream::BigEndian);
                stream >> body_size;
                std::cout << "data size" << body_size << std::endl;
                recieve_buffer.remove(0,head_size);
            }
            if (body_size != 0) {

                //std::cout << recieve_buffer.size() << std::endl;
                if (recieve_buffer.size() < body_size) {
                    break;
                }
               
                // 信号触发有可能缓冲区会粘包，导致缓冲区不只有一个包的数据。
                QJsonObject json = QJsonDocument::fromJson(recieve_buffer.left(body_size)).object(); // 之前代码是解析了整个缓冲区，需要只解析当前包体的json，如果缓冲区到达多个数据包时会解析错误。
                /*if () {

                }*/
                std::cout << " ----------------------------------------------------------------------------------------------------------" << std::endl;
                std::cout << "body data " << recieve_buffer.left(body_size).toStdString() << std::endl;
                std::cout << " ----------------------------------------------------------------------------------------------------------" << std::endl;
                handle(json);
                recieve_buffer.remove(0, body_size);
                body_size = 0; //重置 包体大小
                
            }

            

        }


        });

}
void tcp::tcp_connect(QString host, uint16_t port)
{
    _socket.connectToHost(host,port);
    
}

void tcp::send_json(QJsonObject json )
{
    
    QByteArray json_data = QJsonDocument(json).toJson();
    //std::cout << "tcp send data" << std::endl;
    //std::cout << json_data.toStdString() << std::endl;
    // 将长度编码为 4 字节大端格式（网络字节序）
    uint32_t length = static_cast<uint32_t>(json_data.size());
    //std::cout << length << std::endl;
    QByteArray header(4, 0); // 分配 4 字节缓冲区
    QDataStream ds(&header, QIODevice::WriteOnly);
    ds.setByteOrder(QDataStream::BigEndian); // 统一为大端
    ds << length;
    // 发送头部信息
    if (_socket.write(header) != header.size()) {
        // 处理发送头部信息失败的情况
        std::cout << "send faild" << std::endl;
        return;
    }

    // 发送 JSON 数据
    if (_socket.write(json_data) != json_data.size()) {
        // 处理发送 JSON 数据失败的情况
        std::cout << "send faild" << std::endl;
        return;
    }
    //std::cout << "send success" << std::endl;

}

QTcpSocket& tcp::get_socket()
{
    return _socket;
}

void tcp::handle(QJsonObject json)
{
    QString model = json["model"].toString();
    if (model == "login") {
        //std::cout << json["error_code"].toString().toStdString() << std::endl;
        emit tcp_finish_login(json);
        return;
    }
    if (model == "friend_model") {

        //std::cout << QJsonDocument(json).toJson().toStdString() << std::endl;
        emit tcp_finish_friend(json);
        return;
    }
    if (model == "web_board_model") {
        //std::cout << QJsonDocument(json).toJson().toStdString() << std::endl;
        emit tcp_finish_web_board(json);
        return;

    }
    if (model == "welcome_model") {
        emit tcp_finish_welcome(json);
        return;
    }

}

