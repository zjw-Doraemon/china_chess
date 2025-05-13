#include "web_board_control.h"
#include "ui_web_board_control.h"
#include <iostream>
web_board_control::web_board_control(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::web_board_control)
{
    ui->setupUi(this);
    //���ӻ��������ź�
    connect(&board,&new_web_board::paint_chess_pieces,ui->board->get_scene(), &new_chess_board_ui::ChessBoardUi::paint_chess_pieces);
    //���ӵ�������ź�
    connect(ui->board->get_scene(), &new_chess_board_ui::ChessBoardUi::piece_click, &board, &new_web_board::slot_piece_click);
    connect(&board, &new_web_board::piece_click, ui->board->get_scene(), &new_chess_board_ui::ChessBoardUi::click_piece);
    //�������̵���ź�
    connect(ui->board->get_scene(), &new_chess_board_ui::ChessBoardUi::board_click, &board, &new_web_board::slot_board_click);
    //��������
    connect(&board, &new_web_board::hide_piece, ui->board->get_scene(), &new_chess_board_ui::ChessBoardUi::remove_piece);
    //�ƶ�����
    connect(&board, &new_web_board::move_piece, ui->board->get_scene(), &new_chess_board_ui::ChessBoardUi::move_piece);
    //ʤ��
    connect(&board, &new_web_board::slot_win, this, [=] {
        QObject* parentObj = this->parent();  // ��ȡ������QObject* ���ͣ�
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
