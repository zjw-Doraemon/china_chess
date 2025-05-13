#include "global.h"
defer::defer(defer_function end) :end(end) {

}
defer::~defer() {
    end();
    
}


std::string get_now() {
    // 设置时区为中国上海（代表中国时区）
    setenv("TZ", "Asia/Shanghai", 1);
    tzset();
    auto now = std::chrono::system_clock::now(); // 获取当前时间点
    std::time_t now_c = std::chrono::system_clock::to_time_t(now);
    tm timeinfo;
    // 使用 localtime_r 替代 localtime
    const tm* result = localtime_r(&now_c, &timeinfo);
    if (result == nullptr) {
        // 处理错误情况
        return "";
    }
    std::ostringstream oss;
    oss << std::put_time(result, "%Y-%m-%d %H:%M:%S");
    return oss.str();
}
