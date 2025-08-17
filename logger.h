#ifndef FIDELITY_LOGGER_H
#define FIDELITY_LOGGER_H

#include <fstream>
#include <mutex>
#include <string>

namespace fidelity {
    enum class LogLevel {
        DEBUG,
        INFO,
        WARNING,
        ERROR
    };

    class Logger {
    public:
        static Logger& getInstance();
        Logger(const Logger&) = delete;
        Logger& operator=(const Logger&) = delete;

        void log(LogLevel level, const std::string& message);
        void setLogFile(const std::string& filename);
        void enableConsoleOutput(bool enable);

    private:
        Logger();
        ~Logger();

        static std::string getCurrentTimestamp();
        static std::string levelToString(LogLevel level);

        std::ofstream logFile;
        std::mutex logMutex;
        bool consoleOutput;
        std::string logFilename;
    };

#define LOG_DEBUG(msg) fidelity::Logger::getInstance().log(fidelity::LogLevel::DEBUG, msg)
#define LOG_INFO(msg) fidelity::Logger::getInstance().log(fidelity::LogLevel::INFO, msg)
#define LOG_WARNING(msg) fidelity::Logger::getInstance().log(fidelity::LogLevel::WARNING, msg)
#define LOG_ERROR(msg) fidelity::Logger::getInstance().log(fidelity::LogLevel::ERROR, msg)

} // namespace fidelity

#endif