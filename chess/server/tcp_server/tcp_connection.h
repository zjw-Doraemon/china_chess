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
	// ������ÿ������ �����Լ����߳� ������̱߳�ը �ᵼ�� ��Դ����

	//std::condition_variable cv; // ���Ͷ��е���������
	//std::mutex mtx; // ���Ͷ��е���
	//bool is_stop = false; // �����߳�ֹͣ��־
	//std::thread send_thread; // �����߳�
	//void send_data(); // �����̺߳���


	//std::condition_variable cv_recieve; // ���ն��е���������
	//std::mutex recieve_mtx; // ���ն��е���
	//std::thread recieve_handle_thread; // ���մ����߳�
	//bool recieve_is_stop = false; // ���մ����߳�ֹͣ��־
	//uint32_t recieve_num_order = 1; // Ϊ������
	//void handle_msg(); �����̺߳���


	// �߳�ִ���� ����ص�����
	//void call_back(std::pair < uint32_t, boost::json::object>&& json);
	//std::unordered_map<uint32_t, boost::json::object> handle_result;

	//std::queue<boost::json::object> recieve_queue; // ���ն���
	//std::queue<boost::json::object> send_queue; // ���Ͷ���


	// �첽��������
	std::mutex map_mtx; // ������ʱ�洢��������Щ���󲢲��Ǹ÷��͵����ֻ�Ǵ������ѣ�����
	std::unordered_map<uint32_t, boost::json::object> store_map;
	std::atomic<uint64_t> send_queue_num{ 1 }; // ���ͽ�Ҫ���͵Ļذ����
	std::atomic<uint64_t> recieve_queue_num{ 1 };// ���յİ���Ϊ���ı��
	void child_CallBack(std::pair<uint32_t, boost::json::object>&& json); //���߳���ִ�еĻص�����
	void CallBack(std::pair<uint32_t, boost::json::object>&& json);// ioc �첽ִ�еĻص�����




	void Close();
	void read_head();
	void read_body(uint32_t length);
	
	boost::asio::ip::tcp::socket _socket;
	uint8_t head_size = 4;  // ����ͷ ��С

	// ���ջ����� ������Ϊ�ֲ����� ��Ϊ�첽����ʱ���ֲ������Ѿ�����
	char _data[2048]{ 0 };
};


// �߼� �����߳�ֻ���𽫽��յ������ݴ��뵽�����ն����У����ն��д����߳�ֻ�������ݰ�˳�򽻸��̳߳ص��̴߳���
// ��˳�򽻸������̣߳� �����߳�ֻ����ӷ����߳���ȡ������Ȼ����
// ���ϱ����� ��Ϊ ÿ�����Ӷ��������̣߳��������Դ���ѡ��̱߳�ը��