#pragma once

#include <thread>
#include "SingleClass.h"
#include <queue>
#include <mutex>
#include <functional>
#include <vector>
#include <condition_variable>
#include <future>
#include <tuple>
#include "global.h"
class thread_pool : public single_class<thread_pool>
{
	friend class single_class<thread_pool>;
public:
	~thread_pool();
	template< class F ,class... Args>
	void submit_task( F&& f, Args&&... args) {
		if (is_stop) {
			throw std::runtime_error("Thread pool is stopped.");
		}
		std::unique_lock<std::mutex> lock(mtx);
		//auto task = std::function<F()>(std::bind(std::forward<F>(f), std::forward<Args>(args)...));
		
		// 1 �������
		// ����ʹ�� std::forward_as_tuple  ��Ϊ���ǲ��ɸ��Ƶ� ���� std::function Ҫ��ɸ���
		auto task = [f,args = std::make_tuple(std::forward<Args>(args)...)]() mutable {

			std::apply([f](auto&&... a) {
				auto return_data = std::invoke(std::forward<Args>(a)...);
				//std::cout << std::is_same<decltype(return_data), std::pair<uint32_t, boost::json::object >>::value << std::endl;
				//std::cout << return_data.second["error_code"] << std::endl;
				f(std::move(return_data));
				}, std::move(args));

			
		};
		//2  ���Դ�����ֵ ���ú���������ֵ ֵ����
		/*auto task = [f = std::forward<F>(f),
			args = std::make_tuple(std::forward<Args>(args)...)]() mutable {
			std::apply([&](auto&&... a) {
				std::invoke(f, std::forward<decltype(a)>(a)...);
				}, args);

		};*/
		// 3 ���Դ�����ֵ ���ú���������ֵ
		// ��ȷʵ������ת�� ���ǲ�֪��Ϊʲô ��������ʵû��ʵ������ת�� ������Ϊ��ֵ���ݸ���ֵҲû�£�����ֵ�Ͳ��ܴ��ݸ���ֵ���� ��������ˣ���
		// 
		// 
		
		//  ����Ϊ֮��˼��������
		//  ����std::move ʵ��������ת��
		/*int main() {
		int x = 2;
		int y = 3;
		auto _a = std::forward_as_tuple(x,8);
		std::apply([](auto&& a,auto&& b) {
			std::cout << std::is_same<decltype(a), int&>::value << std::endl;
			std::cout << std::is_same<decltype(b), int&&>::value << std::endl;
			},std::move(_a));
		//std::cout << std::is_same<decltype(1), int>::value << std::endl;

		return 0;
		}*/


		//std::move()  һ������ ��������д���� ��ֵ���� ��󴫵ݻ�����ֵ  �������ֵ���� ��󴫵ݻ�����ֵ
		/*int x = 1;
		auto _a = std::forward_as_tuple(x, 8);
    
		int a = 3;
		int& b = a;
		int&& c = std::move(b);
		int&& d = 4;
		std::cout << std::is_same<decltype(std::get<1>(_a)), int&>::value << std::endl;

    
		std::cout << std::is_same<decltype(d), int&&>::value << std::endl;*/


		/*auto task = [func = std::forward<F>(f),
			args_tuple = std::make_tuple(std::forward<Args>(args)...)]() mutable {
			std::apply([&](auto&&... args) {
				std::invoke(func, std::forward<decltype(args)>(args)...);
				}, std::move(args_tuple));
			};*/

		// 4




		task_queue.emplace(task);
		cv.notify_one();
			
	}

	private:
		bool is_stop = false;
		std::condition_variable cv;
		std::mutex mtx;
		thread_pool();
		std::queue<std::function<void()>> task_queue;
		std::vector<std::thread> thread_vector;
	
};