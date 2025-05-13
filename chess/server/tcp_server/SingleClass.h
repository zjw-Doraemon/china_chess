#pragma once
#include <iostream>
#include <memory>
#include <mutex>
// ����ָ�뷽ʽ ����ģʽģ����
template<typename T>
class SingleClass {
public:
	// static �����Ǿ�̬����
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

// ��̬��Ա�����ĳ�ʼ��
template <typename T>
std::shared_ptr<T> SingleClass<T>::_instance = nullptr;

// �ֲ���̬������ʽ ����ģʽģ����

template<typename T>
class single_class {
public:
	static T& get_instance() {
		static T instance;
		return instance;
	}
protected: // ����Ϊʲôʹ�� protected ������ private ����Ϊ ������Ҫ�̳и���ʱ��Ҫ���ø���Ĺ��캯��
	single_class() = default;
	single_class(const single_class&) = delete;
	single_class& operator= (const single_class&) = delete;
};