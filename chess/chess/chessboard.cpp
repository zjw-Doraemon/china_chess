#include "chessboard.h"
#include "global.h"
#include "chessfactory.h"
#include <QPainter>
#include <QLabel>
#include <iostream>
#include <windows.h>


chessboard::chessboard(QObject* parent):QObject(parent),_chess_board(10,std::vector<ChessBase*>(9,nullptr))
{
    //init_chess_board("red");
    
}

bool chessboard::hasPieceAt(const QPoint& pos) const
{
    // 确保 pos 在棋盘范围内
    if (pos.x() < 0 || pos.x() >= 9 || pos.y() < 0 || pos.y() >= 10) {
        return false;
    }

    return _chess_board[pos.y()][pos.x()] != nullptr;
}

ChessBase* chessboard::get_piece(int id)
{
    return piece_map[id];
}

QString chessboard::get_view()
{
    return board_view;
}

void chessboard::update_chess(int id,QPoint start, QPoint end)
{
    _chess_board[start.y()][start.x()] = nullptr; // 坐标与后端数组中的 索引 x与y是反转的 
    _chess_board[end.y()][end.x()] = get_piece(id);
}


void chessboard::init_chess_board(QString view) {
    board_view = view;
    ChessFactory& factory = ChessFactory::instance();
    int id = 0;
    // 棋子初始化数据
    struct PieceInfo {
        int id;
        Color color;
        QString type;
        QPoint position;
    };

    QVector<PieceInfo> red_view = {
    {0, Color::BLACK, "Chariot", {0, 0}}, {1, Color::BLACK, "Horse", {1, 0}}, {2, Color::BLACK, "Elephant", {2, 0}},
    {3, Color::BLACK, "Advisor", {3, 0}}, {4, Color::BLACK, "King", {4, 0}}, {5, Color::BLACK, "Advisor", {5, 0}},
    {6, Color::BLACK, "Elephant", {6, 0}}, {7, Color::BLACK, "Horse", {7, 0}}, {8, Color::BLACK, "Chariot", {8, 0}},
    {9, Color::BLACK, "Cannon", {1, 2}}, {10, Color::BLACK, "Cannon", {7, 2}}, {11, Color::BLACK, "Soldier", {0, 3}},
    {12, Color::BLACK, "Soldier", {2, 3}}, {13, Color::BLACK, "Soldier", {4, 3}}, {14, Color::BLACK, "Soldier", {6, 3}},
    {15, Color::BLACK, "Soldier", {8, 3}}, {16, Color::RED, "Chariot", {0, 9}}, {17, Color::RED, "Horse", {1, 9}},
    {18, Color::RED, "Elephant", {2, 9}}, {19, Color::RED, "Advisor", {3, 9}}, {20, Color::RED, "King", {4, 9}},
    {21, Color::RED, "Advisor", {5, 9}}, {22, Color::RED, "Elephant", {6, 9}}, {23, Color::RED, "Horse", {7, 9}},
    {24, Color::RED, "Chariot", {8, 9}}, {25, Color::RED, "Cannon", {1, 7}}, {26, Color::RED, "Cannon", {7, 7}},
    {27, Color::RED, "Soldier", {0, 6}}, {28, Color::RED, "Soldier", {2, 6}}, {29, Color::RED, "Soldier", {4, 6}},
    {30, Color::RED, "Soldier", {6, 6}}, {31, Color::RED, "Soldier", {8, 6}}
    };

    QVector<PieceInfo> black_view = {
        {16, Color::RED, "Chariot", {0, 0}}, {17, Color::RED, "Horse", {1, 0}}, {18, Color::RED, "Elephant", {2, 0}},
        {19, Color::RED, "Advisor", {3, 0}}, {20, Color::RED, "King", {4, 0}}, {21, Color::RED, "Advisor", {5, 0}},
        {22, Color::RED, "Elephant", {6, 0}}, {23, Color::RED, "Horse", {7, 0}}, {24, Color::RED, "Chariot", {8, 0}},
        {25, Color::RED, "Cannon", {1, 2}}, {26, Color::RED, "Cannon", {7, 2}}, {27, Color::RED, "Soldier", {0, 3}},
        {28, Color::RED, "Soldier", {2, 3}}, {29, Color::RED, "Soldier", {4, 3}}, {30, Color::RED, "Soldier", {6, 3}},
        {31, Color::RED, "Soldier", {8, 3}}, {0, Color::BLACK, "Chariot", {0, 9}}, {1, Color::BLACK, "Horse", {1, 9}},
        {2, Color::BLACK, "Elephant", {2, 9}}, {3, Color::BLACK, "Advisor", {3, 9}}, {4, Color::BLACK, "King", {4, 9}},
        {5, Color::BLACK, "Advisor", {5, 9}}, {6, Color::BLACK, "Elephant", {6, 9}}, {7, Color::BLACK, "Horse", {7, 9}},
        {8, Color::BLACK, "Chariot", {8, 9}}, {9, Color::BLACK, "Cannon", {1, 7}}, {10, Color::BLACK, "Cannon", {7, 7}},
        {11, Color::BLACK, "Soldier", {0, 6}}, {12, Color::BLACK, "Soldier", {2, 6}}, {13, Color::BLACK, "Soldier", {4, 6}},
        {14, Color::BLACK, "Soldier", {6, 6}}, {15, Color::BLACK, "Soldier", {8, 6}}
    };
    if (view == "red") {
        for (int i = 0; i < red_view.size(); i++) {
            int x = red_view[i].position.x();
            int y = red_view[i].position.y();
            // 注意顺序 坐标 和 数组访问的顺序不一致
            ChessBase* tmp = factory.create(red_view[i].type, red_view[i].color, red_view[i].position, red_view[i].id, this, this);
            _chess_board[y][x] = tmp;
            piece_map[red_view[i].id] = tmp;
        }
    }
    else if (view == "black") {
        for (int i = 0; i < black_view.size(); i++) {
            int x = black_view[i].position.x();
            int y = black_view[i].position.y();
            // 注意顺序 坐标 和 数组访问的顺序不一致
            ChessBase* tmp = factory.create(black_view[i].type, black_view[i].color, black_view[i].position, black_view[i].id, this, this);
            _chess_board[y][x] = tmp;
            piece_map[black_view[i].id] = tmp;
        }
    }
    show_board();
   
}
void chessboard::can_move_to(int id, QPoint start, QPoint end) {
    // 当前棋子
    ChessBase* piece = get_piece(id);
    // 将领
    auto black_king = get_piece(4);
    auto red_king = get_piece(20);
    // 控制棋局回合
    Color cur_piece_color = piece->getColor();
    if (cur_piece_color != current_player) {
        note_can_move_to(id, end, false);
        return;
    }

    bool can_move = piece->canMoveTo(end);
    note_can_move_to(id,end,can_move);
    if (can_move == true) {
        current_player = cur_piece_color == Color::RED ? Color::BLACK : Color::RED; //棋子成功走了之后才可以切换回合
        
        // 判断将领是否被吃 判断胜负
        auto tmp = piece->getType();
        if (end == red_king->getPosition() || end == black_king->getPosition()) {
            emitGeneralAlert();
            current_player = Color::RED; //每盘棋都要刷新一下
        }
        piece->setPosition(end);
        // 改变后端棋盘数组
        update_chess(id, start, end);
        show_board();
        
    }
    
    
}

void chessboard::show_board() {
    // 更改控制台编码
    //UINT oldCP = GetConsoleOutputCP();
    SetConsoleOutputCP(65001);
    for (int i = 0; i < _chess_board.size(); i++) {
        for (int j = 0; j < _chess_board[i].size(); j++) {
            if (_chess_board[i][j] == nullptr) {
                std::cout << "   ";
                continue;
            };
            std::cout << _chess_board[i][j]->getType().toStdString() << " ";
        }
        std::cout << std::endl;
    }
    std::cout << std::endl << std::endl<<std::endl;
    // 恢复原来的编码
    //SetConsoleOutputCP(oldCP);
}
