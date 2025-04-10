#include "mainwindow.h"
#include "register_dialog.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    
    , ui(new Ui::MainWindow)
    , _board(new board_control())
    , _start(new welcome())
    , _login(new logindialog())


{
    ui->setupUi(this);
    setCentralWidget(_login);
    setFixedSize(_login->size());
    connect(_login, &logindialog::switch_register, this,[this] {
        register_dialog* register_dlg = new register_dialog();
        register_dlg->exec();
        });
    // 前端 开始界面
    //setCentralWidget(_start);
    //this->setFixedSize(_start->size());
    // 切换棋盘窗口
    connect(_start.get(), &welcome::show_board, this, [=] {
        this->_board->show();
        this->hide();
    });
};




MainWindow::~MainWindow()
{
    delete ui;
}
