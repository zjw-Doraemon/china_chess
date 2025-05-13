#ifndef FRIEND_ITEM_H
#define FRIEND_ITEM_H

#include <QWidget>

namespace Ui {
class friend_item;
}

class friend_item : public QWidget
{
    Q_OBJECT

public:
    QString get_friend_name();
    explicit friend_item(QWidget *parent = nullptr);
    ~friend_item();
    void set_friend_name(QString);
    void hide_add_btn();
    void hide_battle_btn();

private:
    Ui::friend_item *ui;




public slots:
    void add_friend();
    void invite_battle();
};

#endif // FRIEND_ITEM_H
