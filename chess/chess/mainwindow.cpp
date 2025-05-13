#include "mainwindow.h"
#include "register_dialog.h"

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent)

    , ui(new Ui::MainWindow)
    , _board(new board_control())
    , _start(new welcome())
    , _login(new logindialog())
    , _register(new register_dialog())
    , _friend_list(new friend_list())
    , _web_board(new QDialog())
    


{
    
    ui->setupUi(this);
    setCentralWidget(_login.get());
    setFixedSize(_login->size());
    // 注册 登录 两界面互相切换
    connect(_login.get(), &logindialog::switch_register, this, [this] {
        takeCentralWidget();
        setCentralWidget(_register.get());
        
        });
    connect(_register.get(), &register_dialog::switch_log_dialog, this, [this] {
        takeCentralWidget();
        setCentralWidget(_login.get());
        });
   
    // 登录界面切换开始界面
    connect(_login.get(), &logindialog::switch_welcome, this, [this] {
        takeCentralWidget();
        setCentralWidget(_start.get());
        //this->setFixedSize(_start->size());

        });
    // 开始界面切换棋盘窗口
    connect(_start.get(), &welcome::show_board, this, [=] {
        this->_board->show();
        this->hide();
    });
    // 开始界面与好友界面互换

    connect(_start.get(), &welcome::switch_friend_list, this, [=] {
        takeCentralWidget();
        setCentralWidget(_friend_list.get());
        
    });
    connect(_friend_list.get(),&friend_list::switch_welcome,this,[=]{
        
        takeCentralWidget();
        setCentralWidget(_start.get());
    });

   
    //切换网络棋盘界面对战 初始化棋盘 关闭棋盘 切换会主界面
    _web_board.get()->setWindowTitle("网络棋盘");
   
    connect(_friend_list.get(), &friend_list::switch_web_board, this, &MainWindow::switch_web_board);
    connect(_web_board.get(), &QDialog::finished, this, [=] {
        this->show();
        });
        
   

    
    //// 网络棋盘切换到好友界面
    //connect(web_board.get(), &web_board_control::switch_friend_widget, this, [this]  {

    //    takeCentralWidget();
    //    setCentralWidget(_friend_list.get());
    //    this->setFixedSize(_friend_list.get()->size());
    //    
    //});

    
    
    
    


};




MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::switch_web_board(QString view)
{
    // 获取父对象的所有直接子对象（保存到临时列表，避免迭代过程中列表变化）
    QList<QObject*> children = _web_board.get()->children();

    // 遍历并删除每个子对象（递归删除其所有子对象）
    for (QObject* child : children) {
        delete child;  // 关键：删除子对象，触发递归删除其所有子对象
    }
   
    // 创建棋盘并绑定父对象

    web_board_control* web_board = new web_board_control(_web_board.get());
    web_board->init_board(view);  // 初始化棋盘

    // 创建布局并将 web_board 添加到对话框中
    QVBoxLayout* layout = new QVBoxLayout(_web_board.get());
    layout->addWidget(web_board);  // 将 web_board 作为子组件添加到对话框
    _web_board.get()->setLayout(layout);
    
    this->hide();
    _web_board.get()->show();
    
    
    

    

}
