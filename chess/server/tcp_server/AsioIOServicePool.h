#include "global.h"
#include "thread"
#include "SingleClass.h"
using Work = boost::asio::executor_work_guard<boost::asio::io_context::executor_type>;
class AsioIOServicePool : public single_class<AsioIOServicePool>
{
public:
	
	friend single_class<AsioIOServicePool>;
	std::shared_ptr<boost::asio::io_context> GetIOService();
	void stop();
	
private:
	std::mutex mtx;
	~AsioIOServicePool();  // 析构函数设为私有就不能被智能指针管理了
	// std::thread::hardware_concurrency() 最合适的线程数
	AsioIOServicePool(uint8_t size = 3);
	uint8_t ioc_size;
	std::vector<std::thread> _threads; 
	std::vector<std::shared_ptr<boost::asio::io_context>> ioc_thread_pool;
	std::vector<std::unique_ptr<Work>> works;

	uint8_t _nextIOC = 0; // 每次获取ioc都会返回下一个保证负载均衡  会有线程安全问题

};