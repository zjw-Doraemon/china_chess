#ifndef CHESSBASE_H
#define CHESSBASE_H

#include <QObject>
#include "global.h"
#include <QPoint>

class chessboard;
class ChessBase : public QObject
{
    Q_OBJECT
public:
    // 获取棋子ID
    int getId() const;

    // 构造函数
    ChessBase(PieceType type, Color color, const QPoint& pos,int id, chessboard* board ,QObject* parent = nullptr);

    // 获取棋子类型
    QString getType() const;

    // 获取棋子颜色
    Color getColor() const;

    // 获取棋子位置
    QPoint getPosition() const;

    // 设置棋子位置
    void setPosition(const QPoint& newPos);

    // 获取选中状态
    SelectState getSelectionState() const;

    // 设置选中状态
    void setSelectionState(SelectState state);

    // 判断棋子是否可以移动到指定位置
    virtual bool canMoveTo(const QPoint& targetPos) const = 0;

    // 将棋子信息转换为字符串（用于调试或显示）
    virtual QString toString() const;

protected:
    QPoint pos;
    Color color;
    PieceType type;
    SelectState selectionState; // 选中状态
    int id; // 唯一标识
    chessboard* _board ;
    
};

#endif // CHESSBASE_H
