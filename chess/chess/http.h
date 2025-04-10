#ifndef HTTP_H
#define HTTP_H

#include <QObject>
#include <global.h>
class http :  public QObject ,public single_class<http>  // class http : public single_class<http>, public QObject 这样的话，代码出错，还需要注意继承顺序    
{
    Q_OBJECT
    
public:
    friend single_class<http>;
    void post(QUrl url,QJsonObject json,Model model);
    void get(QUrl url,Model model);

private:
    void handle_result(QJsonObject json ,Model model);
    http() ;
    QString ip ;
    uint16_t port;
    QNetworkAccessManager _manager;
signals:
    void http_register_finish(QJsonObject json);
};

#endif // HTTP_H
