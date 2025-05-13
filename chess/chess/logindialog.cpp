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
    // ���͵�¼����
    connect(ui->login, &QPushButton::clicked,this, &logindialog::login);
    // ��������Ϣ
    connect(&http::get_instance(), &http::http_login_finish, this, [this](QJsonObject json) {
        // gate server
        //std::cout << "server information :" << std::endl;
        //std::cout << QJsonDocument(json).toJson().toStdString();
        uint32_t error_code = json["error_code"].toInt();  // ֱ�Ӿ�����������
        QString host = json["host"].toString();
        uint16_t port = json["port"].toInt();
        if (error_code != static_cast<int>(ErrorCodes::success)) {
            // ��ʾ��Ϣ��ʾ��
            QMessageBox::information(this, "tip", "gateserver faild");
            return;
        }
        
        // ��¼
        tcp::get_instance().tcp_connect(host, port);
       
        
        });
    // tcp server
    //tcp server 

        //�������Ͽ�
    // ����
    connect(&tcp::get_instance().get_socket(), &QTcpSocket::disconnected, [] {
        std::cout << "server normol close" << std::endl;
        });
    //�쳣 ���Ӵ���
    connect(&tcp::get_instance().get_socket(), &QTcpSocket::errorOccurred, [](QAbstractSocket::SocketError socketError) {

        std::cout << "error�� " << tcp::get_instance().get_socket().errorString().toStdString() << std::endl;
        });
    //���ӷ������ɹ�
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
        // �����Լ�����Ϣ
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
