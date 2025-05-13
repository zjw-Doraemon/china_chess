#include "welcome.h"
#include "ui_welcome.h"
#include <iostream>
#include "http.h"
#include "global.h"
#include "tcp.h"
#include "user_data.h"
welcome::welcome(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::welcome)

{
    ui->setupUi(this);
    this->setFixedSize(300,500);
    connect(ui->web_test,&QPushButton::clicked,this,&welcome::web_test);
    connect(&tcp::get_instance(), &tcp::tcp_finish_welcome, this, [] {
        std::cout << "revieve one data" << std::endl;
        });
    //std::cout << "welcom";
}

welcome::~welcome()
{
    delete ui;
}

void welcome::on_local_mode_clicked()
{
    emit show_board();
}


void welcome::web_test()
{
    QJsonObject data;
    data["action"] = "web_test";
    data["from_user"] = UserMgr::get_instance().get_me_user().name;
    tcp::get_instance().send_json(data);
    tcp::get_instance().send_json(data);
}

void welcome::on_friend_mode_clicked()
{
    emit switch_friend_list();
}

