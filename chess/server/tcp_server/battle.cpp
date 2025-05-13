#include "battle.h"
#include "battle.h"
#include "battle.h"
#include "battle.h"


chess_game::chess_game()
{
}

// ���캯������
chess_game::chess_game(uint32_t id, const std::string& from, const std::string& to,
    std::shared_ptr<tcp_connection> fromCon, std::shared_ptr<tcp_connection> toCon)
    : id_game(id), from_user(from), to_user(to), from_user_con(fromCon), to_user_con(toCon) {
}

// ��ȡ id_game �ķ�������
uint16_t chess_game::get_id_game() const {
    return id_game;
}

// ���� id_game �ķ�������
void chess_game::set_id_game(uint32_t id) {
    id_game = id;
}

// ��ȡ from_user �ķ�������
const std::string& chess_game::get_from_user() const {
    return from_user;
}

// ���� from_user �ķ�������
void chess_game::set_from_user(const std::string& from) {
    from_user = from;
}

// ��ȡ to_user �ķ�������
const std::string& chess_game::get_to_user() const {
    return to_user;
}

// ���� to_user �ķ�������
void chess_game::set_to_user(const std::string& to) {
    to_user = to;
}

// ��ȡ from_user_con �ķ�������
std::shared_ptr<tcp_connection> chess_game::get_from_user_con() const {
    return from_user_con;
}

// ���� from_user_con �ķ�������
void chess_game::set_from_user_con(const std::shared_ptr<tcp_connection>& fromCon) {
    from_user_con = fromCon;
}

// ��ȡ to_user_con �ķ�������
std::shared_ptr<tcp_connection> chess_game::get_to_user_con() const {
    return to_user_con;
}

// ���� to_user_con �ķ�������
void chess_game::set_to_user_con(const std::shared_ptr<tcp_connection>& toCon) {
    to_user_con = toCon;
}

std::shared_ptr<tcp_connection> chess_game::get_con(std::string name)
{
    if (name == to_user) {
        return from_user_con;
    }
    if (name == from_user) {
        return to_user_con;
    }
    return nullptr;
}

std::string chess_game::get_oppo_name(std::string name)
{
    if (name == to_user) {
        return from_user;
    }
    if (name == from_user) {
        return to_user;
    }
    return "";
}


void battleMgr::add_game(const std::string& name, const chess_game game_info)
{
    std::unique_lock<std::shared_mutex> mtx(rw_mtx);
    game_mgr[name] = game_info;
}

std::shared_ptr<tcp_connection> battleMgr::get_opponent(const std::string& name)
{
    std::shared_lock<std::shared_mutex> mtx(rw_mtx);
    auto game = game_mgr.find(name);
    if (game == game_mgr.end()) {
        return nullptr;
    }
    return game->second.get_con(name);


}

std::string battleMgr::get_opponent_name(const std::string& name)
{
    std::shared_lock<std::shared_mutex> mtx(rw_mtx);
    auto game = game_mgr.find(name);
    if (game == game_mgr.end()) {
        return "";
    }
    return game->second.get_oppo_name(name);
}

uint32_t battleMgr::get_num_game()
{
    return num_game;
}
