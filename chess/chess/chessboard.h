#ifndef CHESSBOARD_H
#define CHESSBOARD_H
#include <QObject>
#include <vector>
#include <QWidget>
#include <map>
#include <QPoint>
#include "global.h"
class ChessBase;

class chessboard :  public QObject
{
    Q_OBJECT
public:
    chessboard(QObject* parent = nullptr);
    bool hasPieceAt(const QPoint& pos) const; //某个位置是否有棋子 
    ChessBase* get_piece(int id);
    QString get_view();
    void update_chess(int id,QPoint start, QPoint end); // 更新棋盘数组
    

private:
    
    std::vector<std::vector<ChessBase*>> _chess_board; // 棋盘数组
    std::map<int, ChessBase*> piece_map; // 棋子map
    QString board_view; // 棋盘当前视角
    void show_board();//简单展示棋盘 
    Color current_player = Color::RED;

// 与前端交互
signals:
    void note_can_move_to(int,QPoint,bool);
    void eat_player();
    void emitGeneralAlert();// 绝杀信号
public slots:
    void can_move_to(int id, QPoint start, QPoint end);
    void init_chess_board(QString); //初始化

};

#endif // CHESSBOARD_H
