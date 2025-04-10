#include "mainwindow.h"
#include <QFile>
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    // 加载 qss 文件
    QFile qss(":/style/ui.qss");
    if(qss.open(QFile::ReadOnly)){
        QString style = QLatin1String(qss.readAll());
        a.setStyleSheet(style);
        qss.close();

    }else{
        qDebug()<<"open failed";
    }
    MainWindow w;
    w.show();
    return a.exec();
}
