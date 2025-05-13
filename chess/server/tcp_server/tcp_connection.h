#pragma once
#include "global.h"
#include "queue"
#include <unordered_map>

class tcp_connection : public std::enable_shared_from_this<tcp_connection> {
public:
	tcp_connection(boost::asio::io_context& ioc);
	~tcp_connection();
	boost::asio::ip::tcp::socket& get_socket();
	void Start();
	void send_data(boost::json::object data);
	
	
	
private:
	// 好像不能每个连接 都有自己的线程 会造成线程爆炸 会导致 资源不足

	//std::condition_variable cv; // 发送队列的条件变量
	//std::mutex mtx; // 发送队列的锁
	//bool is_stop = false; // 发送线程停止标志
	//std::thread send_thread; // 发送线程
	//void send_data(); // 发送线程函数


	//std::condition_variable cv_recieve; // 接收队列的条件变量
	//std::mutex recieve_mtx; // 接收队列的锁
	//std::thread recieve_handle_thread; // 接收处理线程
	//bool recieve_is_stop = false; // 接收处理线程停止标志
	//uint32_t recieve_num_order = 1; // 为请求编号
	//void handle_msg(); 处理线程函数


	// 线程执行完 结果回调函数
	//void call_back(std::pair < uint32_t, boost::json::object>&& json);
	//std::unordered_map<uint32_t, boost::json::object> handle_result;

	//std::queue<boost::json::object> recieve_queue; // 接收队列
	//std::queue<boost::json::object> send_queue; // 发送队列


	// 异步方法处理
	std::mutex map_mtx; // 访问临时存储的请求（这些请求并不是该发送的序号只是处理快而已）的锁
	std::unordered_map<uint32_t, boost::json::object> store_map;
	std::atomic<uint64_t> send_queue_num{ 1 }; // 发送将要发送的回包序号
	std::atomic<uint64_t> recieve_queue_num{ 1 };// 接收的包，为包的编号
	void child_CallBack(std::pair<uint32_t, boost::json::object>&& json); //子线程中执行的回调函数
	void CallBack(std::pair<uint32_t, boost::json::object>&& json);// ioc 异步执行的回调函数




	void Close();
	void read_head();
	void read_body(uint32_t length);
	
	boost::asio::ip::tcp::socket _socket;
	uint8_t head_size = 4;  // 数据头 大小

	// 接收缓冲区 不能作为局部变量 因为异步调用时，局部变量已经销毁
	char _data[2048]{ 0 };
};


// 逻辑 ：主线程只负责将接收到的数据存入到，接收队列中，接收队列处理线程只负责将数据按顺序交给线程池的线程处理，
// 按顺序交给发送线程， 发送线程只负责从发送线程中取出数据然后发送
// 以上被抛弃 因为 每个连接都绑定两个线程，会造成资源灾难。线程爆炸。