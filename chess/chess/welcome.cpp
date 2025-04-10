#include "welcome.h"
#include "ui_welcome.h"
#include <iostream>
#include "http.h"
#include "global.h"
welcome::welcome(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::welcome)
{
    ui->setupUi(this);
    this->setFixedSize(300,500);
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


void welcome::on_friend_mode_clicked()
{
    QJsonObject test;
    test["action"] = "register";
    http::get_instance().post(QUrl(),test,Model::friend_model);
}

