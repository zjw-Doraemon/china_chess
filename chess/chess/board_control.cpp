#include "board_control.h"
#include "ui_board_control.h"
#include <iostream>
void board_control::init_board(QString view)
{
    logic_board->init_chess_board(view);
    ui->board->init_board(view);
}

board_control::board_control(QString empty)
    :ui(nullptr)
{
}

board_control::board_control(QWidget *parent)
    : QDialog(parent)
    , logic_board(new chessboard(this))
    , ui(new Ui::board_control)
    
{
    ui->setupUi(this);
    // 连接信号

    // 初始化棋盘
    init_board("red");
    // 移动 吃子 本地后端
    
    connect(ui->board->get_scene(), &ChessBoardUi::PieceMove, logic_board, &chessboard::can_move_to); // ui类发给逻辑类  走子逻辑
    connect(logic_board, &chessboard::note_can_move_to, ui->board->get_scene(), &ChessBoardUi::can_move); //逻辑类返还给ui类
    connect(logic_board, &chessboard::emitGeneralAlert, ui->board->get_scene(), &ChessBoardUi::judge_win); // 判断胜负

    

    // 切换棋盘
    connect(logic_board, &chessboard::emitGeneralAlert, this, [this] {
        ui->board->clear_pieces();
        this->view = view == "red" ? "black" : "red";
        init_board(view);
        });
}

board_control::~board_control()
{
    delete ui;
}







