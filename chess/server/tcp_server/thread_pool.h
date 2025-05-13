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
		
		// 1 编译错误
		// 不可使用 std::forward_as_tuple  因为他是不可复制的 但是 std::function 要求可复制
		auto task = [f,args = std::make_tuple(std::forward<Args>(args)...)]() mutable {

			std::apply([f](auto&&... a) {
				auto return_data = std::invoke(std::forward<Args>(a)...);
				//std::cout << std::is_same<decltype(return_data), std::pair<uint32_t, boost::json::object >>::value << std::endl;
				//std::cout << return_data.second["error_code"] << std::endl;
				f(std::move(return_data));
				}, std::move(args));

			
		};
		//2  可以处理左值 调用函数传入左值 值传递
		/*auto task = [f = std::forward<F>(f),
			args = std::make_tuple(std::forward<Args>(args)...)]() mutable {
			std::apply([&](auto&&... a) {
				std::invoke(f, std::forward<decltype(a)>(a)...);
				}, args);

		};*/
		// 3 可以处理右值 调用函数传入右值
		// 正确实现完美转发 但是不知道为什么 ，好像其实没有实现完美转发 就是因为右值传递给左值也没事，但左值就不能传递给右值引用 （好像错了？）
		// 
		// 
		
		//  以下为之后思考的内容
		//  好像std::move 实现了完美转发
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


		//std::move()  一个容器 如果容器中存的是 左值引用 最后传递会是左值  如果是右值引用 最后传递会是右值
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