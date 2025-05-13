#ifndef CHESSFACTORY_H
#define CHESSFACTORY_H
#include "chessbase.h"


class ChessFactory {
public:
    using CreateFunc = std::function<ChessBase* (Color, const QPoint&, int, chessboard*, QObject*)>;
    
    // **注册棋子**
    void registerPiece(const QString& type, CreateFunc func) {
        factoryMap[type] = func;
    }

    // **创建棋子**
    ChessBase* create(const QString& type, Color color, const QPoint& pos, int id, chessboard* board, QObject* parent = nullptr) {
        auto it = factoryMap.find(type);
        if (it != factoryMap.end()) {
            return it->second(color, pos, id, board, parent);
        }
        return nullptr; // 未找到对应棋子
    }
    
    // **单例模式，确保全局只有一个工厂**
    static ChessFactory& instance() {
        static ChessFactory factory;
        return factory;
    }

private:
    ChessFactory();
    std::unordered_map<QString, CreateFunc> factoryMap;
};















class King : public ChessBase
{
    Q_OBJECT
public:
    King(Color color, const QPoint& pos, int id, chessboard* board, QObject* parent = nullptr)
        : ChessBase(PieceType::King, color, pos, id, board,parent) {
    }

    bool canMoveTo(const QPoint& targetPos) const override;
};




class Advisor : public ChessBase
{
    Q_OBJECT
public:
    Advisor(Color color, const QPoint& pos, int id, chessboard* board, QObject* parent = nullptr)
        : ChessBase(PieceType::Advisor, color, pos, id, board, parent) {
    }

    bool canMoveTo(const QPoint& targetPos) const override;
};

class Elephant : public ChessBase
{
    Q_OBJECT
public:
    Elephant(Color color, const QPoint& pos, int id, chessboard* board,QObject* parent = nullptr)
        : ChessBase(PieceType::Elephant, color, pos, id, board, parent) {
    }

    bool canMoveTo(const QPoint& targetPos) const override;
};

class Horse : public ChessBase
{
    Q_OBJECT
public:
    Horse(Color color, const QPoint& pos, int id, chessboard* board, QObject* parent = nullptr)
        : ChessBase(PieceType::Horse, color, pos, id, board, parent) {
    }

    bool canMoveTo(const QPoint& targetPos) const override;
};

class Chariot : public ChessBase
{
    Q_OBJECT
public:
    Chariot(Color color, const QPoint& pos, int id, chessboard* board, QObject* parent = nullptr)
        : ChessBase(PieceType::Chariot, color, pos, id, board, parent) {
    }

    bool canMoveTo(const QPoint& targetPos) const override;
};

class Cannon : public ChessBase
{
    Q_OBJECT
public:
    Cannon(Color color, const QPoint& pos, int id, chessboard* board, QObject* parent = nullptr)
        : ChessBase(PieceType::Cannon, color, pos, id, board, parent) {
    }

    bool canMoveTo(const QPoint& targetPos) const override;
};

class Soldier : public ChessBase
{
    Q_OBJECT
public:
    Soldier(Color color, const QPoint& pos, int id, chessboard* board, QObject* parent = nullptr)
        : ChessBase(PieceType::Soldier, color, pos, id, board, parent) {
    }

    bool canMoveTo(const QPoint& targetPos) const override;
};





#endif // CHESSFACTORY_H
