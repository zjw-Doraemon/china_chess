#include "email.h"

// �����ļ� ini
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/ini_parser.hpp>

// poco
#include "Poco/UUID.h"
#include "Poco/UUIDGenerator.h"
#include "Poco/Net/NetException.h"
email::email()
{
	//��ʼ�� host
	boost::property_tree::ptree pt;
	boost::property_tree::ini_parser::read_ini("config.ini", pt);
	host = pt.get<std::string>("Email.Host");

}

std::string email::send_email_verify_code(const std::string& Recipient)
{
	// ���� UUID ������
	Poco::UUIDGenerator generator;
	// ����һ������� UUID
	Poco::UUID uuid = generator.createRandom();
	// �� UUID ת��Ϊ�ַ�����ʽ
	std::string verificationCode = uuid.toString();
	// �ض�
	verificationCode = verificationCode.substr(0, 4);
	try {
		// ���ӷ�����
		boost::property_tree::ptree pt;
		boost::property_tree::ini_parser::read_ini("config.ini", pt);
		std::string auth_pass = pt.get<std::string>("Email.auth_code");
		std::string my_email = pt.get<std::string>("Email.my_email");
		_session = std::make_unique<Poco::Net::SMTPClientSession>(host, 25);
		_session->login(Poco::Net::SMTPClientSession::LoginMethod::AUTH_LOGIN, my_email, auth_pass);
		
	
		Poco::Net::MailMessage message;
		// ���÷�����
		message.setSender(my_email);
		// �����ռ���
		message.addRecipient(Poco::Net::MailRecipient(Poco::Net::MailRecipient::PRIMARY_RECIPIENT, Recipient));
		// �����ʼ�����
		message.setSubject("code Email");
		
		std::string content = "�����֤���� " + verificationCode + " 5����֮����Ч ";
		// �����ʼ���������
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
