#include "AsioIOServicePool.h"

AsioIOServicePool::~AsioIOServicePool()
{
	stop();
	//std::cout << "ioc �¼�ѭ���̳߳��Ѿ�����" << std::endl;
}

AsioIOServicePool::AsioIOServicePool(uint8_t size ):ioc_size(size),ioc_thread_pool(size),works(size){
	for (int i = 0; i < size; i++) {
		ioc_thread_pool[i] = std::make_shared<boost::asio::io_context>();
		works[i] = std::unique_ptr<Work>(new Work(ioc_thread_pool[i]->get_executor()));
	}
	//�������ioservice����������̣߳�ÿ���߳��ڲ�����ioservice
	for (std::size_t i = 0; i < ioc_thread_pool.size(); ++i) {
		_threads.emplace_back([this, i]() {
			ioc_thread_pool[i]->run();
			});
	}
}

std::shared_ptr<boost::asio::io_context> AsioIOServicePool::GetIOService() {
	std::unique_lock<std::mutex> lock(mtx);
	auto service = ioc_thread_pool[_nextIOC++];
	if (_nextIOC == ioc_thread_pool.size()) {
		_nextIOC = 0;
	}
	return service;
}

void AsioIOServicePool::stop()
{
	for (int i = 0; i < works.size(); ++i) {
		works[i]->get_executor().context().stop();
		works[i] = nullptr;
	}
	for (auto& t : _threads) {
		t.join();
	}
}
