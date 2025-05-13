#pragma once
#include <iostream>
#include <memory>
#include <mutex>
// 智能指针方式 单例模式模板类
template<typename T>
class SingleClass {
public:
	// static 代表是静态函数
	static std::shared_ptr<T> GetInstance() {
		static std::once_flag s_flag;
		std::call_once(s_flag, [&]() {
			_instance = std::shared_ptr<T>(new T);
			});

		return _instance;
	}
protected:
	SingleClass() = default;
	SingleClass(const SingleClass&) = delete;
	SingleClass& operator= (const SingleClass&) = delete;
	static std::shared_ptr<T> _instance;
};

// 静态成员变量的初始化
template <typename T>
std::shared_ptr<T> SingleClass<T>::_instance = nullptr;

// 局部静态变量方式 单例模式模板类

template<typename T>
class single_class {
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