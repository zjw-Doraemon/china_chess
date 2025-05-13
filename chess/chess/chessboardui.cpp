#include "chessboardui.h"
#include <QGraphicsSceneMouseEvent>
#include <iostream>
#include <QCoreApplication>
#include <QGraphicsDropShadowEffect>
#include <QPropertyAnimation>
#include <QTimer>
#include "new_chess_board_ui.h"
// scene 场景容器类
void ChessBoardUi::init_view_red_or_black(QString view)
{
    // 棋子初始化数据
    struct PieceInfo {
        int id;
        QString color;
        QString type;
        QPoint position;
        QString imagePath;
    };
    // 红方视角
    QVector<PieceInfo> red_view = {
        {0, "black", "车", {0, 0}, ":/ChessImage/che.ico"},
        {1, "black", "马", {1, 0}, ":/ChessImage/ma.ico"},
        {2, "black", "象", {2, 0}, ":/ChessImage/xiang.ico"},
        {3, "black", "士", {3, 0}, ":/ChessImage/shi.ico"},
        {4, "black", "将", {4, 0}, ":/ChessImage/jiang.ico"},
        {5, "black", "士", {5, 0}, ":/ChessImage/shi.ico"},
        {6, "black", "象", {6, 0}, ":/ChessImage/xiang.ico"},
        {7, "black", "马", {7, 0}, ":/ChessImage/ma.ico"},
        {8, "black", "车", {8, 0}, ":/ChessImage/che.ico"},
        {9, "black", "炮", {1, 2}, ":/ChessImage/pao.ico"},
        {10, "black", "炮", {7, 2}, ":/ChessImage/pao.ico"},
        {11, "black", "卒", {0, 3}, ":/ChessImage/bing.ico"},
        {12, "black", "卒", {2, 3}, ":/ChessImage/bing.ico"},
        {13, "black", "卒", {4, 3}, ":/ChessImage/bing.ico"},
        {14, "black", "卒", {6, 3}, ":/ChessImage/bing.ico"},
        {15, "black", "卒", {8, 3}, ":/ChessImage/bing.ico"},

        {16, "red", "车", {0, 9}, ":/ChessImage/che_red.ico"},
        {17, "red", "马", {1, 9}, ":/ChessImage/ma_red.ico"},
        {18, "red", "相", {2, 9}, ":/ChessImage/xiang_red.ico"},
        {19, "red", "仕", {3, 9}, ":/ChessImage/shi_red.ico"},
        {20, "red", "帅", {4, 9}, ":/ChessImage/jiang_red.ico"},
        {21, "red", "仕", {5, 9}, ":/ChessImage/shi_red.ico"},
        {22, "red", "相", {6, 9}, ":/ChessImage/xiang_red.ico"},
        {23, "red", "马", {7, 9}, ":/ChessImage/ma_red.ico"},
        {24, "red", "车", {8, 9}, ":/ChessImage/che_red.ico"},
        {25, "red", "炮", {1, 7}, ":/ChessImage/pao_red.ico"},
        {26, "red", "炮", {7, 7}, ":/ChessImage/pao_red.ico"},
        {27, "red", "兵", {0, 6}, ":/ChessImage/bing_red.ico"},
        {28, "red", "兵", {2, 6}, ":/ChessImage/bing_red.ico"},
        {29, "red", "兵", {4, 6}, ":/ChessImage/bing_red.ico"},
        {30, "red", "兵", {6, 6}, ":/ChessImage/bing_red.ico"},
        {31, "red", "兵", {8, 6}, ":/ChessImage/bing_red.ico"}
    };

    // 黑方视角
    QVector<PieceInfo> black_view = {
        {16, "red", "车", {0, 0}, ":/ChessImage/che_red.ico"},
        {17, "red", "马", {1, 0}, ":/ChessImage/ma_red.ico"},
        {18, "red", "相", {2, 0}, ":/ChessImage/xiang_red.ico"},
        {19, "red", "仕", {3, 0}, ":/ChessImage/shi_red.ico"},
        {20, "red", "帅", {4, 0}, ":/ChessImage/jiang_red.ico"},
        {21, "red", "仕", {5, 0}, ":/ChessImage/shi_red.ico"},
        {22, "red", "相", {6, 0}, ":/ChessImage/xiang_red.ico"},
        {23, "red", "马", {7, 0}, ":/ChessImage/ma_red.ico"},
        {24, "red", "车", {8, 0}, ":/ChessImage/che_red.ico"},
        {25, "red", "炮", {1, 2}, ":/ChessImage/pao_red.ico"},
        {26, "red", "炮", {7, 2}, ":/ChessImage/pao_red.ico"},
        {27, "red", "兵", {0, 3}, ":/ChessImage/bing_red.ico"},
        {28, "red", "兵", {2, 3}, ":/ChessImage/bing_red.ico"},
        {29, "red", "兵", {4, 3}, ":/ChessImage/bing_red.ico"},
        {30, "red", "兵", {6, 3}, ":/ChessImage/bing_red.ico"},
        {31, "red", "兵", {8, 3}, ":/ChessImage/bing_red.ico"},

        {0, "black", "车", {0, 9}, ":/ChessImage/che.ico"},
        {1, "black", "马", {1, 9}, ":/ChessImage/ma.ico"},
        {2, "black", "象", {2, 9}, ":/ChessImage/xiang.ico"},
        {3, "black", "士", {3, 9}, ":/ChessImage/shi.ico"},
        {4, "black", "将", {4, 9}, ":/ChessImage/jiang.ico"},
        {5, "black", "士", {5, 9}, ":/ChessImage/shi.ico"},
        {6, "black", "象", {6, 9}, ":/ChessImage/xiang.ico"},
        {7, "black", "马", {7, 9}, ":/ChessImage/ma.ico"},
        {8, "black", "车", {8, 9}, ":/ChessImage/che.ico"},
        {9, "black", "炮", {1, 7}, ":/ChessImage/pao.ico"},
        {10, "black", "炮", {7, 7}, ":/ChessImage/pao.ico"},
        {11, "black", "卒", {0, 6}, ":/ChessImage/bing.ico"},
        {12, "black", "卒", {2, 6}, ":/ChessImage/bing.ico"},
        {13, "black", "卒", {4, 6}, ":/ChessImage/bing.ico"},
        {14, "black", "卒", {6, 6}, ":/ChessImage/bing.ico"},
        {15, "black", "卒", {8, 6}, ":/ChessImage/bing.ico"}
    };


    //创建棋子并添加到棋盘
    // 创建棋子
    if(view == "red"){

        for (const auto &piece : red_view) {
            Chesspieces *chessPiece = new Chesspieces(piece.id,piece.color, piece.imagePath);
             // 连接点击棋子的信号  管理基本的吃子信号
            connect(chessPiece, &Chesspieces::click_select_sent_id, this, &ChessBoardUi::update_newpiece_and_move);
            // 放入map中
            _chess_pieces[piece.id] = chessPiece;
            // 加入棋盘
            _chessboard->add_pieces(chessPiece, piece.position);
            
            
        }
        
    }
    if(view == "black"){
        for (const auto &piece : black_view) {
            Chesspieces *chessPiece = new Chesspieces(piece.id,piece.color, piece.imagePath);
            connect(chessPiece, &Chesspieces::click_select_sent_id, this, &ChessBoardUi::update_newpiece_and_move);
                
            // 放入map中
            _chess_pieces[piece.id] = chessPiece;
            // 加入棋盘
            _chessboard->add_pieces(chessPiece,piece.position);
            
            
        }
        
    }
    piece_diameter = _chess_pieces[0]->get_diameter();  // 初始化棋子直径
}


void ChessBoardUi:: centerItemInScene(QGraphicsItem* item, QGraphicsScene* scene) {
    if (!item || !scene) return;

    QRectF sceneRect = scene->sceneRect();  // 获取 scene 的大小
    QRectF itemRect = item->boundingRect(); // 获取 item 的大小（局部坐标）

    // 计算 item 在 scene 中的中心位置
    QPointF centerPos = sceneRect.center() - QPointF(itemRect.width() / 2, itemRect.height() / 2);

    item->setPos(centerPos);

}



ChessBoardUi::ChessBoardUi(QObject *parent,QString view)
    : QGraphicsScene(parent)
    
    ,_chessboard(new ChessBoardItem() )
{
    //设置大小
    setSceneRect(0, 0, 600, 650); // 设置 scene 的范围
    // 画边框
    QGraphicsRectItem* border = this->addRect(this->sceneRect(), QPen(Qt::red, 3));
    border->setZValue(1);  // 让边框位于最顶层
    // 添加棋盘 居中棋盘
    addItem(_chessboard);
    centerItemInScene(_chessboard,this);
   
    // 设置背景
    setBackgroundBrush(QPixmap(":/ChessImage/back.jpg"));

    //连接棋盘点击信号
    connect(_chessboard, &ChessBoardItem::position_board, this, [this](QPoint target_pos) {
        // 如果封印取消相应
        if (is_seal) {
            return;
        }
        QPoint pos = point_grid(target_pos); // 棋子坐标规范化
        if (selected_pieces != -1) {
            auto tmp = cur_pieces;
            QPoint cur = QPoint(tmp->get_cur_pos().x() / _chessboard->get_board_spacing(), tmp->get_cur_pos().y() / _chessboard->get_board_spacing());
            QPoint target = QPoint((pos.x() + piece_diameter / 2) / _chessboard->get_board_spacing(), (pos.y() + piece_diameter / 2) / _chessboard->get_board_spacing());
            // 发送请求 是否可以移动
            emit PieceMove(selected_pieces, cur, target);
               
            
            
        }
        });
    // 获取后端返回信息
    //connect();

}

Chesspieces *ChessBoardUi::get_pieces_from_id(int id)
{
    return _chess_pieces[id];
}
// 点击棋子
void ChessBoardUi::update_newpiece_and_move(int id)
{
    // 如果封印取消相应
    if (is_seal) {
        return;
    }
    
    if (selected_pieces == -1) {
        select_piece(id);
        return;
    }
    if (selected_pieces == id) {
        cancel_select(id);
        return;
    }

    // 是自己队伍的棋子
    auto last = get_pieces_from_id(selected_pieces);
    auto new_piece = get_pieces_from_id(id);
    if ( last->get_color() == new_piece->get_color()) {
        cancel_select(selected_pieces);
        select_piece(id);
        return;

    }
    // 吃掉对方棋子 点击其他棋子
    
    
    auto to_eat = this->get_pieces_from_id(id);
    to_pieces = to_eat;  // 设置第二次被点击的棋子
    QPoint cur = QPoint(cur_pieces->get_cur_pos().x() / _chessboard->get_board_spacing(),
        cur_pieces->get_cur_pos().y() / _chessboard->get_board_spacing());
    QPoint to = QPoint(to_eat->get_cur_pos().x() / _chessboard->get_board_spacing(),
        to_eat->get_cur_pos().y() / _chessboard->get_board_spacing());
    emit PieceMove(selected_pieces, cur, to);
    

    //new_piece->setVisible(false);
    //last->playMoveAnimation(point_grid(new_piece->get_cur_pos()));
    //// 这里可以添加 被吃掉棋子的容器 方便悔棋
    ////delete new_piece; // 删除棋子
    //last->change_is_selected(false);
    //this->selected_pieces = -1;
        

    
}
// 规范化的坐标是 左上角坐标并不是 网格中心点
QPoint ChessBoardUi::point_grid(QPoint target) {
    int board_spa = _chessboard->get_board_spacing(); // 棋盘间距
    int piece_d = piece_diameter; // 棋子直径

    int mod_x = target.x() % board_spa;
    int mod_y = target.y() % board_spa;

    int base_x = (target.x() / board_spa) * board_spa;  // 当前网格的左侧边界
    int base_y = target.y() - mod_y;  // 当前网格的上侧边界

    // 判断是否超过网格中心
    bool isRightHalf = (mod_x > board_spa / 2);
    bool isBottomHalf = (mod_y > board_spa / 2);

    // 计算新坐标
    int new_x = base_x + (isRightHalf ? board_spa : 0) - piece_d / 2;
    int new_y = base_y + (isBottomHalf ? board_spa : 0) - piece_d / 2;

    return QPoint(new_x, new_y);
}



void ChessBoardUi::select_piece(int id)
{
    auto select = get_pieces_from_id(id);
    cur_pieces = select;
    select->change_is_selected(true);
    selected_pieces = id;
}

void ChessBoardUi::cancel_select(int id)
{
    auto un_select = get_pieces_from_id(id);
    selected_pieces = -1;
    un_select->change_is_selected(false);

}

void ChessBoardUi::show_checkmate_text() {
    // 创建文本 Item
    QGraphicsTextItem* textItem = new QGraphicsTextItem("绝杀");
    QFont font("Arial", 80, QFont::Bold);
    textItem->setFont(font);
    textItem->setDefaultTextColor(Qt::red);

    // 设置位置（棋盘中心）
    QRectF sceneRect = this->sceneRect();
    textItem->setPos(sceneRect.width() / 2 - 100, sceneRect.height() / 2 - 50);

    this->addItem(textItem);

    // 创建透明度效果
    QGraphicsOpacityEffect* opacityEffect = new QGraphicsOpacityEffect;
    textItem->setGraphicsEffect(opacityEffect);

    // 设置淡入动画
    QPropertyAnimation* fadeIn = new QPropertyAnimation(opacityEffect, "opacity");
    fadeIn->setDuration(1000);
    fadeIn->setStartValue(0.0);
    fadeIn->setEndValue(1.0);
    fadeIn->start();

    // 2 秒后开始淡出
    QTimer::singleShot(2000, [=]() {
        QPropertyAnimation* fadeOut = new QPropertyAnimation(opacityEffect, "opacity");
        fadeOut->setDuration(1000);
        fadeOut->setStartValue(1.0);
        fadeOut->setEndValue(0.0);
        fadeOut->start();

        // 动画结束后删除
        QObject::connect(fadeOut, &QPropertyAnimation::finished, [=]() {
            this->removeItem(textItem);
            delete textItem;
            });
        });
}

void ChessBoardUi::clear_piece()
{
    QList<QGraphicsItem*> childItems = _chessboard->childItems();

    // 遍历所有子项
    for (QGraphicsItem* child : childItems) {
        // 递归清除子项的子项
        

        // 从场景中移除子项
        if (child->scene()) {
            child->scene()->removeItem(child);
        }

        // 删除子项（如果需要）
        delete child;
    }
}

void ChessBoardUi::judge_win()
{
    show_checkmate_text(); //动画播放
    

}

void ChessBoardUi::web_move(int id, QPoint target , bool move)
{
    if(move == false) {
        return;
    }
    // 吃子
    if (to_pieces != nullptr) {
        int board_spa = _chessboard->get_board_spacing(); // 棋盘间距
        QPoint end = point_grid(QPoint(target.x() * board_spa, target.y() * board_spa));

        to_pieces->setVisible(false);
        auto tmp = cur_pieces;
        tmp->playMoveAnimation(end);
        cancel_select(id);
        to_pieces = nullptr;
    }
    // 点击棋盘
    else {
        int board_spa = _chessboard->get_board_spacing(); // 棋盘间距
        QPoint end = point_grid(QPoint(target.x() * board_spa, target.y() * board_spa));

        auto tmp = cur_pieces;
        tmp->playMoveAnimation(end);
        cancel_select(id);

    }
}

void ChessBoardUi::can_move(int id,QPoint target,bool move) 
{
    if (move == false) {
        cancel_select(id);
        to_pieces = nullptr;
        return;
    }
    // 吃子
    if (to_pieces != nullptr) {
        int board_spa = _chessboard->get_board_spacing(); // 棋盘间距
        QPoint end = point_grid(QPoint(target.x() * board_spa, target.y() * board_spa));

        to_pieces->setVisible(false);
        auto tmp = cur_pieces;
        tmp->playMoveAnimation(end);
        cancel_select(id);
        to_pieces = nullptr;
    }
    // 点击棋盘
    else {
        int board_spa = _chessboard->get_board_spacing(); // 棋盘间距
        QPoint end = point_grid(QPoint(target.x() * board_spa, target.y() * board_spa));

        auto tmp = cur_pieces;
        tmp->playMoveAnimation(end);
        cancel_select(id);
        
    }

}
























// board item 项目棋盘类


ChessBoardItem::ChessBoardItem(int board_spa):board_spacing(board_spa)
{

}

void ChessBoardItem::add_pieces(Chesspieces* piece, QPoint target_pos)
{
    int new_x, new_y;
    QPoint target = QPoint(target_pos.x() * board_spacing, target_pos.y() * board_spacing);
    piece->setParentItem(this);
    int board_spa = this->get_board_spacing(); // 棋盘间距
    int piece_d = piece->get_diameter(); // 棋子直径
    
    new_x = (target.x() % board_spa > board_spa / 2 ?
        target.x() / board_spa * board_spa + board_spa :
        target.x() / board_spa * board_spa) - piece_d / 2;

    new_y = (target.y() % board_spa > board_spa / 2 ?
        target.y() / board_spa * board_spa + board_spa :
        target.y() / board_spa * board_spa) - piece_d / 2;
    
    piece->setPos(new_x,new_y);
}

QRectF ChessBoardItem::boundingRect() const {

    return QRectF(0, 0, _wide, _long); // 棋盘大小
}



void ChessBoardItem::paint(QPainter* painter, const QStyleOptionGraphicsItem*, QWidget*) {
    int gridSize = board_spacing;
    int cols = 9;  // 9 列（索引 0~8）
    int rows = 10; // 10 行（索引 0~9）
    int width = board_spacing * (cols - 1);   // 8 * 60
    int height = board_spacing * (rows - 1);  // 9 * 60

    // 画背景棋盘
     QPixmap bg(":/ChessImage/board_full.png");
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
    QPoint pos = event->pos().toPoint();
    emit position_board(pos);
}

int ChessBoardItem::get_board_spacing()
{
    return board_spacing;
}






















// pieces item 项目棋子类
void Chesspieces::playMoveAnimation(QPoint targetPos) {
    if (!moveAnimation) {
        moveAnimation = new QPropertyAnimation(this, "pos");
        moveAnimation->setDuration(300);
        moveAnimation->setEasingCurve(QEasingCurve::OutCubic);
        moveAnimation->setParent(this);
    }
    else {
        moveAnimation->stop();  // 停止当前动画
    }

    moveAnimation->setStartValue(this->pos());
    moveAnimation->setEndValue(targetPos);
    moveAnimation->start();
    
}


void Chesspieces::playScaleAnimation(bool selected) {
    

    // 以自身中心进行缩放
    this->setTransformOriginPoint(this->boundingRect().center());

    // 若动画未创建，则创建
    if (!scaleAnimation) {
        scaleAnimation = new QPropertyAnimation(this, "scale");
        scaleAnimation->setDuration(300);
        scaleAnimation->setEasingCurve(QEasingCurve::OutBack);
        scaleAnimation->setParent(this);  // 绑定到对象，自动管理内存

        connect(scaleAnimation, &QPropertyAnimation::finished, this, [this]() {
            scaleAnimation->stop();  // 确保动画完全停止
            });
    }
    else {
        scaleAnimation->stop();  // 停止当前动画
    }

    // 设置动画起始和结束值
    scaleAnimation->setStartValue(this->scale());
    scaleAnimation->setEndValue(selected ? 1.2 : 1.0);
    
    scaleAnimation->start();
    prepareGeometryChange();
    update();
    
    
}



Chesspieces::Chesspieces(int id,QString color, const QString &picture_path, QRectF rec, QGraphicsItem *parent)
    : QGraphicsObject(parent), id(id), picture_path(picture_path),color(color),_rec(rec) {


}
QString Chesspieces::get_picture_path(){
    return picture_path;
}

void Chesspieces::mousePressEvent(QGraphicsSceneMouseEvent *event)
{

    //qDebug() << "棋子界面被点击！";
    
    //this->setProperty("is_selected",is_selected);
    emit click_select_sent_id(id,is_selected);

}
void Chesspieces::change_is_selected(bool select)
{
    is_selected = select;
    this->playScaleAnimation(select);
}
int Chesspieces::get_id()
{
    return id;
}
QString Chesspieces::get_color()
{
    return color;
}
QPoint Chesspieces::get_cur_pos()
{
    QPoint tmp = this->pos().toPoint() + this->boundingRect().center().toPoint();
    return  tmp;
}
int Chesspieces::get_diameter()
{
    int diameter = _rec.width();
    return diameter;
}
QRectF Chesspieces::boundingRect() const {
    return _rec;
}

void Chesspieces::paint(QPainter *painter, const QStyleOptionGraphicsItem *, QWidget *) {


    QPixmap pixmap(picture_path);
    if (pixmap.isNull()) {
        qWarning() << "Failed to load piece image";
        return;
    }
    QPixmap scaledPixmap = pixmap.scaled(boundingRect().size().toSize(), Qt::KeepAspectRatioByExpanding, Qt::SmoothTransformation);

    painter->drawPixmap(0, 0, scaledPixmap);  // 绘制棋子


    //painter->setPen(QPen(Qt::red, 2));  // 设置红色边框，线宽2
    //painter->drawRect(boundingRect());  // 画出 item 的边界

}
























QRectF usr_item::boundingRect() const
{
    return QRectF();
}

void usr_item::paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget)
{
}
































// view  视角类

void AutoResizeView::init_board(QString view)
{
    _scene->init_view_red_or_black(view);
}

AutoResizeView::AutoResizeView(QWidget *parent):_scene(new ChessBoardUi(this))
    {
    this->setScene(_scene);
    setRenderHint(QPainter::Antialiasing); // 抗锯齿，优化显示
    setRenderHint(QPainter::SmoothPixmapTransform);
    //setDragMode(QGraphicsView::NoDrag); // 禁用拖拽
    //setBackgroundBrush(QColor(173, 216, 230));
    
}

void AutoResizeView::clear_pieces()
{
    _scene->clear_piece();
}

ChessBoardUi *AutoResizeView::get_scene()
{
    return _scene;
}


void AutoResizeView::resizeEvent(QResizeEvent *event) {
    QGraphicsView::resizeEvent(event);
    if (scene() && !scene()->items().isEmpty()) {
        fitInView(scene()->itemsBoundingRect(), Qt::KeepAspectRatio);
    }
}

