#include "http.h"
#include <iostream>

void http::post(QUrl url,QJsonObject json,Model model)
{
    url.setScheme("http");
    url.setHost(ip);
    url.setPort(port);
    QByteArray data = QJsonDocument(json).toJson();
    QNetworkRequest request(url);
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");  //设置数据类型
    QNetworkReply *reply = _manager.post(request,data);

    QObject::connect(reply, &QNetworkReply::finished, [=]() {
        if(reply->error() != QNetworkReply::NoError){

            reply->deleteLater();
            std::cout << reply->errorString().toStdString();
            std::cout<< "connect failed"<<std::endl;
            return;
        }
        //std::cout<< "connect success"<<std::endl;

        // 读取返回数据
        QByteArray responseData = reply->readAll();
        
        QJsonDocument doc = QJsonDocument::fromJson(responseData);
        if (doc.isNull()) {
            std::cout << "JSON parse failed" << responseData.toStdString() << std::endl;
            reply->deleteLater();
            return;
        }
        //std::cout << doc.toJson().toStdString();
        http::handle_result(doc.object(), model);
        reply->deleteLater();
    });

}

void http::get(QUrl url ,Model model)
{
    url.setScheme("http");
    url.setHost(ip);
    url.setPort(port);
    //std::cout << url.toString().toStdString() << std::endl;
    // 创建网络请求
    QNetworkRequest request(url);
    // 发送 GET 请求
    QNetworkReply *reply = _manager.get(request);
    QObject::connect(reply, &QNetworkReply::finished, [=]() {
        if(reply->error() != QNetworkReply::NoError){

            reply->deleteLater();
            std::cout << reply->errorString().toStdString() << std::endl;
            std::cout<< "connect failed"<<std::endl;
            return;
        }
        //std::cout<< "服务器连接成功"<<std::endl;
        reply->deleteLater();
    });
}
void http::handle_result(QJsonObject json, Model model)
{
    if (model == Model::register_model) {
        //std::cout << QJsonDocument(json).toJson().toStdString();
        http_register_finish(json);
    }
    if (model == Model::logic_model) {
        //std::cout << QJsonDocument(json).toJson().toStdString();
        http_login_finish(json);
    }
    if (model == Model::friend_model) {
        //std::cout << QJsonDocument(json).toJson().toStdString();
        http_friend_finish(json);
    }
}
http::http(){

    // 读取服务器配置
    {
        QFile file("config.json");

        // 检查文件是否存在并成功打开
        if (!file.open(QIODevice::ReadOnly)) {
            std::cout << "Error: Could not open the file config.json!";
            return;
        }

        // 读取数据
        QByteArray data = file.readAll();
        file.close();
        //qDebug() << data;

        // 解析 JSON
        QJsonParseError error;
        QJsonDocument doc = QJsonDocument::fromJson(data, &error);
        if (doc.isNull()) {
            std::cout << "Error: Invalid JSON format!" ;
            return;
        }

        // 获取顶层 JSON 对象
        QJsonObject jsonObj = doc.object();

        // 检查是否包含 "server" 字段
        if (!jsonObj.contains("server") || !jsonObj["server"].isObject()) {
            std::cout << "Error: 'server' object not found or invalid!";
            return;
        }

        // 获取 "server" 对象
        QJsonObject serverObj = jsonObj["server"].toObject();

        // 提取 IP 地址
        if (!serverObj.contains("ip") || !serverObj["ip"].isString()) {
            std::cout << "Error: 'ip' field missing or invalid!";
            return;
        }
        ip = serverObj["ip"].toString();

        // 提取端口号
        if (!serverObj.contains("port") || !serverObj["port"].isDouble()) {
            std::cout << "Error: 'port' field missing or invalid!";
            return;
        }
        port = serverObj["port"].toInt();
    }






}
