/**
 * @file Logger.cpp
 * @author Manuel Nadji (https://github.com/gnarrf95)
 * 
 * @brief Implementation of global logger interface.
 * 
 * @date 2021-02-09
 * 
 */

#include "Logger.hpp"

#include <cstdio>
#include <cstdarg>
#include <cstring>

/**
 * @brief Initialize global instance with @c NULL .
 * 
 */
Logger *Logger::global_instance = nullptr;

//----------------------------------------------------------------------------------------------------------------------
//
Logger *Logger::init_Global(Logger *new_logger) {
    Logger::global_instance = new_logger;
}

//----------------------------------------------------------------------------------------------------------------------
//
Logger *Logger::get_Global() {
    return Logger::global_instance;
}

//----------------------------------------------------------------------------------------------------------------------
//
void Logger::destroy_Global() {
    delete Logger::global_instance;
    Logger::global_instance = nullptr;
}

//----------------------------------------------------------------------------------------------------------------------
//
void Logger::log_global(logType_t log_type, const char *fmt, ...) {
    va_list args;
    char buf[2048];

    Logger *g_logger = Logger::get_Global();
    if(g_logger)
    {
        va_start(args, fmt);
        vsnprintf(buf, sizeof(buf), fmt, args);
        g_logger->log(log_type, buf);
        va_end(args);
    }
}

//----------------------------------------------------------------------------------------------------------------------
//
void Logger::log_global_sc(
    logType_t log_type,
    const char *file,
    const char *function,
    int line,
    const char *fmt, ...
) {
    va_list args;
    char buf[2048];

    Logger *g_logger = Logger::get_Global();
    if(g_logger)
    {
        va_start(args, fmt);
        vsnprintf(buf, sizeof(buf), fmt, args);
        g_logger->log_sc(log_type, file, function, line, buf);
        va_end(args);
    }
}