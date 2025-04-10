#ifndef WELCOME_H
#define WELCOME_H

#include <QMainWindow>
#include "./ui_mainwindow.h"
#include "chessboardui.h"
#include <QGraphicsView>
#include <QVBoxLayout>
#include "chessboard.h"
namespace Ui {
class welcome;
}

class welcome : public QWidget
{
    Q_OBJECT

public:
    explicit welcome(QWidget *parent = nullptr);
    ~welcome();

private slots:
    void on_local_mode_clicked();

    void on_friend_mode_clicked();

private:
    Ui::welcome *ui;
signals:
    void show_board();

};

#endif // WELCOME_H
