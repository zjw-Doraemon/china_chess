#ifndef CUSTOM_WIDGET_H
#define CUSTOM_WIDGET_H

#include <QObject>
#include <QPushButton>
class custom_widget
{
public:
    custom_widget();
};





class ClickedBtn:public QPushButton
{
    Q_OBJECT
public:
    ClickedBtn(QWidget *parent = nullptr);
    ~ClickedBtn();
    void SetState(QString nomal, QString hover, QString press);
protected:
    virtual void enterEvent(QEnterEvent *event) override; // 鼠标进入
    virtual void leaveEvent(QEvent *event) override; // 鼠标离开
    virtual void mousePressEvent(QMouseEvent *event) override; // 鼠标按下
    virtual void mouseReleaseEvent(QMouseEvent *event) override; // 鼠标释放
private:
    QString _normal;
    QString _hover;
    QString _press;
};



















#endif // CUSTOM_WIDGET_H
