#include "logindialog.h"
#include "ui_logindialog.h"
logindialog::logindialog(QWidget *parent)
    : QDialog{parent}
    ,ui(new Ui::LoginDialog())

{
    setFixedSize(300,500);
    ui->setupUi(this);
    connect(ui->sign,&QPushButton::clicked,this,&logindialog::switch_register);
}
logindialog::~logindialog() {
    delete ui;
}