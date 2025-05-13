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
    // ��ȡ��֤��
    connect(ui->getVerificationCodeButton, &QPushButton::clicked,this,&register_dialog::get_varify_code);
    //����ע������
    connect(ui->sure_btu, &QPushButton::clicked, this, &register_dialog::send_register);
    // �л���¼����
    connect(ui->cancel_btn,&QPushButton::clicked,this,&register_dialog::switch_log_dialog);

    // ����http�ظ�
    connect(&http::get_instance(), &http::http_register_finish, this, [this](QJsonObject json) {
        //std::cout << json["error_code"].toInt() << std::endl;
        uint8_t error_code =  json["error_code"].toInt();  // ֱ�Ӿ�����������
        if (error_code != static_cast<int>(ErrorCodes::success)) {
            // ��ʾ��Ϣ��ʾ��
            QMessageBox::information(this, "tip", "faild");
            return;
        }
        // ��ʾ��Ϣ��ʾ��
        QMessageBox::information(this, "tip", "success");
        });
}

void register_dialog::get_varify_code()
{
    std::cout << "get_varify_code" << std::endl;
    QJsonObject varify;
    varify["action"] = "get_varify_code";
    varify["email"] = ui->email_edit->text();
    http::get_instance().post(QUrl(), varify, Model::register_model);
}

void register_dialog::send_register()
{

   
    QJsonObject register_msg;
    register_msg["action"] = "register";
    register_msg["name"] = ui->user_edit->text();
    register_msg["email"] = ui->email_edit->text();
    register_msg["pass_word"] = ui->pass_edit->text();
    register_msg["varify_code"] = ui->varify_edit->text();
    http::get_instance().post(QUrl(), register_msg, Model::register_model);

}


register_dialog::~register_dialog() {
    delete ui;
}