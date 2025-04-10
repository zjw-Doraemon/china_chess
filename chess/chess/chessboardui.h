#ifndef CHESSBOARDUI_H
#define CHESSBOARDUI_H
#include <QGraphicsScene>
#include <QGraphicsItem>
#include <QPainter>
#include <QGraphicsView>

#include <QPoint.h>
#include <vector>
#include <map>

class QPropertyAnimation;
// 整体 scene 类
class ChessBoardItem;
class Chesspieces;
class ChessBoardUi :  public QGraphicsScene
{
    Q_OBJECT
public:
    void init_view_red_or_black(QString); // 初始化棋盘
    void  centerItemInScene(QGraphicsItem* item, QGraphicsScene* scene);  // 将item居中

    ChessBoardUi(QObject * parent = nullptr,QString view = "red");
    Chesspieces* get_pieces_from_id(int);
    QPoint point_grid(QPoint);  // 坐标棋盘规范化
    /*初始化后端棋局   这里无奈的很，以后可能要重新设计因为，
    由于connect操作在对象初始化之后，所以只能在外部初始化后端棋局了*/

    void select_piece(int id);
    void cancel_select(int id);
    void show_checkmate_text(); // 绝杀动画
    void clear_piece();
    

private:
    int piece_diameter; //棋子直径 初始化棋盘之后再设置
    int selected_pieces = -1; // 当前选中棋子
    Chesspieces* cur_pieces = nullptr; // 当前选中棋子的指针
    Chesspieces* to_pieces = nullptr; // 可能被吃的棋子的指针
    ChessBoardItem* _chessboard; // 棋盘类
    std::map<int, Chesspieces*> _chess_pieces; //棋子集合
    QString board_view;


public slots:
    void update_newpiece_and_move(int id); //吃子逻辑 类
    void judge_win();

// 与后端交互
public slots:
    
    void can_move(int,QPoint,bool); // 是否可以移动
    
signals:
    void PieceMove(int id, QPoint start, QPoint end);

};










// 棋盘item类
class ChessBoardItem : public QGraphicsObject {
    Q_OBJECT

public:
    ChessBoardItem(int board_spacing = 60);
    void add_pieces(Chesspieces* piece, QPoint pos);
    QRectF boundingRect() const override; // 设置棋盘边界
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override; // 绘制棋盘
    void mousePressEvent(QGraphicsSceneMouseEvent* event);
    int get_board_spacing ();
   


private:
    int board_spacing ;
signals:
    void position_board(QPoint);

};












// 棋子item类
class Chesspieces : public QGraphicsObject{
    Q_OBJECT
public:
    Chesspieces(int id, QString color, const QString &picture_path, QRectF rec = QRectF(0, 0, 60, 60),QGraphicsItem *parent = nullptr);
    QRectF boundingRect() const override;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;
    QString get_picture_path();
    void mousePressEvent(QGraphicsSceneMouseEvent *event);
    void change_is_selected(bool select);  // 改变当前选中状态  改变即播放选中动画
    int get_id();
    QString get_color();
    QPoint get_cur_pos(); // 获取当前坐标 中心点坐标
    int get_diameter();//获取棋子直径

public:
    void playMoveAnimation(QPoint targetPos); //棋子移动动画
private:
    void playScaleAnimation(bool); //缓慢放大缩小动画函数

private:
    int id;
    QString picture_path;
    bool is_selected = false; // 默认初始化为未选中
    QString color;
    QRectF _rec ; // 棋子矩形
    QPropertyAnimation *scaleAnimation = nullptr;// 缩放动画对象
    QPropertyAnimation* moveAnimation = nullptr; //移动动画对象
signals:
    void click_select_sent_id(int id , bool is_select);
    void click_pos(QPoint);
};











// 用户信息item
class usr_item : public QGraphicsObject {
    Q_OBJECT
public:
    QRectF boundingRect() const override;
    void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget) override;
private:

};













// 视角类
class AutoResizeView : public QGraphicsView {
    Q_OBJECT
public:
    void init_board(QString);
    explicit AutoResizeView( QWidget *parent = nullptr);
    void clear_pieces();
    ChessBoardUi* get_scene();

protected:
    void resizeEvent(QResizeEvent *event) override; // 监听窗口大小变化
private:
    ChessBoardUi* _scene;
};









#endif // CHESSBOARDITEM_H
