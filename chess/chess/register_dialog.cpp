#include "register_dialog.h"
#include "ui_register_dialog.h"
#include "http.h"
#include "iostream"
#include "qmessagebox.h"
register_dialog::register_dialog(QWidget* parent)
    :QDialog(parent),ui(new Ui::register_dialog())
{

    ui->setupUi(this);
    setFixedSize(300, 500);
    connect(ui->getVerificationCodeButton, &QPushButton::clicked,this,&register_dialog::get_varify_code);
    connect(&http::get_instance(), &http::http_register_finish, this, [](QJsonObject json) {
        std::cout << json["error_code"].toString().toStdString() << std::endl;
        uint8_t error_code = std::stol( json["error_code"].toString().toStdString());  //json的string，转换为qt的，最后转换为c++的
        if (error_code != static_cast<int>(ErrorCodes::success)) {
            // 显示信息提示框
            QMessageBox::information(nullptr, "tip", "email send faild");
            return;
        }
        // 显示信息提示框
        QMessageBox::information(nullptr, "tip", "email send success");
        });
}

void register_dialog::get_varify_code()
{
    std::cout << "get_varify_code" << std::endl;
    QJsonObject varify;
    varify["action"] = "get_varify_code";
    varify["name"] = ui->user_edit->text();
    varify["email"] = ui->email_edit->text();
    varify["pass_word"] = ui->pass_edit->text();
    http::get_instance().post(QUrl(), varify, Model::register_model);
}


register_dialog::~register_dialog() {
    delete ui;
}