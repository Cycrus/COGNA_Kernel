/**
 * @file LoggerStd.hpp
 * @author Manuel Nadji (https://github.com/gnarrf95)
 * 
 * @brief Implementation of LoggerStd.
 * 
 * @date 2021-02-09
 * 
 */

#include "LoggerStd.hpp"

#include <iostream>

/**
 * @brief Definitions of log type representations.
 * 
 */
static const char *LOGSTRING_DEBUG = "=DBG=";
static const char *LOGSTRING_INFO = "=INF=";
static const char *LOGSTRING_WARNING = "=WRN=";
static const char *LOGSTRING_ERROR = "=ERR=";
static const char *LOGSTRING_CRITICAL = "*CRT*";
static const char *LOGSTRING_FATAL = "**FATAL**";

//----------------------------------------------------------------------------------------------------------------------
//
LoggerStd::LoggerStd() {
}

//----------------------------------------------------------------------------------------------------------------------
//
LoggerStd::~LoggerStd() {
}

//----------------------------------------------------------------------------------------------------------------------
//
void LoggerStd::log(logType_t log_type, const char *msg) {
    std::cout << "[ ";

    switch(log_type)
    {
        case logType_t::DEBUG:
        {
            std::cout << LOGSTRING_DEBUG;
            break;
        }

        case logType_t::INFO:
        {
            std::cout << LOGSTRING_INFO;
            break;
        }

        case logType_t::WARN:
        {
            std::cout << LOGSTRING_WARNING;
            break;
        }

        case logType_t::ERROR:
        {
            std::cout << LOGSTRING_ERROR;
            break;
        }

        case logType_t::CRITICAL:
        {
            std::cout << LOGSTRING_CRITICAL;
            break;
        }

        case logType_t::FATAL:
        {
            std::cout << LOGSTRING_FATAL;
            break;
        }

        default:
        {
            std::cout << LOGSTRING_DEBUG;
            break;
        }
    }

    std::cout << " ] " << msg << std::endl;
}

//----------------------------------------------------------------------------------------------------------------------
//
void LoggerStd::log_sc(
                        logType_t log_type,
                        const char *file,
                        const char *function,
                        int line,
                        const char *msg
) {
    std::cout << "[ ";

    switch(log_type)
    {
        case logType_t::DEBUG:
        {
            std::cout << LOGSTRING_DEBUG;
            break;
        }

        case logType_t::INFO:
        {
            std::cout << LOGSTRING_INFO;
            break;
        }

        case logType_t::WARN:
        {
            std::cout << LOGSTRING_WARNING;
            break;
        }

        case logType_t::ERROR:
        {
            std::cout << LOGSTRING_ERROR;
            break;
        }

        case logType_t::CRITICAL:
        {
            std::cout << LOGSTRING_CRITICAL;
            break;
        }

        case logType_t::FATAL:
        {
            std::cout << LOGSTRING_FATAL;
            break;
        }

        default:
        {
            std::cout << LOGSTRING_DEBUG;
            break;
        }
    }

    std::cout << " " << file << ":" << line << " " << function << "() ] " << msg << std::endl;
}