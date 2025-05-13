#include "web_board_control.h"
#include "ui_web_board_control.h"
#include <iostream>
web_board_control::web_board_control(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::web_board_control)
{
    ui->setupUi(this);
    //连接绘制棋子信号
    connect(&board,&new_web_board::paint_chess_pieces,ui->board->get_scene(), &new_chess_board_ui::ChessBoardUi::paint_chess_pieces);
    //连接点击棋子信号
    connect(ui->board->get_scene(), &new_chess_board_ui::ChessBoardUi::piece_click, &board, &new_web_board::slot_piece_click);
    connect(&board, &new_web_board::piece_click, ui->board->get_scene(), &new_chess_board_ui::ChessBoardUi::click_piece);
    //连接棋盘点击信号
    connect(ui->board->get_scene(), &new_chess_board_ui::ChessBoardUi::board_click, &board, &new_web_board::slot_board_click);
    //隐藏棋子
    connect(&board, &new_web_board::hide_piece, ui->board->get_scene(), &new_chess_board_ui::ChessBoardUi::remove_piece);
    //移动棋子
    connect(&board, &new_web_board::move_piece, ui->board->get_scene(), &new_chess_board_ui::ChessBoardUi::move_piece);
    //胜利
    connect(&board, &new_web_board::slot_win, this, [=] {
        QObject* parentObj = this->parent();  // 获取父对象（QObject* 类型）
        QDialog* Parent = static_cast<QDialog*>(parentObj);
        Parent->close();
        });
    

}

web_board_control::~web_board_control()
{
    std::cout << "web_board delete" << std::endl;
    delete ui;
}

void web_board_control::init_board(QString view)
{
    board.init_board(view);
}
