#ifndef GLOBAL_H
#define GLOBAL_H
#include <functional>
#include <QMessageBox>
// 定义棋子类型枚举
enum class PieceType { King, Advisor, Elephant, Horse, Chariot, Cannon, Soldier };
// 定义棋子颜色枚举，包含红色和黑色两种颜色
enum class Color { RED, BLACK };
// 定义棋子的选中状态枚举，包含已选中和未选中两种状态
enum class SelectState { SELECTED, UNSELECTED };
// 定义位置坐标结构体，使用 x 和 y 表示二维坐标
struct Position { int x, y; };

enum class Model {friend_model = 0,register_model = 1,logic_model = 2};

//解析json 数据
#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
// http
#include "QNetworkAccessManager"
#include <QNetworkRequest>
#include <QNetworkReply>


enum class ErrorCodes {
	success = 0,
	register_failed = 1001, // 注册错误
	send_varify_failed = 1002, // 发送验证码错误
	varify_faild = 1003,
};


// Defer类
class Defer {
public:
	// 接受一个lambda表达式或者函数指针
	Defer(std::function<void()> func) : func_(func) {}

	// 析构函数中执行传入的函数
	~Defer() {
		func_();
	}

private:
	std::function<void()> func_;
};

// 局部静态变量方式 单例模式模板类

template<typename T>
class single_class  {
public:
    static T& get_instance() {
        static T instance;
        return instance;
    }
protected: // 这里为什么使用 protected 而不是 private 是因为 子类想要继承父类时需要调用父类的构造函数
    single_class() = default;
    single_class(const single_class&) = delete;
    single_class& operator= (const single_class&) = delete;
};




#endif // GLOBAL_H
