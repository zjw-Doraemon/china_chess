
#include "thread_pool.h"

thread_pool::thread_pool() {
	uint8_t core_num = std::thread::hardware_concurrency();
	for (int i = 0; i < core_num; ++i) {
		thread_vector.emplace_back([this] {
			
			while (1) {
				std::function<void()> tmp_function;
				{
					std::unique_lock<std::mutex> lok(mtx);
					cv.wait(lok, [this] {
						if (is_stop == true) {
							return true;
						}
						if (task_queue.size() == 0) {
							return false;
						}
						return true;
						});
					if (is_stop == true) {
						return;
					}
					tmp_function = task_queue.front();
					task_queue.pop();
				}
				tmp_function();
			}
			});
	}
}
thread_pool::~thread_pool() {
	is_stop = true;
	for (int i = 0; i < thread_vector.size(); ++i) {
		thread_vector[i].join();
	}
}