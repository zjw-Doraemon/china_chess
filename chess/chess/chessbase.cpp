#include "chessbase.h"
#include <QString>
#include <QDebug>
// 构造函数实现
ChessBase::ChessBase(PieceType type, Color color, const QPoint& pos, int id, chessboard* board, QObject* parent)
    : QObject(parent), pos(pos), color(color), type(type),  id(id),_board(board) {
}
int ChessBase::getId() const {
    return id;
}

// 获取棋子类型
QString ChessBase::getType() const {
    switch (type) {
    case PieceType::King:
        return "将";
    case PieceType::Advisor:
        return "士";
    case PieceType::Elephant:
        return "象";
    case PieceType::Horse:
        return "马";
    case PieceType::Chariot:
        return "车";
    case PieceType::Cannon:
        return "炮";
    case PieceType::Soldier:
        return "卒";
    default:
        return "未知";
    }
}


// 获取棋子颜色
Color ChessBase::getColor() const {
    return color;
}

// 获取棋子位置
QPoint ChessBase::getPosition() const {
    return pos;
}

// 设置棋子位置
void ChessBase::setPosition(const QPoint& newPos) {
    pos = newPos;
}


// 判断棋子是否可以移动到指定位置
bool ChessBase::canMoveTo(const QPoint& targetPos) const {
    // 纯虚函数，具体实现由派生类提供
    // 这里提供一个默认实现，返回 false
    return false;
}

