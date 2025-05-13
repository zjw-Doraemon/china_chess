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
    virtual ChessBase* get_piece(int id);
    virtual QString get_view(); // 获取当前视角 推断当前角色 黑棋还是红棋
    void update_chess(int id,QPoint start, QPoint end); // 更新棋盘数组
    void show_board();//简单展示棋盘 




protected:

    std::vector<std::vector<ChessBase*>> _chess_board; // 棋盘数组
    std::map<int, ChessBase*> piece_map; // 棋子map
    Color current_player = Color::RED; // 当前玩家 最开始是红走
    QString board_view; // 棋盘当前视角

// 与前端交互
signals:
    void note_can_move_to(int,QPoint,bool);
    void eat_player();
    void emitGeneralAlert();// 绝杀信号
 public slots:
    virtual void can_move_to(int id, QPoint start, QPoint end);
    void init_chess_board(QString); //初始化

};







class new_web_board : public  chessboard {
    Q_OBJECT
public:
    new_web_board(QObject* parent = nullptr);
    bool hasPieceAt(const QPoint& pos) const; //某个位置是否有棋子 
    void init_board(QString view);
    
    void slot_piece_click(uint8_t id);
    void slot_board_click(uint8_t x, uint8_t y);
    void sync_board(uint8_t id, uint8_t x, uint8_t y);
    ChessBase* get_piece(int id);
    QString get_view(); // 获取当前视角 推断当前角色 黑棋还是红棋 //需要提供给棋子使用 
    
signals:
    void paint_chess_pieces(uint8_t id, uint8_t x, uint8_t y);
    void piece_click(uint8_t id);
    void hide_piece(uint8_t id);
    void move_piece(uint8_t id, uint8_t x, uint8_t y);
    void slot_win();


private:
    bool checkMoveLegality(int& id, QPoint& end); // 检查所有移动合法性
    bool is_win(int& id, QPoint& end); // 判断是否胜利
    void update_status(int& id, QPoint& start, QPoint& end); // 处理移动后状态更新
    void update_chess(int id, QPoint start, QPoint end); // 更新棋盘数组
    void show_board();//简单展示棋盘
    void init_action_map();

private:
    QString board_view;
    int32_t choosed_id = -1;
    Color current_player = Color::RED; // 当前玩家 最开始是红走 控制回合
    std::vector<std::vector<ChessBase*>> _chess_board; // 棋盘数组
    std::map<int, ChessBase*> piece_map; // 棋子map
    std::map<std::string, std::function<void(QJsonObject json)>> action_map;
    
};









#endif // CHESSBOARD_H


