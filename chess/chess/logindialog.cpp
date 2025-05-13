#include "logindialog.h"
#include "ui_logindialog.h"
#include "http.h"
#include "iostream"
#include "qmessagebox.h"
#include "tcp.h"
#include "user_data.h"
logindialog::logindialog(QWidget *parent)
    : QDialog{parent}
    ,ui(new Ui::LoginDialog())

{
    

    
    setFixedSize(300,500);
    ui->setupUi(this);
    connect(ui->sign,&QPushButton::clicked,this,&logindialog::switch_register);
    // 发送登录请求
    connect(ui->login, &QPushButton::clicked,this, &logindialog::login);
    // 处理返回信息
    connect(&http::get_instance(), &http::http_login_finish, this, [this](QJsonObject json) {
        // gate server
        //std::cout << "server information :" << std::endl;
        //std::cout << QJsonDocument(json).toJson().toStdString();
        uint32_t error_code = json["error_code"].toInt();  // 直接就是数字类型
        QString host = json["host"].toString();
        uint16_t port = json["port"].toInt();
        if (error_code != static_cast<int>(ErrorCodes::success)) {
            // 显示信息提示框
            QMessageBox::information(this, "tip", "gateserver faild");
            return;
        }
        
        // 登录
        tcp::get_instance().tcp_connect(host, port);
       
        
        });
    // tcp server
    //tcp server 

        //服务器断开
    // 正常
    connect(&tcp::get_instance().get_socket(), &QTcpSocket::disconnected, [] {
        std::cout << "server normol close" << std::endl;
        });
    //异常 连接错误
    connect(&tcp::get_instance().get_socket(), &QTcpSocket::errorOccurred, [](QAbstractSocket::SocketError socketError) {

        std::cout << "error： " << tcp::get_instance().get_socket().errorString().toStdString() << std::endl;
        });
    //连接服务器成功
    QObject::connect(&tcp::get_instance().get_socket(), &QTcpSocket::connected, [this]() {
        //std::cout<<"connect success start login"<<std::endl;
        QJsonObject data;
        data["action"] = "login";
        data["email"] = ui->email_edit->text();
        data["pass_word"] = ui->key_word->text();
        tcp::get_instance().send_json(data);

        });
    connect(&tcp::get_instance(), &tcp::tcp_finish_login, this, [this](QJsonObject json) {
        int error_code = json["error_code"].toInt();
        QString name = json["name"].toString();
        if (error_code != static_cast<int>(ErrorCodes::success)) {
            QMessageBox::information(this, "tip", "gateserver faild");
            return;
        }
        // 设置自己的信息
        UserInfo user(name,ui->email_edit->text());
        UserMgr::get_instance().set_me_user(user);
        emit switch_welcome();
        });
    connect(ui->forget, &QPushButton::clicked, this, [] {
        //std::cout << "hello" << std::endl;
        QJsonObject data;
        data["action"] = "login";
        tcp::get_instance().send_json(data);
        });
}
logindialog::~logindialog() {
    delete ui;
}

void logindialog::login()
{
    QJsonObject register_msg;
    register_msg["action"] = "login";
    register_msg["email"] = ui->email_edit->text();
    register_msg["pass_word"] = ui->key_word->text();
    http::get_instance().post(QUrl(), register_msg, Model::logic_model);
}
