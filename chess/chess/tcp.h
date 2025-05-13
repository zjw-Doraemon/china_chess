#ifndef TCP_H
#define TCP_H

#include <QObject>
#include "global.h"
class tcp : public QObject , public single_class<tcp>
{
    Q_OBJECT
public:
    friend single_class<tcp>;
    explicit tcp(QObject *parent = nullptr);
    void tcp_connect(QString host, uint16_t port);
    void send_json(QJsonObject json );
    QTcpSocket& get_socket();
private:
    void handle(QJsonObject json);
    QTcpSocket _socket;
    // char data[4];
    uint32_t head_size = 4; //头的字节数
    uint32_t body_size = 0;
    QByteArray recieve_buffer;
signals:
    void tcp_finish_login(QJsonObject json);
    void tcp_finish_friend(QJsonObject json);
    void tcp_finish_web_board(QJsonObject json);
    void tcp_finish_welcome(QJsonObject json);
};

#endif // TCP_H
