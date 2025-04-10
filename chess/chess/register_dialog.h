#ifndef REGISTER_DIALOG_H
#define REGISTER_DIALOG_H
#include <QDialog>
namespace Ui {
class register_dialog;
}

class register_dialog : public QDialog
{
    Q_OBJECT
public:
    explicit register_dialog(QWidget *parent = nullptr);
    ~register_dialog();
    void get_varify_code();

private:
    Ui::register_dialog* ui;
};

#endif // REGISTER_DIALOG_H
