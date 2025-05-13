# include "tcp_connection.h"
# include "logic.h"
# include "user.h"
# include "thread_pool.h"
# include <cstring>
# include <functional>
# include <thread>
# include <chrono>
tcp_connection::tcp_connection(boost::asio::io_context& ioc):_socket(ioc)
{
    // 启动发送线程
    //send_thread = std::thread(&tcp_connection::send_data,this); 
    // 启动处理接收队列的线程
    //recieve_handle_thread = std::thread(&tcp_connection::handle_msg, this);



}

tcp_connection::~tcp_connection()
{
    Close();
    //recieve_handle_thread.join();
    //send_thread.join();
    //std::cout << "connect close" << std::endl;
}

void tcp_connection::Close()
{
    _socket.close();
}




//void tcp_connection::send_data()
//{
//    //std::cout << "The function for processing the sending queue starts." << std::endl;
//    while (1) {
//        std::unique_lock<std::mutex> lock(mtx);
//        cv.wait(lock, [this]()->bool {
//            //std::cout << "send data awake one" << std::endl;
//            if (is_stop == true) {
//                return true;
//            }
//            if (send_queue.size() == 0) {
//                std::cout << "send queue is empty" << std::endl;
//                return false;
//            }
//            return true;
//            });
//
//        if (is_stop == true) {
//            return;
//        }
//        // 数据大小
//        std::string data = boost::json::serialize(send_queue.front());
//        uint32_t data_size = htonl(data.size()); // 转换字节序 ， 数据大小不太可能超过32 位的数字表示能力
//        try{
//            boost::asio::write(_socket, boost::asio::buffer((&data_size), sizeof(data_size)));
//            boost::asio::write(_socket, boost::asio::buffer(data));
//            send_queue.pop();
//        }
//        catch (...) {
//            std::cout << "send failed" << std::endl;
//        }
//       
//    }
//}
//
//
//void tcp_connection::handle_msg()
//{
//    //std::cout << "The function for processing the receiving queue begins. " << std::endl;
//    while (1) {
//        {
//            std::unique_lock<std::mutex> lock(recieve_mtx);
//            cv_recieve.wait(lock, [this]()->bool {
//                //std::cout << "handle data awake one" << std::endl;
//                if (recieve_is_stop == true) {
//                    return true;
//                }
//                if (recieve_queue.size() == 0) {
//                    //std::cout << "queue is empty" << std::endl;
//                    return false;
//                }
//                return true;
//                });
//
//            if (recieve_is_stop == true) {
//                return;
//            }
//            
//            std::function<void(std::pair<uint32_t,boost::json::object&&>)> call_back_function(
//                [_self = shared_from_this()](std::pair <uint32_t,boost::json::object>&& json) {
//                    _self->call_back(std::move(json));
//                });
//                
//            std::cout << "task begin" << std::endl;
//            thread_pool::get_instance().submit_task
//            (   call_back_function,
//                &logic_system::handle_json, 
//                logic_system::GetInstance(),
//                std::pair<uint32_t, boost::json::object >(recieve_num_order,recieve_queue.front()),
//                shared_from_this()
//            );
//
//          
//            recieve_queue.pop();
//           
//        }
//
//
//    }
//}
//
//void tcp_connection::call_back(std::pair < uint32_t, boost::json::object>&& json)
//{
//    if (json.first == recieve_num_order ) {
//        std::unique_lock<std::mutex> lok(mtx);
//        send_queue.push(std::move(json.second));
//        ++recieve_num_order;
//        cv.notify_one();
//        return;
//    }
//    else {
//        handle_result.emplace(json);
//        // 看看备用结果有没有接下来的包
//        auto find_result = handle_result.find(recieve_num_order);
//        while (find_result != handle_result.end()) {
//            send_queue.push(std::move(find_result->second));
//            handle_result.erase(find_result);
//            ++recieve_num_order;
//        }
//    }
//
//
//}
//





boost::asio::ip::tcp::socket& tcp_connection::get_socket()
{
	return _socket;
}

void tcp_connection::Start()
{
    read_head();
    /*while (1) {
        std::this_thread::sleep_for(std::chrono::seconds(5));
        std::cout << "hello world" << std::endl;
    }*/
}



void tcp_connection::read_head()
{
    auto _self = shared_from_this();
    
	boost::asio::async_read(_socket, boost::asio::buffer(_data), boost::asio::transfer_exactly(head_size),
		[_self](const boost::system::error_code& ec, int num) {
            try {
                if (ec ) {
                    //std::cout << "handle read failed, error is " << ec.what() << std::endl;
                    _self->Close();
                    
                    UserMgr::get_instance().delete_user_from_connection(_self);
                    return;
                }
                //std::cout << _data << std::endl;
                uint32_t length_body = 0;
                // 把 _data 所指向的前 4 个字节复制到 length_body 中
                std::memcpy(&length_body, _self->_data, sizeof(length_body));
                // 把网络字节序转换为本地字节序
                length_body = ntohl(length_body);
                //std::cout << length_body << std::endl;
                _self->read_body(length_body);
                
              
            }
            catch (std::exception& e) {
                std::cout << "Exception code is " << e.what() << std::endl;
            }
        });
}


void tcp_connection::read_body(uint32_t length)
{
    auto _self = shared_from_this();
    
    boost::asio::async_read(_socket, boost::asio::buffer(_data), boost::asio::transfer_exactly(length),
        [=](const boost::system::error_code& ec, int num) {

            try {
                if (ec) {
                    //std::cout << "body read failed, error is " << ec.what() << std::endl;
                    _self->Close();
                    UserMgr::get_instance().delete_user_from_connection(_self);
                    
                    return;
                }
                //std::cout << _self.use_count() << std::endl;
                
                
                // 连接类 自带线程 处理任务方法 废弃
                /*{
                    std::unique_lock<std::mutex> mtx(recieve_mtx);
                    recieve_queue.emplace(json);
                    cv_recieve.notify_one();
                }*/
                // 异步注册到ioc 方法
                boost::json::object json = boost::json::parse(_data).as_object();
                std::function<void(std::pair<uint32_t, boost::json::object>&&)> child_callBack
                    ([self = shared_from_this()](std::pair<uint32_t, boost::json::object>&& data) {
                        //std::cout << data.second["error_code"]<< std::endl;
                        self->child_CallBack(std::move(data));
                    });
                thread_pool::get_instance().submit_task
                (   child_callBack,
                    &logic_system::handle_json,
                    logic_system::GetInstance(),
                    std::pair<uint32_t, boost::json::object >(recieve_queue_num, json),
                    shared_from_this()
                );
                recieve_queue_num++; // 下一个接收的包的序号加一

                //std::cout << json["action"] << std::endl;
                memset(_data,0,sizeof(_data));
                read_head();
                
            }
            catch (std::exception& e) {
                std::cout << "Exception code is " << e.what() << std::endl;
            }
        });
}



void tcp_connection::child_CallBack(std::pair<uint32_t, boost::json::object>&& data)
{
    boost::asio::post(_socket.get_executor(), [this,data = std::move(data)]() mutable {
        //std::cout << send_queue_num << std::endl;
        
        CallBack(std::move(data));
        });
}

void tcp_connection::CallBack(std::pair<uint32_t, boost::json::object>&& json)
{

    if (json.first == send_queue_num) {
        if (json.second.empty()) {
            std::cout << "json.second is empty!" << std::endl;
            return; // 或者采取其他处理措施
        }
        send_data(json.second);
        //std::cout << "send data" << std::endl;
        ++send_queue_num; //下次发送的包的序号加一



        auto next_package = store_map.find(send_queue_num);
        while (next_package != store_map.end()) {
            send_data(next_package->second);
            ++send_queue_num; //下次发送的包的序号加一

        }
    }
    else {
        store_map.emplace(std::move(json));
    }
    //send_data(json.second);
    
    
}


void tcp_connection::send_data(boost::json::object json_data)
{
    std::cout << UserMgr::get_instance().get_name_from_con(shared_from_this()) + " " + boost::json::serialize(json_data) << std::endl ;
    // 数据大小 发数据头
    std::string data = boost::json::serialize(json_data);
    uint32_t data_size = htonl(data.size()); // 转换字节序 ， 数据大小不太可能超过32 位的数字表示能力

    boost::asio::async_write(_socket, boost::asio::buffer((&data_size), sizeof(data_size)), []
    (boost::system::error_code ec, std::size_t bytes_sent) {
            if (ec) {
                std::cout << "body read failed, error is " << ec.what() << std::endl;
            }

        });
    std::string body_data = boost::json::serialize(json_data);
    boost::asio::async_write(_socket, boost::asio::buffer(body_data), []
    (boost::system::error_code ec, std::size_t bytes_sent) {
            if (ec) {
                std::cout << "body read failed, error is " << ec.what() << std::endl;
            }

        });
    
}
