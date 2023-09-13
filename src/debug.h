#pragma once

#include <functional>

#undef LOG_DEBUG_ON
#define LOG_INFO_ON

#ifdef LOG_DEBUG_ON
void _my_log_impl(const char* format, ...);
#define LOG_DEBUG(format, ...) _my_log_impl(format, ##__VA_ARGS__)
#else
#define LOG_DEBUG(format, ...) 
#endif

#ifdef LOG_INFO_ON
void _my_log_impl(const char* format, ...);
#define LOG_INFO(format, ...) _my_log_impl(format, ##__VA_ARGS__)
#else
#define LOG_INFO(format, ...) 
#endif
#define PRINT 0
#define DEBUG 0
#define RCCHECK(fn) {rcl_ret_t temp_rc = fn; if ((temp_rc != RCL_RET_OK)){error_loop();}}

namespace hexapod {
    void initLogOutput(std::function<void(const char*)> writer, std::function<int(void)> time_func);
}
