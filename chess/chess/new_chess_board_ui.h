#pragma once

#include <QGraphicsScene>
#include <QGraphicsItem>
#include <QPainter>
#include <QGraphicsView>
#include <QTimer>
#include <QPoint.h>
#include <vector>
#include <map>
#include <unordered_map>

namespace new_chess_board_ui {
    class ChessBoardItem;
    class Chesspieces;
    // scene 管理类
    class ChessBoardUi : public QGraphicsScene
    {
        Q_OBJECT
    
    public:
        ChessBoardUi(QObject* parent = nullptr);
        


    // 向外部提供的接口
    public slots:
        void paint_chess_pieces(uint8_t id, uint8_t x, uint8_t y); //绘制棋子到棋盘
        void move_piece(uint8_t id, uint8_t x, uint8_t y);// 移动棋子位置
        void remove_piece(uint8_t id);//删除一个棋子 内存删除
        
        void click_piece(uint8_t id);//点击一个棋子
    signals:
        void board_click(uint8_t x, uint8_t y);
        void piece_click(uint8_t id);




    private:
        // 嵌套类，使用 public 访问权限以便外部访问成员
        class piece_info {
        public:
            QString type;
            QString color;
        };
    private:
        void  centerItemInScene(QGraphicsItem* item, QGraphicsScene* scene);  // 将item居中
        void init_id_to_piece_info();
    private:
        ChessBoardItem *_chessboard; // 棋盘类
        std::unordered_map<uint8_t, piece_info> id_to_piece_info;
        std::unordered_map<uint8_t, Chesspieces*> id_to_pieces;

    };





    class ChessBoardItem : public QGraphicsObject {
        Q_OBJECT

    public:
        ChessBoardItem(int board_spacing = 60);
        void add_pieces(Chesspieces* piece, QPoint pos);
        QRectF boundingRect() const override; // 设置棋盘边界
        uint8_t get_board_spacing(); // 获取棋盘格子间距
        


    private: 
        void mousePressEvent(QGraphicsSceneMouseEvent* event);
        void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget) override; // 绘制棋盘       
    private:
        uint8_t board_spa; // 格子间距
        uint16_t _long = 540;
        uint16_t _wide = 480;
    signals:
        void position_board(uint8_t x, uint8_t y); // 发送点击棋盘的位置

    };







    // 棋子类

    // 棋子item类
    class Chesspieces : public QGraphicsObject {
        Q_OBJECT
    public:
        Chesspieces(int32_t id,const QString& pieceType, const QString& pieceColor, QGraphicsItem* parent = nullptr);
        QRectF boundingRect() const override;
        void animation_move(uint8_t x, uint8_t y);
        void animation_select();
        void animation_click();
        void hide_me();// 隐藏自己
        void show_me();// 展示自己
    signals:
        void piece_click(uint8_t id); // 棋子点击信号




    private:
        QPoint get_cur_pos(); // 获取当前坐标 中心点坐标
        void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget) override;
        void mousePressEvent(QGraphicsSceneMouseEvent* event);
        bool is_be_big = false; // 当前棋子是否在放大状态
    private:
        QRectF _rec = QRectF(0, 0, 55, 55); // 棋子矩形
        int32_t id;
        QString pieceType; // 棋子类型
        QString pieceColor; // 棋子颜色
    };








    // 视角类
    class AutoResizeView : public QGraphicsView {
        Q_OBJECT
    public:
        AutoResizeView(QWidget* parent);
        ChessBoardUi* get_scene();
        


    private:
        void resizeEvent(QResizeEvent* event) override; // 监听窗口大小变化
    private:
        ChessBoardUi* _scene;
    };

};