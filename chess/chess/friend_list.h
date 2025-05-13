#ifndef FRIEND_LIST_H
#define FRIEND_LIST_H

#include <QWidget>

namespace Ui {
class friend_list;
}

class friend_list : public QWidget
{
    Q_OBJECT

public:
    explicit friend_list(QWidget *parent = nullptr);
    ~friend_list();
    void add_item(QString);
    void add_search_item(QString);
    void init_friend_list();
private slots:
    void on_cancel_clicked();

    void on_search_btn_clicked();

    void on_show_friend_list_clicked();

private:
    Ui::friend_list *ui;
    void init_response_map();
    std::unordered_map<QString, std::function<void(QJsonObject&)>> handle_response_map;

signals: // qt信号强制公开 不设访问权限
    void switch_welcome();
    void switch_web_board(QString view);
};

#endif // FRIEND_LIST_H
