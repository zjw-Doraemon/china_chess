#include "battle.h"
#include "battle.h"
#include "battle.h"
#include "battle.h"


chess_game::chess_game()
{
}

// 构造函数定义
chess_game::chess_game(uint32_t id, const std::string& from, const std::string& to,
    std::shared_ptr<tcp_connection> fromCon, std::shared_ptr<tcp_connection> toCon)
    : id_game(id), from_user(from), to_user(to), from_user_con(fromCon), to_user_con(toCon) {
}

// 获取 id_game 的方法定义
uint16_t chess_game::get_id_game() const {
    return id_game;
}

// 设置 id_game 的方法定义
void chess_game::set_id_game(uint32_t id) {
    id_game = id;
}

// 获取 from_user 的方法定义
const std::string& chess_game::get_from_user() const {
    return from_user;
}

// 设置 from_user 的方法定义
void chess_game::set_from_user(const std::string& from) {
    from_user = from;
}

// 获取 to_user 的方法定义
const std::string& chess_game::get_to_user() const {
    return to_user;
}

// 设置 to_user 的方法定义
void chess_game::set_to_user(const std::string& to) {
    to_user = to;
}

// 获取 from_user_con 的方法定义
std::shared_ptr<tcp_connection> chess_game::get_from_user_con() const {
    return from_user_con;
}

// 设置 from_user_con 的方法定义
void chess_game::set_from_user_con(const std::shared_ptr<tcp_connection>& fromCon) {
    from_user_con = fromCon;
}

// 获取 to_user_con 的方法定义
std::shared_ptr<tcp_connection> chess_game::get_to_user_con() const {
    return to_user_con;
}

// 设置 to_user_con 的方法定义
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
