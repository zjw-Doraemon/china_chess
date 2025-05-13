#include "friend_list.h"
#include "ui_friend_list.h"
#include "friend_item.h"
#include "QJsonObject"
#include "http.h"
#include "tcp.h"
#include "iostream"
#include "user_data.h"
#include "qjsonarray.h"
friend_list::friend_list(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::friend_list)
{
    setFixedSize(300,500);
    ui->setupUi(this);
    ui->search_list->hide();
    // 搜索好友通知 ，好友列表通知 等
    connect(&http::get_instance(), &http::http_friend_finish, this, [this](QJsonObject json) {
        QString request = json["request"].toString();
        uint8_t error_code = json["error_code"].toInt();  // 直接就是数字类型
        
        if (request == "search_friend") {
            QString friend_name = json["friend_name"].toString();
            if (error_code != static_cast<int>(ErrorCodes::success)) {
                // 显示信息提示框
                QMessageBox::information(this, "tip", "faild");
                return;
            }

            ui->search_list->show();
            ui->list_friend->hide();
            add_search_item(friend_name);
            // 显示信息提示框
            QMessageBox::information(this, "tip", "success");
        }
        if (request == "get_friend_list") {
            QJsonArray array_json = json["friend_array"].toArray();
            for (auto c : array_json) {
                UserMgr::get_instance().add_friend(c.toObject()["name"].toString(), c.toObject()["email"].toString());
                add_item(c.toObject()["name"].toString());
            }
        }
        
        });
    // 收到好友请求信息
    connect(&tcp::get_instance(), &tcp::tcp_finish_friend, this, [this](QJsonObject json) {
        QString action = json["action"].toString();
        if (handle_response_map.find(action) == handle_response_map.end()) {
            //std::cout << "action no find" << std::endl;
            return;
        }
        handle_response_map[action](json);
        });

    // 信号永远都是公有的，它可以在任何地方发射信号，也可以在任何地方连接信号。
    // 槽函数就是普通的函数， 它的前面的权限 ，和普通函数一样，它只要可以被访问就可以被连接，被调用。

    init_response_map();
}

friend_list::~friend_list()
{
    delete ui;
}

void friend_list::add_search_item(QString name)
{

    friend_item* customWidget = new friend_item(this);
    customWidget->hide_battle_btn();
    customWidget->set_friend_name(name);
    QListWidgetItem* item = new QListWidgetItem(ui->search_list);
    // 设置 item 的大小，使其能容纳自定义 widget
    item->setSizeHint(QSize(ui->search_list->width(), customWidget->height()));
    ui->search_list->setItemWidget(item, customWidget);
}
void friend_list::init_friend_list()
{
    QJsonObject data;
    data["action"] = "get_friend_list";
    data["request"] = "get_friend_list";
    data["name"] = UserMgr().get_instance().get_me_user().name;
    http::get_instance().post(QUrl(), data, Model::friend_model);
}
void friend_list::add_item(QString name)
{
    
    
    friend_item* customWidget = new friend_item(this);
    customWidget->set_friend_name(name);
    customWidget->hide_add_btn();
    QListWidgetItem *item = new QListWidgetItem(ui->list_friend);
    // 设置 item 的大小，使其能容纳自定义 widget
    item->setSizeHint(QSize(ui->list_friend->width(),customWidget->height()));
    ui->list_friend->setItemWidget(item, customWidget);
}

void friend_list::on_cancel_clicked()
{
    emit switch_welcome();
}


void friend_list::on_search_btn_clicked()
{
    if (ui->search_edit->text() == UserMgr::get_instance().get_me_user().name 
        || ui->search_edit->text() == "") {
        return;
    }
    
    ui->search_list->clear();
    QJsonObject data;
    data["action"] = "search_friend";
    data["request"] = "search_friend";
    data["friend_name"] = ui->search_edit->text();
    
    http::get_instance().post(QUrl(),data,Model::friend_model);
}




void friend_list::on_show_friend_list_clicked()
{
    // 确保只请求或初始化一次好友列表。
    if (ui->list_friend->count() == 0) {
        init_friend_list();
    }
    ui->search_list->clear();
    ui->search_list->hide();
    ui->list_friend->show();
}

void friend_list::init_response_map()
{
    handle_response_map["request_add_friend"] = [this](QJsonObject& json) {

        QString name = json["from_friend_name"].toString();
        //std::cout << name.toStdString() << std::endl;
        // 是否同意添加好友对话框
        QMessageBox msgBox(this);
        msgBox.setWindowTitle("操作确认");
        QString the_tip = name + " 请求添加你为好友？";
        msgBox.setText(the_tip);
        QPushButton* btnAgree = msgBox.addButton("同意", QMessageBox::ActionRole);
        msgBox.addButton("取消", QMessageBox::RejectRole);
        // 显示模态对话框并获取用户选择
        msgBox.exec();

        // 根据点击的按钮执行逻辑（此处留出自定义空间）
        if (msgBox.clickedButton() == btnAgree) {
            QJsonObject data;
            data["action"] = "auth_friend";

            data["from_user"] = UserMgr::get_instance().get_me_user().name;
            data["to_user"] = name;

            data["status"] = "accepted";
            tcp::get_instance().send_json(data);

        }
        else {
            QJsonObject data;
            data["action"] = "auth_friend";

            data["from_user"] = UserMgr::get_instance().get_me_user().name;
            data["to_user"] = name;

            data["status"] = "rejected";
            tcp::get_instance().send_json(data);
        }




     };




    handle_response_map["request_auth_friend"] = [this](QJsonObject& json) {
        QString name = json["from_user"].toString();
        QString status = json["status"].toString();
        if (status == "accepted") {
            QMessageBox::information(this, "tip", QString(name + " 同意添加你为好友。"));
        }
        else {
            QMessageBox::information(this, "tip", QString(name + " 拒绝添加你为好友。"));
        }


    };

    
    handle_response_map["request_battle"] = [this](QJsonObject& json) {
        QString name = json["from_user"].toString();
        // 是否同意对战
        QMessageBox msgBox(this);
        msgBox.setWindowTitle("操作确认");
        QString the_tip = name + " 邀请你对战";
        msgBox.setText(the_tip);
        QPushButton* btnAgree = msgBox.addButton("同意", QMessageBox::ActionRole);
        msgBox.addButton("取消", QMessageBox::RejectRole);
        // 显示模态对话框并获取用户选择
        msgBox.exec();

        // 根据点击的按钮执行逻辑（此处留出自定义空间）
        if (msgBox.clickedButton() == btnAgree) {
            QJsonObject data;
            data["action"] = "auth_battle";
            data["status"] = "accepted";
            data["from_user"] = UserMgr::get_instance().get_me_user().name;
            data["to_user"] = name;
            tcp::get_instance().send_json(data);
            emit switch_web_board("black");

        }
        else {
            QJsonObject data;
            data["action"] = "auth_battle";
            data["status"] = "reject";
            data["from_user"] = UserMgr::get_instance().get_me_user().name;
            data["to_user"] = name;
            tcp::get_instance().send_json(data);
        }


    };
      
    handle_response_map["response_battle"] = [this](QJsonObject& json) {
        QString name = json["from_user"].toString();
        QString status = json["status"].toString();
        if (status == "accepted") {
            QMessageBox::information(this, "tip", QString(name + " 同意对战。"));
            emit switch_web_board("red");
        }
        else {
            QMessageBox::information(this, "tip", QString(name + " 拒绝对战 "));
        }

    };

        
}

