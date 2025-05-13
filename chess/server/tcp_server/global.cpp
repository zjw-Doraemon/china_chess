#include "global.h"
defer::defer(defer_function end) :end(end) {

}
defer::~defer() {
    end();
    
}


std::string get_now() {
    // ����ʱ��Ϊ�й��Ϻ��������й�ʱ����
    setenv("TZ", "Asia/Shanghai", 1);
    tzset();
    auto now = std::chrono::system_clock::now(); // ��ȡ��ǰʱ���
    std::time_t now_c = std::chrono::system_clock::to_time_t(now);
    tm timeinfo;
    // ʹ�� localtime_r ��� localtime
    const tm* result = localtime_r(&now_c, &timeinfo);
    if (result == nullptr) {
        // ����������
        return "";
    }
    std::ostringstream oss;
    oss << std::put_time(result, "%Y-%m-%d %H:%M:%S");
    return oss.str();
}
