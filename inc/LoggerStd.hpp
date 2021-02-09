/**
 * @file LoggerStd.hpp
 * @author Manuel Nadji (https://github.com/gnarrf95)
 * 
 * @brief Logger implementation using iostream.
 * 
 * @date 2021-02-09
 * 
 */

#ifndef INCLUDE_LOGGERSTD_HPP
#define INCLUDE_LOGGERSTD_HPP

#include "Logger.hpp"

/**
 * @brief Customized class of Logger.
 * 
 * Uses iostream library to output to console.
 * 
 */
class LoggerStd : public Logger {
    public:
        /**
         * @brief Construct a new LoggerStd object.
         * 
         */
        LoggerStd();

        /**
         * @brief Destroy the LoggerStd object
         * 
         */
        ~LoggerStd();

        /**
         * @brief Logs message to console.
         * 
         * @param log_type  Type of log message.
         * @param msg       Message to display.
         */
        void log(logType_t log_type, const char *msg);

        /**
         * @brief Logs message with source code information to console.
         * 
         * @param log_type  Type of log message.
         * @param file      File in which log was called.
         * @param function  Function in which log was called.
         * @param line      Line number on which log was called.
         * @param msg       Message to display.
         */
        void log_sc(
                    logType_t log_type,
                    const char *file,
                    const char *function,
                    int line,
                    const char *msg
        );
};

#endif /* INCLUDE_LOGGERSTD_HPP */