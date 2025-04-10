#include "email.h"

// 配置文件 ini
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/ini_parser.hpp>

// poco
#include "Poco/UUID.h"
#include "Poco/UUIDGenerator.h"
#include "Poco/Net/NetException.h"
email::email()
{
	//初始化 host
	boost::property_tree::ptree pt;
	boost::property_tree::ini_parser::read_ini("config.ini", pt);
	host = pt.get<std::string>("Email.Host");

}

std::string email::send_email_verify_code(const std::string& Recipient)
{
	// 创建 UUID 生成器
	Poco::UUIDGenerator generator;
	// 生成一个随机的 UUID
	Poco::UUID uuid = generator.createRandom();
	// 将 UUID 转换为字符串形式
	std::string verificationCode = uuid.toString();
	// 截断
	verificationCode = verificationCode.substr(0, 4);
	try {
		// 连接服务器
		boost::property_tree::ptree pt;
		boost::property_tree::ini_parser::read_ini("config.ini", pt);
		std::string auth_pass = pt.get<std::string>("Email.auth_code");
		std::string my_email = pt.get<std::string>("Email.my_email");
		_session = std::make_unique<Poco::Net::SMTPClientSession>(host, 25);
		_session->login(Poco::Net::SMTPClientSession::LoginMethod::AUTH_LOGIN, my_email, auth_pass);
		
	
		Poco::Net::MailMessage message;
		// 设置发件人
		message.setSender(my_email);
		// 设置收件人
		message.addRecipient(Poco::Net::MailRecipient(Poco::Net::MailRecipient::PRIMARY_RECIPIENT, Recipient));
		// 设置邮件主题
		message.setSubject("code Email");
		
		std::string content = "你的验证码是 " + verificationCode + " 5分钟之内有效 ";
		// 设置邮件正文内容
		message.setContent(content);
		_session->sendMessage(message);
		_session->close();
	}
	catch (const Poco::Net::NetException& e) {
		std::cerr << "Network error: " << e.displayText() << std::endl;
		std::cerr << "Network error: " << e.code() << std::endl;
		
		if (_session != nullptr) {
			_session->close();
		}
		
		return "";
	}

	return verificationCode;
}
