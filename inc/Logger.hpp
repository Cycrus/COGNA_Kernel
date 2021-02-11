/**
 * @file Logger.hpp
 * @author Manuel Nadji (https://github.com/gnarrf95)
 * 
 * @brief This is a Logger system, that can call customized implementations.
 * 
 * **Note:**
 * This Logger system is based on my C library (https://github.com/gnarrf95/libjayc).
 * For the purpose of this project I entirely implemented it new in C++.
 * 
 * To create a custom implementation, implement the @c #log() and @c #log_sc() functions.
 * The enumeration @c #logType_t is used, to show the severity of the message. This is
 * also used to visually differentiate the log types.
 * 
 * To create a global instance, use the @c #init_Global() function.
 * The instance can be destroyed later with the @c #destroy_Global() function.
 * 
 * The macros at the end of the file can be used to call the global instance and
 * automatically use the source code information of where the macro was called.
 * 
 * @date 2021-02-09
 * 
 */

#ifndef INCLUDE_LOGGER_HPP
#define INCLUDE_LOGGER_HPP

/**
 * @brief Abstract class for logger
 * 
 * Also contains the global functions.
 * 
 */
class Logger {
    public:

        /**
         * @brief Marks severity of log message.
         * 
         */
        typedef enum {
            DEBUG,
            INFO,
            WARN,
            ERROR,
            CRITICAL,
            FATAL
        } logType_t;

        /**
         * @brief Function for simple log calls.
         * 
         * Needs to be implemented in custom implementation.
         * Displays warning message, only differentiates between log types.
         * 
         * @param log_type  Type of log message.
         * @param msg       Message to display.
         */
        virtual void log(logType_t log_type, const char *msg) = 0;

        /**
         * @brief Function to call log calls with source code information.
         * 
         * Displays message, as well as information of where it was called.
         * 
         * @param log_type  Type of log message.
         * @param file      File in which log was called.
         * @param function  Function in which log was called.
         * @param line      Line number on which log was called.
         * @param msg       Message to display.
         */
        virtual void log_sc(
                            logType_t log_type,
                            const char *file,
                            const char *function,
                            int line,
                            const char *msg
        ) = 0;

    private:
        /**
         * @brief Global logger instance to be called by global functions.
         * 
         */
        static Logger *global_instance;

    public:
        /**
         * @brief Initializes global instance.
         * 
         * @param new_logger    Logger to set to global.
         * 
         * @return              @c NULL , if failed.
         */
        static Logger *init_Global(Logger *new_logger);

        /**
         * @brief Returns global instance.
         * 
         * @return              Global logger instance.
         * @return              @c NULL , if failed.
         */
        static Logger *get_Global();

        /**
         * @brief Frees global instance.
         * 
         */
        static void destroy_Global();

        /**
         * @brief Function to log with global instance.
         * 
         * @param log_type  Type of log message.
         * @param fmt       Format string used for stdarg.h .
         */
        static void log_global(logType_t log_type, const char *fmt, ...);

        /**
         * @brief Function to log with source code info on global instance.
         * 
         * @param log_type  Type of log message.
         * @param file      File in which log was called,
         * @param function  Function in which log was called.
         * @param line      Line number on which log was called.
         * @param fmt       Format string used for stdarg.h .
         */
        static void log_global_sc(
                                    logType_t log_type,
                                    const char *file,
                                    const char *function,
                                    int line,
                                    const char *fmt, ...
        );
};

/**
 * @brief Macros for specific log types.
 * 
 * These macros can be used in whole program. They use the global instance, so this has to be set for
 * them to work.
 * 
 * Automatically inserts source code information of place where they are called.
 * 
 */
#define LOG_DEBUG(fmt, ...) Logger::log_global_sc(Logger::logType_t::DEBUG, __FILE__, __func__, __LINE__, fmt, ##__VA_ARGS__)
#define LOG_INFO(fmt, ...) Logger::log_global_sc(Logger::logType_t::INFO, __FILE__, __func__, __LINE__, fmt, ##__VA_ARGS__)
#define LOG_WARN(fmt, ...) Logger::log_global_sc(Logger::logType_t::WARN, __FILE__, __func__, __LINE__, fmt, ##__VA_ARGS__)
#define LOG_ERROR(fmt, ...) Logger::log_global_sc(Logger::logType_t::ERROR, __FILE__, __func__, __LINE__, fmt, ##__VA_ARGS__)
#define LOG_CRIT(fmt, ...) Logger::log_global_sc(Logger::logType_t::CRITICAL, __FILE__, __func__, __LINE__, fmt, ##__VA_ARGS__)
#define LOG_FATAL(fmt, ...) Logger::log_global_sc(Logger::logType_t::FATAL, __FILE__, __func__, __LINE__, fmt, ##__VA_ARGS__)

#endif /* INCLUDE_LOGGER_HPP */