#ifndef BOARD_CONTROL_H
#define BOARD_CONTROL_H

#include "chessboard.h"
#include "chessboardui.h"
#include "new_chess_board_ui.h"
#include <QDialog>

namespace Ui {
class board_control;
}

class board_control : public QDialog
{
    Q_OBJECT

public:
    virtual void init_board(QString); // 初始化前后端棋盘
    board_control(QString empty);
    explicit board_control(QWidget *parent = nullptr);
    ~board_control();

protected:
    Ui::board_control *ui;
    //AutoResizeView* _board; //棋盘界面
    chessboard* logic_board; //后端界面;
    
    QString view = "red"; //上次棋盘的视角
signals:
    void quit_board();
    void switch_welcome();
};











#endif // BOARD_CONTROL_H
