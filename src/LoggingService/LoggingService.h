#pragma once

#include <string>
#include <fstream>
#include <mutex>
#include <chrono>
#include <iomanip>
#include <sstream>
#include <iostream>

enum class LogLevel {
    Info,
    Warning,
    Error,
    Debug
};

class LoggingService {
public:
    LoggingService(const LoggingService&) = delete;
    LoggingService& operator=(const LoggingService&) = delete;

    static LoggingService& getInstance();

    void setLogFile(const std::string& filePath);
    void log(LogLevel level, const std::string& message);

    void info(const std::string& message);
    void warn(const std::string& message);
    void error(const std::string& message);
    void debug(const std::string& message);

private:
    LoggingService();
    ~LoggingService();

    std::ofstream m_logFile;
    std::string m_logFilePath;
    std::mutex m_mutex;

    std::string getCurrentTimestamp() const;
    std::string logLevelToString(LogLevel level) const;
};
