#include "LoggerStd.hpp"

int main()
{
    Logger::init_Global(new LoggerStd());

    LOG_DEBUG("This is a debug message.");
    LOG_INFO("This is a info message.");
    LOG_WARN("This is a warning message.");
    LOG_ERROR("This is a error message.");
    LOG_CRIT("This is a critical message.");
    LOG_FATAL("This is a fatal message.");

    Logger::destroy_Global();
    return 0;
}