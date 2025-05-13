# json 格式规则


    tcp 服务器
                 json键的值的意义比较多变的 不同值代表不同意义

         action  他是客户端和服务端用来分辨具体的请求的信息 都是使用action。

以下是具体的请求信息

add_friend  客户端向服务端发送 添加好友请求

invite_battle 客户端向服务端发送 想要邀请某位好友对战的请求

response_add_friend  服务端向客户端发送 告诉客户端添加好友请求是否成功，操作是否成功。

request_add_friend   服务端向客户端发送  告诉客户端有人想要加你为好友

response_auth_friend  服务器向客户端发送  认证好友请求发送到服务器成功，这个操作成功

auth_friend  客户端向服务器发送 关于认证好友的请求

request_auth_friend  服务端向客户端发送 某个好友同意或拒绝加好友

request_battle  服务端向客户端发送 某个好友邀请对战的请求

response_battle  服务器向客户端发送 好友是否同意接受对战

response_battle_tcp_server  客户端向服务器发送的请求是否 成功  服务器回复的

request_battle_from_server  服务器向客户端回复 发送的请求是否成功

move_piece  客户端向服务端发送 走子信息

handle_oppo_move_info 服务器向客户端发送  对手客户端向他发送的走子信息

notice_game_end  服务器向客户端发送  通知对局结束 一般是自己老将被吃了


      model  他是服务器用来告诉客户端要将这个回复发送给哪个模块 因为请求具体处理交给了每个模块 

以下是具体的请求信息

enum class Model {friend_model = 0,register_model = 1,logic_model = 2};

friend_model 客户端关于朋友的模块

register_model 客户端关于注册的模块

logic_model 客户端关于登录的模块






    一些具体信息 json键的值的意义比较确定的 
添加好友的时候发送的

my_name  客户端告诉服务端自己的名字是

add_friend_name   客户端告诉服务端，我想要添加的好友的名字是

from_friend_name  服务器告诉客户端，来自哪个好友想要加你为好友
 
认证好友的时候发送的  优点乱以后记住要统一 一下

from_user 客户端向服务端发送 来自哪个用户的请求   或者是  服务端向客户端发送 来自哪个用户对你发送的信息

to_user  客户端向服务端发送  客户端想要向哪个用户发送

status  服务端向客户端发送 对方是否同意加好友 或客户端向服务端发送 我是否同意加好友