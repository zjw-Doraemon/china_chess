#include "chessboard.h"
#include "global.h"
#include "chessfactory.h"
#include <QPainter>
#include <QLabel>
#include <iostream>
#include <windows.h>
#include "user_data.h"
#include "tcp.h"
#include <QJsonArray>
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
    //show_board();
   
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
    // 善后工作 
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































new_web_board::new_web_board(QObject* parent):chessboard(parent), _chess_board(10, std::vector<ChessBase*>(9, nullptr))
{
    connect(&tcp::get_instance(), &tcp::tcp_finish_web_board, this, [=](QJsonObject json) {
        std::string action = json["action"].toString().toStdString();
        if (action_map.find(action) == action_map.end()) {
            //std::cout << "board action no find" << std::endl;
            return;
        }
        action_map[action](json);
        });
    init_action_map();
}
void new_web_board::init_action_map()
{
    // 对方走子请求
    action_map["handle_oppo_move_info"] = [this](QJsonObject json) {
        int piece_id = json["piece_id"].toInt();
        QPoint end_point{ json["end_point"].toArray()[0].toInt(),json["end_point"].toArray()[1].toInt() };

        // 需要转换 y坐标
        QPoint start_pos = piece_map[piece_id]->getPosition();
        QPoint end_pos{ end_point.x(),(9 - end_point.y()) };
        
        // 调用不同逻辑
        if (hasPieceAt(end_pos)) {
            int id = _chess_board[end_pos.y()][end_pos.x()]->getId();
            // 取消自己的选中
            piece_click(choosed_id);
            choosed_id = -1;
            hide_piece(id);
            move_piece(piece_id,end_pos.x(),end_pos.y());
            update_status(piece_id,start_pos,end_pos);
            
        }
        else {
            move_piece(piece_id, end_pos.x(), end_pos.y());
            update_status(piece_id, start_pos, end_pos);
        }

        
        };
    action_map["notice_game_end"] = [this](QJsonObject json) {
        std::cout << "emit slot_win" << std::endl;
        emit slot_win();
        };
}
bool new_web_board::hasPieceAt(const QPoint& pos) const
{ 
    // 确保 pos 在棋盘范围内
    if (pos.x() < 0 || pos.x() >= 9 || pos.y() < 0 || pos.y() >= 10) {
        return false;
    }
    return _chess_board[pos.y()][pos.x()] != nullptr;
}

void new_web_board::init_board(QString view)
{ 
    // 删除之前剩余棋子
    for (int i = 0; i < _chess_board.size(); i++) {
        for (int j = 0; j < _chess_board[i].size(); j++) {
            if (_chess_board[i][j] == nullptr) {
                
                continue;
            };
            hide_piece(_chess_board[i][j]->getId());
        }
        std::cout << std::endl;
    }


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
            paint_chess_pieces(red_view[i].id,x,y);
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
            paint_chess_pieces(black_view[i].id, x, y);
        }
    }

}

void new_web_board::slot_piece_click(uint8_t id)
{
    
    
    // 第一次点击
    if (choosed_id == -1  ) {
        // 第一次无法点击对方棋子
        if (board_view != (piece_map[id]->getColor() == Color::BLACK ? "black" : "red")) {
            return;
        }
        choosed_id = id;
        piece_click(id);
        return;
    }
    
    // 第二次点击
    
    //如果是点击的是第一次点击的
    if (id == choosed_id) {
        choosed_id = -1;
        piece_click(id);
        return;
    }
    //如果是本阵容的棋子
    if (piece_map[id]->getColor() == piece_map[choosed_id]->getColor()) {
        piece_click(choosed_id);
        piece_click(id);
        choosed_id = id;
        return;
    }
    // 当前棋子颜色 位置
    QString color = piece_map[choosed_id]->getColor() == Color::RED ? "red" : "black";
    QPoint cur_piece_pos = piece_map[choosed_id]->getPosition();
    // 对方棋子
    ChessBase* second_piece = piece_map[id];
    QPoint pos = second_piece->getPosition();
    
    bool success = checkMoveLegality(choosed_id,pos);
    if (success) {
        hide_piece(id);
        move_piece(choosed_id,pos.x(), pos.y());
        update_status(choosed_id, cur_piece_pos, pos);
        piece_click(choosed_id);
        // 判断是否胜利
        if (is_win(choosed_id,pos)) {
            emit slot_win();
            QJsonObject data;
            data["action"] = "end_game";
            data["from_user"] = UserMgr::get_instance().get_me_user().name;
            tcp::get_instance().send_json(data);

        }
        sync_board(choosed_id,pos.x(),pos.y()); // 同步给对方
        choosed_id = -1;
        
    }
    

}


void new_web_board::slot_board_click(uint8_t x, uint8_t y)
{
    // 如果当前没有选中棋子 直接退出
    if (choosed_id == -1) {
        return;
    }
    
    
    QPoint click_pos{ x,y };
    QPoint cur_piece_pos = piece_map[choosed_id]->getPosition();
    bool success = checkMoveLegality(choosed_id,click_pos);
    if (success) {
        
        move_piece(choosed_id, x, y);
        update_status(choosed_id, cur_piece_pos, click_pos);
        piece_click(choosed_id);
        sync_board(choosed_id, x, y); // 同步给对方
        choosed_id = -1;
    }
}

void new_web_board::sync_board(uint8_t id, uint8_t x, uint8_t y)
{
    QJsonObject data;
    data["action"] = "move_piece";
    data["from_user"] = UserMgr::get_instance().get_me_user().name;
    data["piece_id"] = id;
    QJsonArray pos{x,y};
    data["end_point"] = pos;
    tcp::get_instance().send_json(data);
}

ChessBase* new_web_board::get_piece(int id)
{
    return piece_map[id];
}

QString new_web_board::get_view()
{
    return board_view;
}



bool new_web_board::checkMoveLegality(int& id, QPoint& end)
{
    // 当前棋子
    ChessBase* piece = piece_map[choosed_id];
    
    // 控制棋局回合 该红旗还是黑棋走了
    
    QString current_player_string = current_player == Color::RED ? "red" : "black";
    if (current_player_string != board_view) {
        //std::cout << (current_player == Color::BLACK ? "black" : "red") << std::endl;
        //std::cout << (cur_piece_color == Color::BLACK ? "black" : "red") << std::endl;
        return false;
    }
    // 判断棋子规则
    bool can_move = piece->canMoveTo(end);
    return can_move;
}

bool new_web_board::is_win(int& id, QPoint& end)
{
    // 将领
    auto black_king = get_piece(4);
    auto red_king = get_piece(20);
    if (end == red_king->getPosition() || end == black_king->getPosition()) {
        
        current_player = Color::RED;//每盘棋都要刷新一下 开局一定是红棋先走
        return true;
    }
    return false;
}

void new_web_board::update_status(int& id, QPoint& start, QPoint& end)
{
    // 当前棋子
    ChessBase* piece = piece_map[id];
    current_player = piece->getColor() == Color::RED ? Color::BLACK : Color::RED; //棋子成功走了之后才可以切换回合
    //更新棋子位置
    piece->setPosition(end);

    //改变逻辑棋盘
    update_chess(id, start, end);
    //打印逻辑棋盘
    //show_board();
}
void new_web_board::update_chess(int id, QPoint start, QPoint end)
{
    _chess_board[start.y()][start.x()] = nullptr; // 坐标与后端数组中的 索引 x与y是反转的 
    _chess_board[end.y()][end.x()] = piece_map[id];
}



void new_web_board::show_board()
{
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
    std::cout << std::endl << std::endl << std::endl;
    // 恢复原来的编码
    //SetConsoleOutputCP(oldCP);
}
