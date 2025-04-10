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
#include "memory"
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
    Ui::MainWindow *ui;
    std::unique_ptr<welcome>  _start; //开始界面
    std::unique_ptr<board_control> _board;
    logindialog* _login;

    

};
#endif // MAINWINDOW_H
