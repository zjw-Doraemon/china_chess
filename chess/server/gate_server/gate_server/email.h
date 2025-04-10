#include <iostream>
#include "SingleClass.h"
#include <Poco/Net/HTTPClientSession.h>  // ʾ��ͷ�ļ�
#include <Poco/Foundation.h>
#include "Poco/Net/MailMessage.h"
#include <Poco/Net/SecureSMTPClientSession.h>
class email : public single_class<email>
{
public:
    email();
    friend class single_class<email>;
    std::string send_email_verify_code(const std::string& Recipient );
private:
    std::string host; //�Լ��������ַ
    // ���Ӷ��󣨿ɸ��ã�
    std::unique_ptr<Poco::Net::SMTPClientSession> _session;
    

};