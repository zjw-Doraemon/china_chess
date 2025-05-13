#include "chessfactory.h"
#include "chessboard.h"
#include "global.h"
#include <iostream>
bool King::canMoveTo(const QPoint& target) const {


    int dx = abs(target.x() - pos.x());
    int dy = abs(target.y() - pos.y());
    
    // 仅能移动一步
    bool isOneStepMove = (dx + dy == 1);
    bool inOwnHalf;
    // 考虑阵营
    if (_board->get_view() == "black") {
        inOwnHalf = (color == Color::RED) ? (target.y() <= 2) : (target.y() >= 7) ;
    }
    else {
        inOwnHalf = (color == Color::RED) ? (target.y() >= 7) : (target.y() <= 2) ;
    }
    inOwnHalf = inOwnHalf && (target.x() >= 3 && target.x() <= 5);
    // 老将对面  
    // 这个方案需要为其他棋子增加 如果在老将的线上不可以移动的逻辑，较为麻烦
    //bool king_meet = false; 
    //int op_king_id = this->getColor() == Color::BLACK ? 20 : 4; // 对面老将
    //ChessBase* king_op = _board->get_piece(op_king_id);
    //
    //if (target.x() == king_op->getPosition().x()) {
    //    int direct = king_op->getPosition().y() > target.y() ? 1 : -1; // 敌方老将位置相对于 我方的方向
    //    int tmp = target.y() + 1; // 方向
    //    bool has_piece;
    //    while (tmp != king_op->getPosition().y()) {
    //        if (_board->hasPieceAt(QPoint(target.x(), tmp))) {
    //            king_meet = true;
    //        }
    //        tmp = tmp + direct;
    //    }
    //    return king_meet;
    //}

    // 老将碰面
    bool king_eat= true; // 老将是否可以对吃
    int op_king_id = this->getColor() == Color::BLACK ? 20 : 4; // 对面老将
    ChessBase* king_op = _board->get_piece(op_king_id);
    if (target == king_op->getPosition()) {
        int direct = king_op->getPosition().y() > target.y() ? 1 : -1; // 敌方老将位置相对于 我方的方向
        int tmp = target.y() + 1; // 方向
        bool has_piece;
        while (tmp != king_op->getPosition().y()) {
            if (_board->hasPieceAt(QPoint(target.x(), tmp))) {
                king_eat = false;
            }
            tmp = tmp + direct;
        }
        return king_eat;
    }
    

    return isOneStepMove && inOwnHalf;
}

bool Advisor::canMoveTo(const QPoint& target) const {
    int dx = abs(target.x() - pos.x());
    int dy = abs(target.y() - pos.y());
    // 是否在本方棋盘内
    bool inOwnHalf;
    // 考虑阵营
    if (_board->get_view() == "black") {
        inOwnHalf = (color == Color::RED) ? (target.y() <= 2) : (target.y() >=7);
    }
    else {
        inOwnHalf = (color == Color::RED) ? (target.y() >= 7) : (target.y() <= 2);
    }
    inOwnHalf = inOwnHalf && (target.x() >= 3 && target.x() <= 5);
    // 走的方向
    bool isDiagonalMove = (dx == 1 && dy == 1);
    

    return isDiagonalMove  && inOwnHalf;
}

bool Elephant::canMoveTo(const QPoint& target) const {
    int dx = abs(target.x() - pos.x());
    int dy = abs(target.y() - pos.y());

    bool isElephantMove = (dx == 2 && dy == 2); // 是否田子格行走 

    // 是否在本方棋盘内
    bool inOwnHalf;
    // 考虑阵营
    if (_board->get_view() == "black") {
        inOwnHalf = (color == Color::RED) ? (target.y() <= 4) : (target.y() >= 5);
    }
    else {
        inOwnHalf = (color == Color::RED) ? (target.y() >= 5) : (target.y() <= 4);
    }
      

    QPoint middle((pos.x() + target.x()) / 2, (pos.y() + target.y()) / 2);
    bool elephantEyeBlocked = _board->hasPieceAt(middle);

    return isElephantMove && inOwnHalf && !elephantEyeBlocked;
}


bool Horse::canMoveTo(const QPoint& target) const {
    int dx = abs(target.x() - pos.x());
    int dy = abs(target.y() - pos.y());

    bool isLMove = (dx == 2 && dy == 1) || (dx == 1 && dy == 2);
    // 有没有别马腿的棋子
    QPoint blockingPoint = (dx == 2) 
        ? QPoint((pos.x() + target.x()) / 2, pos.y())
        : QPoint(pos.x(), (pos.y() + target.y()) / 2);
    bool horseLegBlocked = _board->hasPieceAt(blockingPoint);

    return isLMove && !horseLegBlocked;
}
bool Chariot::canMoveTo(const QPoint& target) const {
    // 1. 确保是直线移动
    bool isStraightMove = (target.x() == pos.x()) ^ (target.y() == pos.y());
    if (!isStraightMove) return false;

    // 2. 计算路径是否通畅
    int stepX = (target.x() > pos.x()) - (target.x() < pos.x()); // 1, 0, -1
    int stepY = (target.y() > pos.y()) - (target.y() < pos.y()); // 1, 0, -1

    // 路径上是否有棋子 
    bool has_piece = false;
    QPoint checkPos = pos + QPoint(stepX, stepY);
    while (checkPos != target) {
        if (_board->hasPieceAt(checkPos)) {
            has_piece = true; // 只要路径上有棋子，就不能走
        }
        checkPos += QPoint(stepX, stepY);
    }
    // 3. 目标位置是否有棋子（判断是否吃子）
    bool hasTargetPiece = _board->hasPieceAt(target);

    return isStraightMove&&!has_piece; // 直线且无阻挡，允许移动
}


bool Cannon::canMoveTo(const QPoint& targetPos) const {
    // 1. 是否是直线移动（炮只能直走）
    bool isStraightMove = (targetPos.x() == pos.x() || targetPos.y() == pos.y());
    if (!isStraightMove) {
        return false;
    }
    // 2. 计算路径上的棋子数量
    int stepX = (targetPos.x() > pos.x()) ? 1 : (targetPos.x() < pos.x() ? -1 : 0);  //确定方向
    int stepY = (targetPos.y() > pos.y()) ? 1 : (targetPos.y() < pos.y() ? -1 : 0);

    QPoint checkPos = pos + QPoint(stepX, stepY);
    int obstacleCount = 0;
    while (checkPos != targetPos) {
        obstacleCount += _board->hasPieceAt(checkPos);
        checkPos += QPoint(stepX, stepY);
    }

    // 3. 目标位置是否有棋子（判断是否吃子）
    bool hasTargetPiece = _board->hasPieceAt(targetPos);

    // 4. 规则判断
    bool canMoveFreely = (obstacleCount == 0) && !hasTargetPiece; // 直走无阻挡
    bool canCapture = (obstacleCount == 1) && hasTargetPiece;      // 隔一个吃子

    return isStraightMove && (canMoveFreely || canCapture);
}

bool Soldier::canMoveTo(const QPoint& target) const {
    // 获取当前位置
    QPoint pos = this->pos;

    // 计算移动的步长
    int dx = target.x() - pos.x();
    int dy = target.y() - pos.y();
    int forward;// 兵的前进方向
    bool isBeforeRiver = false;// 判断是否未过河

    // 考虑阵营
    if (_board->get_view() == "black") {
        forward = (this->color == Color::RED) ? 1 : -1;
        isBeforeRiver = (this->color == Color::RED) ? (pos.y() <= 4) : (pos.y() >= 5);
    }
    else {
        forward = (this->color == Color::RED) ? -1 : 1;
        isBeforeRiver = (this->color == Color::RED) ? (pos.y() >= 5) : (pos.y() <= 4);
    }
    
   

    

    // 计算是否是合法的直行
    bool canMoveForward = (dx == 0 && dy == forward);

    // 计算是否是合法的左右移动（仅过河后）
    bool canMoveSideways = (dy == 0 && abs(dx) == 1);

    // 未过河时只能前进
    if (isBeforeRiver) {
        return canMoveForward;
    }
    
    // 过河后可以前进或左右移动
    return canMoveForward || canMoveSideways;
}








ChessFactory::ChessFactory() {
    registerPiece("King", [](Color color, const QPoint& pos, int id, chessboard* board, QObject* parent) {
        return new King(color, pos, id, board, parent);
        });

    registerPiece("Advisor", [](Color color, const QPoint& pos, int id, chessboard* board, QObject* parent) {
        return new Advisor(color, pos, id, board, parent);
        });

    registerPiece("Elephant", [](Color color, const QPoint& pos, int id, chessboard* board, QObject* parent) {
        return new Elephant(color, pos, id, board, parent);
        });

    registerPiece("Horse", [](Color color, const QPoint& pos, int id, chessboard* board, QObject* parent) {
        return new Horse(color, pos, id, board, parent);
        });

    registerPiece("Chariot", [](Color color, const QPoint& pos, int id, chessboard* board, QObject* parent) {
        return new Chariot(color, pos, id, board, parent);
        });

    registerPiece("Cannon", [](Color color, const QPoint& pos, int id, chessboard* board, QObject* parent) {
        return new Cannon(color, pos, id, board, parent);
        });

    registerPiece("Soldier", [](Color color, const QPoint& pos, int id, chessboard* board, QObject* parent) {
        return new Soldier(color, pos, id, board, parent);
        });
}

