#include "logic.h"
#include "mysql.h"
#include "battle.h"
std::pair<uint32_t, boost::json::object > logic_system::handle_json
(std::pair<uint32_t, boost::json::object >&& json, std::shared_ptr<tcp_connection> con)
{
	std::string action = json.second["action"].as_string().data();
	//std::cout << action << std::endl;
	if (logic_map.find(action) == logic_map.end()) {
		std::cout << "action no find" << std::endl;
		boost::json::object data;
		data["error_code"] = (int)ErrorCode::no_find_request;
		return std::pair<uint32_t, boost::json::object >(json.first,data);
	}
	auto result_json = std::pair<uint32_t, boost::json::object >(json.first, logic_map[action](json.second,con));
	//std::cout << result_json.second["error_code"] << std::endl;
	return result_json;
}

logic_system::logic_system()
{
	init_logic_map();
}

void logic_system::init_logic_map()
{
	logic_map["login"] = [](boost::json::object& json, std::shared_ptr<tcp_connection> con)->boost::json::object {
		
		std::string email = json["email"].as_string().data();
		std::string password = json["pass_word"].as_string().data();
		bool success = mysql_mgr::get_instance().login(email,password);
		if (!success) {
			boost::json::object data;
			data["model"] = "login";
			data["error_code"] = (int)ErrorCode::login_faild;
			return data;
		}
		// Ϊ�û�����������û�
		UserInfo user = mysql_mgr::get_instance().get_user_info(email);
		user.set_connection(con);
		UserMgr::get_instance().add_user(user);

		std::cout << user.name  << " has login ." << std::endl;
		boost::json::object data;
		data["model"] = "login";
		data["name"] = user.name;
		data["error_code"] = (int)ErrorCode::success;
		return data;
		};
	logic_map["add_friend"] = [](boost::json::object& json, std::shared_ptr<tcp_connection> con)->boost::json::object {
		std::string name = json["add_friend_name"].as_string().data();
		std::string my_name = json["my_name"].as_string().data();
		UserInfo friend_add = UserMgr::get_instance().find_user(name);
		if (friend_add.is_effective == false) {
			boost::json::object data;
			data["action"] = "response_add_friend";
			data["model"] = "friend_model";
			data["error_code"] = (int)ErrorCode::add_friend_faild;
			return data;
		}
		//���� sql ��
		mysql_mgr::get_instance().apply_friend(my_name,name);
		// ���ݸ����ѵ���Ϣ
		boost::json::object data;
		data["model"] = "friend_model";
		data["action"] = "request_add_friend";
		data["from_friend_name"] = my_name;
		
		friend_add._connection->send_data(data);


		// ���سɹ����ͼӺ��������Ӧ��Ϣ
		boost::json::object data_me;
		data_me["model"] = "friend_model";
		data_me["action"] = "response_add_friend";
		data_me["error_code"] = (int)ErrorCode::success;
		return data_me;
		};

	logic_map["auth_friend"] = [](boost::json::object& json, std::shared_ptr<tcp_connection> con)->boost::json::object {
		std::string from_name = json["from_user"].as_string().data();
		std::string to_name = json["to_user"].as_string().data();
		std::string status = json["status"].as_string().data();
		// ���Һ����Ƿ�����
		UserInfo _friend = UserMgr::get_instance().find_user(to_name);
		if (_friend.is_effective == false) {
			boost::json::object data;
			data["action"] = "response_add_friend";
			data["model"] = "friend_model";
			data["error_code"] = (int)ErrorCode::friend_offline;
			return data;
		}

		// �������ݿ�
		mysql_mgr::get_instance().auth_apply_friend(status,from_name,to_name);

		boost::json::object data;
		data["model"] = "friend_model";
		data["action"] = "request_auth_friend";
		data["from_user"] = from_name;
		data["status"] = status == "accepted" ? "accepted":"reject";
		_friend._connection->send_data(data);
		// ���ػ�Ӧ
		boost::json::object data_me;
		data_me["model"] = "friend_model";
		data_me["action"] = "response_auth_friend";
		data_me["error_code"] = (int)ErrorCode::success;
		return data_me;

		
	
	};

	logic_map["invite_battle"] = [](boost::json::object& json, std::shared_ptr<tcp_connection> con)->boost::json::object {
		std::string from_name = json["from_user"].as_string().data();
		std::string to_name = json["to_user"].as_string().data();
		// ���Һ����Ƿ�����
		UserInfo _friend = UserMgr::get_instance().find_user(to_name);
		if (_friend.is_effective == false) {
			boost::json::object data;
			data["action"] = "request_battle_from_server";
			data["model"] = "friend_model";
			data["error_code"] = (int)ErrorCode::friend_offline;
			return data;
		}
		// ����ѷ�������
		
		boost::json::object data;
		data["model"] = "friend_model";
		data["action"] = "request_battle";
		data["from_user"] = from_name;
		_friend._connection->send_data(data);

		// ���ػ�Ӧ
		boost::json::object data_me;
		data_me["model"] = "friend_model";
		data_me["action"] = "request_battle_from_server";
		data_me["error_code"] = (int)ErrorCode::success;
		return data_me;


	};
	logic_map["auth_battle"] = [](boost::json::object& json, std::shared_ptr<tcp_connection> con)->boost::json::object {
		std::string from_name = json["from_user"].as_string().data();
		std::string to_name = json["to_user"].as_string().data();

		// ���Һ����Ƿ�����
		UserInfo _friend = UserMgr::get_instance().find_user(to_name);
		if (_friend.is_effective == false) {
			boost::json::object data;
			data["action"] = "response_battle_tcp_server";
			data["model"] = "friend_model";
			data["error_code"] = (int)ErrorCode::friend_offline;
			return data;
		}

		// ��ӶԾ���Ϣ
		chess_game game(battleMgr::get_instance().get_num_game(),from_name,to_name,con, _friend._connection);
		battleMgr::get_instance().add_game(from_name,game);
		battleMgr::get_instance().add_game(to_name, game);

		// ����ѷ�������
		boost::json::object data;
		data["model"] = "friend_model";
		data["action"] = "response_battle";
		data["status"] = json["status"];
		data["from_user"] = from_name;
		_friend._connection->send_data(data);

		// ����������ķ��ػ�Ӧ  // �������ػ�Ӧ ��Ϊ �����صĻ� ������Ӧ�޷����� �ͻ��˿���ѡ����Ӵ˻�Ӧ
		boost::json::object data_me;
		data_me["model"] = "friend_model";
		data_me["action"] = "response_battle_tcp_server";
		data_me["error_code"] = (int)ErrorCode::success;
		return data_me;


	};


	logic_map["move_piece"] = [](boost::json::object& json, std::shared_ptr<tcp_connection> con)->boost::json::object {
		std::string from_name = json["from_user"].as_string().data();

		boost::json::value piece_id = json["piece_id"];
		
		boost::json::value end_point = json["end_point"];
		
		std::shared_ptr<tcp_connection> oppo_con = battleMgr::get_instance().get_opponent(from_name);
		if (oppo_con == nullptr) {
			boost::json::object data;
			data["action"] = "response_battle_tcp_server";
			data["model"] = "friend_model";
			data["error_code"] = (int)ErrorCode::friend_offline;
			return data;
		}
		std::string to_name = battleMgr::get_instance().get_opponent_name(from_name);
		
		

		// ���Һ����Ƿ�����
		UserInfo _friend = UserMgr::get_instance().find_user(to_name);
		if (_friend.is_effective == false) {
			boost::json::object data;
			data["action"] = "response_battle_tcp_server";
			data["model"] = "friend_model";
			data["error_code"] = (int)ErrorCode::friend_offline;
			return data;
		}
		// ���͸�����������Ϣ
		boost::json::object data;
		data["action"] = "handle_oppo_move_info";
		data["model"] = "web_board_model";
		data["piece_id"] = piece_id;
		
		data["end_point"] = end_point;
		oppo_con->send_data(data);


			// ���ػ�Ӧ
		boost::json::object data_me;
		data_me["model"] = "web_board_model";
		data_me["action"] = "response_move_piece";
		data_me["error_code"] = (int)ErrorCode::success;
		return data_me;

	};
	logic_map["end_game"] = [](boost::json::object& json, std::shared_ptr<tcp_connection> con)->boost::json::object {
		std::string from_name = json["from_user"].as_string().data();
		std::shared_ptr<tcp_connection> oppo_con = battleMgr::get_instance().get_opponent(from_name);
		if (oppo_con == nullptr) {
			boost::json::object data;
			data["action"] = "response_battle_tcp_server";
			data["model"] = "friend_model";
			data["error_code"] = (int)ErrorCode::friend_offline;
			return data;
		}
		std::string to_name = battleMgr::get_instance().get_opponent_name(from_name);



		// ���Һ����Ƿ�����
		UserInfo _friend = UserMgr::get_instance().find_user(to_name);
		if (_friend.is_effective == false) {
			boost::json::object data;
			data["action"] = "response_battle_tcp_server";
			data["model"] = "friend_model";
			data["error_code"] = (int)ErrorCode::friend_offline;
			return data;
		}
		// ���͸�������Ϣ
		boost::json::object data;
		data["action"] = "notice_game_end";
		data["model"] = "web_board_model";
		oppo_con->send_data(data);
		// ����������ķ��ػ�Ӧ  // �������ػ�Ӧ ��Ϊ �����صĻ� ������Ӧ�޷����� �ͻ��˿���ѡ����Ӵ˻�Ӧ
		boost::json::object data_me;
		data_me["model"] = "web_board_model";
		data_me["action"] = "response_move_piece";
		data_me["error_code"] = (int)ErrorCode::success;
		return data_me;
		};
	logic_map["web_test"] = [](boost::json::object& json, std::shared_ptr<tcp_connection> con)->boost::json::object {
		// ����������ķ��ػ�Ӧ  // �������ػ�Ӧ ��Ϊ �����صĻ� ������Ӧ�޷����� �ͻ��˿���ѡ����Ӵ˻�Ӧ ��Ϊ�����߼���һ��������
		boost::json::object data_me;
		data_me["model"] = "welcome_model";
		
		return data_me;

		};

}