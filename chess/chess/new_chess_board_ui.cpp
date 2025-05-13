#include "new_chess_board_ui.h"
#include <QGraphicsSceneMouseEvent>
#include <iostream>
#include <QCoreApplication>
#include <QGraphicsDropShadowEffect>
#include <QPropertyAnimation>
#include <QTimer>

namespace new_chess_board_ui {
    // scene 管理类
    ChessBoardUi::ChessBoardUi(QObject* parent) : QGraphicsScene(parent), _chessboard(new ChessBoardItem())
    {
        setSceneRect(_chessboard->boundingRect()); // 设置 scene 的范围
        init_id_to_piece_info();
        // 添加棋盘 居中棋盘
        addItem(_chessboard);
        centerItemInScene(_chessboard, this);
        // 棋盘点击信号
        connect(_chessboard, &ChessBoardItem::position_board, this, [this](uint8_t x,uint8_t y) {
            //std::cout << "board clickd" << std::endl;
            emit board_click(x,y);
            });
          
        
    }
    //public
    void ChessBoardUi::move_piece(uint8_t id, uint8_t x, uint8_t y)
    {
        if (id_to_pieces.find(id) == id_to_pieces.end()) {
            std::cout << "移动的棋子不存在" << std::endl;
            return;
        }
        id_to_pieces[id]->animation_move(x,y);
    }

    void ChessBoardUi::remove_piece(uint8_t id)
    {
        if (id_to_pieces.find(id) == id_to_pieces.end()) {
            std::cout << "移除的棋子不存在" << std::endl;
            return;
        }
        id_to_pieces[id]->deleteLater();
    }

    void ChessBoardUi::click_piece(uint8_t id)
    {
        if (id_to_pieces.find(id) == id_to_pieces.end()) {
            std::cout << "点击的棋子不存在" << std::endl;
            return;
        }
        id_to_pieces[id]->animation_select();
    }

    

    void ChessBoardUi::paint_chess_pieces(uint8_t id, uint8_t x, uint8_t y) {
        QString pieceColor = id_to_piece_info[id].color;
        QString pieceType = id_to_piece_info[id].type;
        Chesspieces* piece = new Chesspieces(id,pieceType, pieceColor);
        _chessboard->add_pieces(piece, QPoint(x, y));
        // 为每个棋子添加信号
        connect(piece, &Chesspieces::piece_click, this, &ChessBoardUi::piece_click);

        // 添加到map
        id_to_pieces[id] = piece;
    }



    void ChessBoardUi::centerItemInScene(QGraphicsItem* item, QGraphicsScene* scene) {
        if (!item || !scene) return;

        QRectF sceneRect = scene->sceneRect();  // 获取 scene 的大小
        QRectF itemRect = item->boundingRect(); // 获取 item 的大小（局部坐标）

        // 计算 item 在 scene 中的中心位置
        QPointF centerPos = sceneRect.center() - QPointF(itemRect.width() / 2, itemRect.height() / 2);

        item->setPos(centerPos);

    }

    void ChessBoardUi::init_id_to_piece_info()
    {
        id_to_piece_info[0] = piece_info{ "Chariot", "black" };
        id_to_piece_info[1] = piece_info{ "Horse", "black" };
        id_to_piece_info[2] = piece_info{ "Elephant", "black" };
        id_to_piece_info[3] = piece_info{ "Advisor", "black" };
        id_to_piece_info[4] = piece_info{ "King", "black" };
        id_to_piece_info[5] = piece_info{ "Advisor", "black" };
        id_to_piece_info[6] = piece_info{ "Elephant", "black" };
        id_to_piece_info[7] = piece_info{ "Horse", "black" };
        id_to_piece_info[8] = piece_info{ "Chariot", "black" };
        id_to_piece_info[9] = piece_info{ "Cannon", "black" };
        id_to_piece_info[10] = piece_info{ "Cannon", "black" };
        id_to_piece_info[11] = piece_info{ "Soldier", "black" };
        id_to_piece_info[12] = piece_info{ "Soldier", "black" };
        id_to_piece_info[13] = piece_info{ "Soldier", "black" };
        id_to_piece_info[14] = piece_info{ "Soldier", "black" };
        id_to_piece_info[15] = piece_info{ "Soldier", "black" };

        id_to_piece_info[16] = piece_info{ "Chariot", "red" };
        id_to_piece_info[17] = piece_info{ "Horse", "red" };
        id_to_piece_info[18] = piece_info{ "Elephant", "red" };
        id_to_piece_info[19] = piece_info{ "Advisor", "red" };
        id_to_piece_info[20] = piece_info{ "King", "red" };
        id_to_piece_info[21] = piece_info{ "Advisor", "red" };
        id_to_piece_info[22] = piece_info{ "Elephant", "red" };
        id_to_piece_info[23] = piece_info{ "Horse", "red" };
        id_to_piece_info[24] = piece_info{ "Chariot", "red" };
        id_to_piece_info[25] = piece_info{ "Cannon", "red" };
        id_to_piece_info[26] = piece_info{ "Cannon", "red" };
        id_to_piece_info[27] = piece_info{ "Soldier", "red" };
        id_to_piece_info[28] = piece_info{ "Soldier", "red" };
        id_to_piece_info[29] = piece_info{ "Soldier", "red" };
        id_to_piece_info[30] = piece_info{ "Soldier", "red" };
        id_to_piece_info[31] = piece_info{ "Soldier", "red" };

    }



















    // board item 项目棋盘类


    ChessBoardItem::ChessBoardItem(int board_spa) :board_spa(board_spa)
    {

    }
    uint8_t ChessBoardItem::get_board_spacing() {
        return board_spa;
    }
    
    void ChessBoardItem::add_pieces(Chesspieces* piece, QPoint target_pos)
    {
        int new_x, new_y;
        QPoint target = QPoint(target_pos.x() * board_spa, target_pos.y() * board_spa);
        int piece_d = piece->boundingRect().width(); // 棋子直径
        // 规范化棋子位置
        new_x = (target.x() % board_spa > board_spa / 2 ?
            target.x() / board_spa * board_spa + board_spa :
            target.x() / board_spa * board_spa) - piece_d / 2;

        new_y = (target.y() % board_spa > board_spa / 2 ?
            target.y() / board_spa * board_spa + board_spa :
            target.y() / board_spa * board_spa) - piece_d / 2;


        piece->setParentItem(this);
        piece->setPos(new_x, new_y);
    }

    QRectF ChessBoardItem::boundingRect() const {

        return QRectF(0, 0, _wide, _long); // 棋盘大小
    }



    void ChessBoardItem::paint(QPainter* painter, const QStyleOptionGraphicsItem*, QWidget*) {
        int gridSize = board_spa;
        int cols = 9;  // 9 列（索引 0~8）
        int rows = 10; // 10 行（索引 0~9）
        int width = board_spa * (cols - 1);   
        int height = board_spa * (rows - 1); 

        // 画背景棋盘
        QPixmap bg("./ChessImage/board/board.png");
        painter->drawPixmap(boundingRect(), bg, bg.rect());

        //  画网格线
        //painter->setPen(QPen(Qt::black, 1));  // 设置黑色线条，宽度 1

        //// 画竖线
        //for (int i = 0; i < cols; ++i) {
        //    int x = i * gridSize;
        //    painter->drawLine(x, 0, x, height);
        //}

        //// 画横线
        //for (int i = 0; i < rows; ++i) {
        //    int y = i * gridSize;
        //    painter->drawLine(0, y, width, y);
        //}
    }

    void ChessBoardItem::mousePressEvent(QGraphicsSceneMouseEvent* event)
    {
        QPoint target = event->pos().toPoint();
        int new_x, new_y; 
        new_x = (target.x() % board_spa > board_spa / 2 ?
            target.x() / board_spa * board_spa + board_spa :
            target.x() / board_spa * board_spa);

        new_y = (target.y() % board_spa > board_spa / 2 ?
            target.y() / board_spa * board_spa + board_spa :
            target.y() / board_spa * board_spa);
        emit position_board(new_x/board_spa,new_y/board_spa);
    }

    

















    // 棋子类
    Chesspieces::Chesspieces(int32_t id, const QString& pieceType, const QString& pieceColor, QGraphicsItem* parent)
        : QGraphicsObject(parent),pieceType(pieceType),pieceColor(pieceColor),id(id) {
        

    }

    QRectF Chesspieces::boundingRect() const {
        return _rec;
    }

    void Chesspieces::animation_move(uint8_t x, uint8_t y)
    {
        int new_x, new_y;
        uint8_t board_spa = static_cast<ChessBoardItem*>(parentItem())->get_board_spacing();
        QPoint target = QPoint(x * board_spa, y * board_spa);
        uint8_t piece_d = _rec.width(); // 棋子直径
        // 规范化棋子位置
        new_x = (target.x() % board_spa > board_spa / 2 ?
            target.x() / board_spa * board_spa + board_spa :
            target.x() / board_spa * board_spa) - piece_d / 2;

        new_y = (target.y() % board_spa > board_spa / 2 ?
            target.y() / board_spa * board_spa + board_spa :
            target.y() / board_spa * board_spa) - piece_d / 2;

        QPoint targetPos(new_x,new_y);

        // 动画
        QPropertyAnimation* moveAnimation = new QPropertyAnimation(this, "pos",this);
        moveAnimation = new QPropertyAnimation(this, "pos");
        moveAnimation->setDuration(300);
        moveAnimation->setEasingCurve(QEasingCurve::OutCubic);
        moveAnimation->setStartValue(this->pos());
        moveAnimation->setEndValue(targetPos);
        moveAnimation->start();
        // 连接 finished 信号到 deleteLater 槽
        connect(moveAnimation, &QPropertyAnimation::finished, moveAnimation, &QObject::deleteLater);
        
    }

    void Chesspieces::animation_select()
    {
        // 以自身中心进行缩放
        this->setTransformOriginPoint(this->boundingRect().center());
        if (is_be_big == true) {
            // 缩放动画：选中时棋子缩小
            QPropertyAnimation* animation = new QPropertyAnimation(this, "scale");
            animation->setStartValue(1.2);
            animation->setEndValue(1.0);  // 放大20%
            animation->setDuration(200);  // 动画时长为200毫秒
            animation->start();
            // 连接 finished 信号到 deleteLater 槽
            connect(animation, &QPropertyAnimation::finished, animation, &QObject::deleteLater);
            is_be_big = !is_be_big;
        }
        else {
            // 缩放动画：选中时棋子放大
            QPropertyAnimation* animation = new QPropertyAnimation(this, "scale");
            animation->setStartValue(1.0);
            animation->setEndValue(1.2);  // 放大20%
            animation->setDuration(200);  // 动画时长为200毫秒
            animation->start();
            // 连接 finished 信号到 deleteLater 槽
            connect(animation, &QPropertyAnimation::finished, animation, &QObject::deleteLater);
            is_be_big = !is_be_big;
        }
    }

    void Chesspieces::animation_click()
    {
    }

    void Chesspieces::hide_me()
    {
        setVisible(false);
    }

    void Chesspieces::show_me()
    {
        setVisible(true);
    }




    QPoint Chesspieces::get_cur_pos()
    {
        QPoint tmp = this->pos().toPoint() + this->boundingRect().center().toPoint();
        return  tmp;
    }

    void Chesspieces::mousePressEvent(QGraphicsSceneMouseEvent* event) {
        emit piece_click(id);
    }
    void Chesspieces::paint(QPainter* painter, const QStyleOptionGraphicsItem*, QWidget*) {

        QString path = QString("./ChessImage/pieces/%1_%2.ico").arg(pieceType).arg(pieceColor);
        QPixmap pixmap(path);
        if (pixmap.isNull()) {
            qWarning() << "Failed to load piece image";
            return;
        }
        QPixmap scaledPixmap = pixmap.scaled(boundingRect().size().toSize(), Qt::KeepAspectRatioByExpanding, Qt::SmoothTransformation);

        painter->drawPixmap(0, 0, scaledPixmap);  // 绘制棋子


        //painter->setPen(QPen(Qt::red, 2));  // 设置红色边框，线宽2
        //painter->drawRect(boundingRect());  // 画出 item 的边界

    }

  











    // 显示类
    AutoResizeView::AutoResizeView(QWidget* parent) :_scene(new ChessBoardUi(this))
    {
        this->setScene(_scene);
        setBackgroundBrush(Qt::transparent); // 关键：视图背景透明

    }
    ChessBoardUi* AutoResizeView::get_scene() {
        return _scene;
    }
    void AutoResizeView::resizeEvent(QResizeEvent* event) {
        QGraphicsView::resizeEvent(event);
        if (scene() && !scene()->items().isEmpty()) {
            fitInView(scene()->itemsBoundingRect(), Qt::KeepAspectRatio);
        }
    }
};