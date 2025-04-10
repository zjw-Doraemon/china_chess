#ifndef LOGINDIALOG_H
#define LOGINDIALOG_H

#include <QWidget>
#include <QDialog>
namespace Ui{
class LoginDialog;
};

class logindialog : public QDialog
{
    Q_OBJECT
public:
    explicit logindialog(QWidget *parent = nullptr);
    ~logindialog();
private:
    Ui::LoginDialog* ui;
signals:
    void switch_register();
};

#endif // LOGINDIALOG_H
