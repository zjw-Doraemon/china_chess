#pragma once
#include <iostream>
#include "SingleClass.h"
#include "tcp_connection.h"
#include <shared_mutex>
#include "user.h"
class chess_game {
public:
    chess_game();
    // ���캯������
    chess_game(uint32_t id, const std::string& from, const std::string& to,
        std::shared_ptr<tcp_connection> fromCon, std::shared_ptr<tcp_connection> toCon);

    // ��ȡ������ id_game �ķ�������
    uint16_t get_id_game() const;
    void set_id_game(uint32_t id);

    // ��ȡ������ from_user �ķ�������
    const std::string& get_from_user() const;
    void set_from_user(const std::string& from);

    // ��ȡ������ to_user �ķ�������
    const std::string& get_to_user() const;
    void set_to_user(const std::string& to);

    // ��ȡ������ from_user_con �ķ�������
    std::shared_ptr<tcp_connection> get_from_user_con() const;
    void set_from_user_con(const std::shared_ptr<tcp_connection>& fromCon);

    // ��ȡ������ to_user_con �ķ�������
    std::shared_ptr<tcp_connection> get_to_user_con() const;
    void set_to_user_con(const std::shared_ptr<tcp_connection>& toCon);

    std::shared_ptr<tcp_connection> get_con(std::string name); // ��ȡ���ֵ�����ָ��
    std::string get_oppo_name(std::string name);

private:
    uint32_t id_game;
    std::string from_user;
    std::string to_user;
    std::shared_ptr<tcp_connection> from_user_con;
    std::shared_ptr<tcp_connection> to_user_con;
};

// Ҫ��ֹ�����ڴ�й© �Ծֽ�����ʱɾ���Ծ�
class battleMgr : public single_class<battleMgr>{
public:
	friend class single_class<battleMgr>;
	void add_game(const std::string& name , const chess_game game_info);
    std::shared_ptr<tcp_connection> get_opponent(const std::string& name); // ��ȡ����ָ��
    std::string get_opponent_name(const std::string& name);
    uint32_t get_num_game();  //��ȡ�Ծ���
private:
    uint32_t num_game = 0;
    std::shared_mutex rw_mtx;
	std::unordered_map<std::string, chess_game> game_mgr;
};