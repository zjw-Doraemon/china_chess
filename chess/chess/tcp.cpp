#include "tcp.h"
#include <iostream>
tcp::tcp(QObject *parent): QObject{parent}
{
    
    
    // ��������
    // �źŴ����п��ܻ�������ճ�������»�������ֻ��һ���������ݡ�
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
                // ��Ա䳤�������ͻ��������ǰ׺ �����ַ�����
                QDataStream stream(&recieve_buffer, QIODevice::ReadWrite);
                // �����ֽ���Ϊ����ֽ��������ֽ���
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
               
                // �źŴ����п��ܻ�������ճ�������»�������ֻ��һ���������ݡ�
                QJsonObject json = QJsonDocument::fromJson(recieve_buffer.left(body_size)).object(); // ֮ǰ�����ǽ�������������������Ҫֻ������ǰ�����json��������������������ݰ�ʱ���������
                /*if () {

                }*/
                std::cout << " ----------------------------------------------------------------------------------------------------------" << std::endl;
                std::cout << "body data " << recieve_buffer.left(body_size).toStdString() << std::endl;
                std::cout << " ----------------------------------------------------------------------------------------------------------" << std::endl;
                handle(json);
                recieve_buffer.remove(0, body_size);
                body_size = 0; //���� �����С
                
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
    // �����ȱ���Ϊ 4 �ֽڴ�˸�ʽ�������ֽ���
    uint32_t length = static_cast<uint32_t>(json_data.size());
    //std::cout << length << std::endl;
    QByteArray header(4, 0); // ���� 4 �ֽڻ�����
    QDataStream ds(&header, QIODevice::WriteOnly);
    ds.setByteOrder(QDataStream::BigEndian); // ͳһΪ���
    ds << length;
    // ����ͷ����Ϣ
    if (_socket.write(header) != header.size()) {
        // ������ͷ����Ϣʧ�ܵ����
        std::cout << "send faild" << std::endl;
        return;
    }

    // ���� JSON ����
    if (_socket.write(json_data) != json_data.size()) {
        // ������ JSON ����ʧ�ܵ����
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

