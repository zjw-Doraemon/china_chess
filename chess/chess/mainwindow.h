#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "./ui_mainwindow.h"
#include "chessboardui.h"
#include <QGraphicsView>
#include <QVBoxLayout>
#include "chessboard.h"
#include "welcome.h"
#include "board_control.h"
#include "logindialog.h"
#include "register_dialog.h"
#include "memory"
#include "friend_list.h"
#include "new_chess_board_ui.h"
#include "web_board_control.h"
QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    void switch_web_board(QString view);
private:
    Ui::MainWindow *ui;
    std::unique_ptr<welcome>  _start; //开始界面
    std::unique_ptr<board_control> _board; // 棋盘界面
    std::unique_ptr < logindialog>_login; // 登录界面
    std::unique_ptr < register_dialog> _register; // 注册界面
    std::unique_ptr <friend_list> _friend_list; // 好友界面
    std::unique_ptr <QDialog> _web_board;
    

    

};
#endif // MAINWINDOW_H
