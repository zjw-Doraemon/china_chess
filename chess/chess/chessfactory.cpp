#include "chessfactory.h"
#include "chessboard.h"
#include "global.h"
#include <iostream>
bool King::canMoveTo(const QPoint& target) const {


    int dx = abs(target.x() - pos.x());
    int dy = abs(target.y() - pos.y());
    
    // �����ƶ�һ��
    bool isOneStepMove = (dx + dy == 1);
    bool inOwnHalf;
    // ������Ӫ
    if (_board->get_view() == "black") {
        inOwnHalf = (color == Color::RED) ? (target.y() <= 2) : (target.y() >= 7) ;
    }
    else {
        inOwnHalf = (color == Color::RED) ? (target.y() >= 7) : (target.y() <= 2) ;
    }
    inOwnHalf = inOwnHalf && (target.x() >= 3 && target.x() <= 5);
    // �Ͻ�����  
    // ���������ҪΪ������������ ������Ͻ������ϲ������ƶ����߼�����Ϊ�鷳
    //bool king_meet = false; 
    //int op_king_id = this->getColor() == Color::BLACK ? 20 : 4; // �����Ͻ�
    //ChessBase* king_op = _board->get_piece(op_king_id);
    //
    //if (target.x() == king_op->getPosition().x()) {
    //    int direct = king_op->getPosition().y() > target.y() ? 1 : -1; // �з��Ͻ�λ������� �ҷ��ķ���
    //    int tmp = target.y() + 1; // ����
    //    bool has_piece;
    //    while (tmp != king_op->getPosition().y()) {
    //        if (_board->hasPieceAt(QPoint(target.x(), tmp))) {
    //            king_meet = true;
    //        }
    //        tmp = tmp + direct;
    //    }
    //    return king_meet;
    //}

    // �Ͻ�����
    bool king_eat= true; // �Ͻ��Ƿ���ԶԳ�
    int op_king_id = this->getColor() == Color::BLACK ? 20 : 4; // �����Ͻ�
    ChessBase* king_op = _board->get_piece(op_king_id);
    if (target == king_op->getPosition()) {
        int direct = king_op->getPosition().y() > target.y() ? 1 : -1; // �з��Ͻ�λ������� �ҷ��ķ���
        int tmp = target.y() + 1; // ����
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
    // �Ƿ��ڱ���������
    bool inOwnHalf;
    // ������Ӫ
    if (_board->get_view() == "black") {
        inOwnHalf = (color == Color::RED) ? (target.y() <= 2) : (target.y() >=7);
    }
    else {
        inOwnHalf = (color == Color::RED) ? (target.y() >= 7) : (target.y() <= 2);
    }
    inOwnHalf = inOwnHalf && (target.x() >= 3 && target.x() <= 5);
    // �ߵķ���
    bool isDiagonalMove = (dx == 1 && dy == 1);
    

    return isDiagonalMove  && inOwnHalf;
}

bool Elephant::canMoveTo(const QPoint& target) const {
    int dx = abs(target.x() - pos.x());
    int dy = abs(target.y() - pos.y());

    bool isElephantMove = (dx == 2 && dy == 2); // �Ƿ����Ӹ����� 

    // �Ƿ��ڱ���������
    bool inOwnHalf;
    // ������Ӫ
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
    // ��û�б����ȵ�����
    QPoint blockingPoint = (dx == 2) 
        ? QPoint((pos.x() + target.x()) / 2, pos.y())
        : QPoint(pos.x(), (pos.y() + target.y()) / 2);
    bool horseLegBlocked = _board->hasPieceAt(blockingPoint);

    return isLMove && !horseLegBlocked;
}
bool Chariot::canMoveTo(const QPoint& target) const {
    // 1. ȷ����ֱ���ƶ�
    bool isStraightMove = (target.x() == pos.x()) ^ (target.y() == pos.y());
    if (!isStraightMove) return false;

    // 2. ����·���Ƿ�ͨ��
    int stepX = (target.x() > pos.x()) - (target.x() < pos.x()); // 1, 0, -1
    int stepY = (target.y() > pos.y()) - (target.y() < pos.y()); // 1, 0, -1

    // ·�����Ƿ������� 
    bool has_piece = false;
    QPoint checkPos = pos + QPoint(stepX, stepY);
    while (checkPos != target) {
        if (_board->hasPieceAt(checkPos)) {
            has_piece = true; // ֻҪ·���������ӣ��Ͳ�����
        }
        checkPos += QPoint(stepX, stepY);
    }
    // 3. Ŀ��λ���Ƿ������ӣ��ж��Ƿ���ӣ�
    bool hasTargetPiece = _board->hasPieceAt(target);

    return isStraightMove&&!has_piece; // ֱ�������赲�������ƶ�
}


bool Cannon::canMoveTo(const QPoint& targetPos) const {
    // 1. �Ƿ���ֱ���ƶ�����ֻ��ֱ�ߣ�
    bool isStraightMove = (targetPos.x() == pos.x() || targetPos.y() == pos.y());
    if (!isStraightMove) {
        return false;
    }
    // 2. ����·���ϵ���������
    int stepX = (targetPos.x() > pos.x()) ? 1 : (targetPos.x() < pos.x() ? -1 : 0);  //ȷ������
    int stepY = (targetPos.y() > pos.y()) ? 1 : (targetPos.y() < pos.y() ? -1 : 0);

    QPoint checkPos = pos + QPoint(stepX, stepY);
    int obstacleCount = 0;
    while (checkPos != targetPos) {
        obstacleCount += _board->hasPieceAt(checkPos);
        checkPos += QPoint(stepX, stepY);
    }

    // 3. Ŀ��λ���Ƿ������ӣ��ж��Ƿ���ӣ�
    bool hasTargetPiece = _board->hasPieceAt(targetPos);

    // 4. �����ж�
    bool canMoveFreely = (obstacleCount == 0) && !hasTargetPiece; // ֱ�����赲
    bool canCapture = (obstacleCount == 1) && hasTargetPiece;      // ��һ������

    return isStraightMove && (canMoveFreely || canCapture);
}

bool Soldier::canMoveTo(const QPoint& target) const {
    // ��ȡ��ǰλ��
    QPoint pos = this->pos;

    // �����ƶ��Ĳ���
    int dx = target.x() - pos.x();
    int dy = target.y() - pos.y();
    int forward;// ����ǰ������
    bool isBeforeRiver = false;// �ж��Ƿ�δ����

    // ������Ӫ
    if (_board->get_view() == "black") {
        forward = (this->color == Color::RED) ? 1 : -1;
        isBeforeRiver = (this->color == Color::RED) ? (pos.y() <= 4) : (pos.y() >= 5);
    }
    else {
        forward = (this->color == Color::RED) ? -1 : 1;
        isBeforeRiver = (this->color == Color::RED) ? (pos.y() >= 5) : (pos.y() <= 4);
    }
    
   

    

    // �����Ƿ��ǺϷ���ֱ��
    bool canMoveForward = (dx == 0 && dy == forward);

    // �����Ƿ��ǺϷ��������ƶ��������Ӻ�
    bool canMoveSideways = (dy == 0 && abs(dx) == 1);

    // δ����ʱֻ��ǰ��
    if (isBeforeRiver) {
        return canMoveForward;
    }
    
    // ���Ӻ����ǰ���������ƶ�
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

