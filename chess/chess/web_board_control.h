#ifndef WEB_BOARD_CONTROL_H
#define WEB_BOARD_CONTROL_H

#include <QWidget>
#include "chessboard.h"
namespace Ui {
class web_board_control;
}

class web_board_control : public QWidget
{
    Q_OBJECT

public:
    explicit web_board_control(QWidget *parent = nullptr);
    ~web_board_control();
    void init_board(QString view);
signals:
   
private:
    Ui::web_board_control *ui;
    new_web_board board;

};

#endif // WEB_BOARD_CONTROL_H
