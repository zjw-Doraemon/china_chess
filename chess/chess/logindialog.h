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
    void login();
    Ui::LoginDialog* ui;
signals:
    void switch_register();
    void switch_welcome();

private slots:
    
};

#endif // LOGINDIALOG_H
