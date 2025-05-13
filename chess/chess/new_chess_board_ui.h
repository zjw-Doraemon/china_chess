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
    // scene ������
    class ChessBoardUi : public QGraphicsScene
    {
        Q_OBJECT
    
    public:
        ChessBoardUi(QObject* parent = nullptr);
        


    // ���ⲿ�ṩ�Ľӿ�
    public slots:
        void paint_chess_pieces(uint8_t id, uint8_t x, uint8_t y); //�������ӵ�����
        void move_piece(uint8_t id, uint8_t x, uint8_t y);// �ƶ�����λ��
        void remove_piece(uint8_t id);//ɾ��һ������ �ڴ�ɾ��
        
        void click_piece(uint8_t id);//���һ������
    signals:
        void board_click(uint8_t x, uint8_t y);
        void piece_click(uint8_t id);




    private:
        // Ƕ���࣬ʹ�� public ����Ȩ���Ա��ⲿ���ʳ�Ա
        class piece_info {
        public:
            QString type;
            QString color;
        };
    private:
        void  centerItemInScene(QGraphicsItem* item, QGraphicsScene* scene);  // ��item����
        void init_id_to_piece_info();
    private:
        ChessBoardItem *_chessboard; // ������
        std::unordered_map<uint8_t, piece_info> id_to_piece_info;
        std::unordered_map<uint8_t, Chesspieces*> id_to_pieces;

    };





    class ChessBoardItem : public QGraphicsObject {
        Q_OBJECT

    public:
        ChessBoardItem(int board_spacing = 60);
        void add_pieces(Chesspieces* piece, QPoint pos);
        QRectF boundingRect() const override; // �������̱߽�
        uint8_t get_board_spacing(); // ��ȡ���̸��Ӽ��
        


    private: 
        void mousePressEvent(QGraphicsSceneMouseEvent* event);
        void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget) override; // ��������       
    private:
        uint8_t board_spa; // ���Ӽ��
        uint16_t _long = 540;
        uint16_t _wide = 480;
    signals:
        void position_board(uint8_t x, uint8_t y); // ���͵�����̵�λ��

    };







    // ������

    // ����item��
    class Chesspieces : public QGraphicsObject {
        Q_OBJECT
    public:
        Chesspieces(int32_t id,const QString& pieceType, const QString& pieceColor, QGraphicsItem* parent = nullptr);
        QRectF boundingRect() const override;
        void animation_move(uint8_t x, uint8_t y);
        void animation_select();
        void animation_click();
        void hide_me();// �����Լ�
        void show_me();// չʾ�Լ�
    signals:
        void piece_click(uint8_t id); // ���ӵ���ź�




    private:
        QPoint get_cur_pos(); // ��ȡ��ǰ���� ���ĵ�����
        void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget) override;
        void mousePressEvent(QGraphicsSceneMouseEvent* event);
        bool is_be_big = false; // ��ǰ�����Ƿ��ڷŴ�״̬
    private:
        QRectF _rec = QRectF(0, 0, 55, 55); // ���Ӿ���
        int32_t id;
        QString pieceType; // ��������
        QString pieceColor; // ������ɫ
    };








    // �ӽ���
    class AutoResizeView : public QGraphicsView {
        Q_OBJECT
    public:
        AutoResizeView(QWidget* parent);
        ChessBoardUi* get_scene();
        


    private:
        void resizeEvent(QResizeEvent* event) override; // �������ڴ�С�仯
    private:
        ChessBoardUi* _scene;
    };

};