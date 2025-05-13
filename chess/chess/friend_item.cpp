#include "friend_item.h"
#include "ui_friend_item.h"
#include "http.h"
#include "tcp.h"
#include "user_data.h"

QString friend_item::get_friend_name()
{
    return ui->friend_name->text();
}

friend_item::friend_item(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::friend_item)
{
    setFixedHeight(50);
    ui->setupUi(this);
    // Ìí¼ÓºÃÓÑ
    connect(ui->add_friend,&QPushButton::clicked,this,&friend_item::add_friend);
    // ·¢ËÍÑûÇë
    connect(ui->battle,&QPushButton::clicked,this,&friend_item::invite_battle);
}

friend_item::~friend_item()
{
    delete ui;
}

void friend_item::set_friend_name(QString name)
{
    ui->friend_name->setText(name);
}

void friend_item::hide_add_btn()
{
    ui->add_friend->hide();
}

void friend_item::hide_battle_btn()
{
    ui->battle->hide();
}

void friend_item::invite_battle()
{
    QJsonObject data;
    data["action"] = "invite_battle";
    data["from_user"] = UserMgr::get_instance().get_me_user().name;
    data["to_user"] = ui->friend_name->text();
    tcp::get_instance().send_json(data);
}

void friend_item::add_friend()
{
    QJsonObject data;
    data["action"] = "add_friend";
    data["my_name"] = UserMgr::get_instance().get_me_user().name;
    data["add_friend_name"] = ui->friend_name->text();
    tcp::get_instance().send_json(data);
}
