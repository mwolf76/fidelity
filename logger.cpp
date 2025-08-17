#include "logger.h"
#include <chrono>
#include <iomanip>
#include <iostream>
#include <sstream>

namespace fidelity {

    Logger& Logger::getInstance()
    {
        static Logger INSTANCE;
        return INSTANCE;
    }

    Logger::Logger()
        : consoleOutput(true)
        , logFilename("/var/log/fidelity.log")
    {
        logFile.open(logFilename, std::ios::app);
    }

    Logger::~Logger()
    {
        if (logFile.is_open()) {
            logFile.close();
        }
    }

    void Logger::setLogFile(const std::string& filename)
    {
        std::lock_guard<std::mutex> lock(logMutex);
        if (logFile.is_open()) {
            logFile.close();
        }
        logFilename = filename;
        logFile.open(logFilename, std::ios::app);
    }

    void Logger::enableConsoleOutput(bool enable)
    {
        consoleOutput = enable;
    }

    void Logger::log(const LogLevel level, const std::string& message)
    {
        std::lock_guard lock(logMutex);

        const std::string timestamp = getCurrentTimestamp();
        const std::string levelStr = levelToString(level);
        const std::string logLine = "[" + timestamp + "] [" + levelStr + "] " + message;

        if (consoleOutput) {
            std::cout << logLine << std::endl;
        }

        if (logFile.is_open()) {
            logFile << logLine << std::endl;
            logFile.flush();
        }
    }

    std::string Logger::getCurrentTimestamp()
    {
        std::stringstream ss;

        const auto now = std::chrono::system_clock::now();
        const auto time_t = std::chrono::system_clock::to_time_t(now);

        ss
            << std::put_time(
                std::localtime(&time_t),
                "%Y-%m-%d %H:%M:%S");

        return ss.str();
    }

    std::string Logger::levelToString(const LogLevel level)
    {
        switch (level) {
            case LogLevel::DEBUG:
                return "DEBUG";

            case LogLevel::INFO:
                return "INFO";

            case LogLevel::WARNING:
                return "WARN";

            case LogLevel::ERROR:
                return "ERROR";

            default:
                return "UNKNOWN";
        }
    }

} // namespace fidelity