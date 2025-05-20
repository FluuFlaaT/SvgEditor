#include "LoggingService.h"
#include <iostream>
#include <filesystem>

const std::string DEFAULT_LOG_FILE = "application.log";

LoggingService::LoggingService() {
    std::string logDirectory = "logs";
    std::string defaultLogPath = logDirectory + "/" + DEFAULT_LOG_FILE;

    if (!std::filesystem::exists(logDirectory)) {
        std::filesystem::create_directory(logDirectory);
    }
    setLogFile(defaultLogPath);
}

LoggingService::~LoggingService() {
    if (m_logFile.is_open()) {
        m_logFile << "[" << getCurrentTimestamp() << "] "
                  << "[" << logLevelToString(LogLevel::Info) << "] "
                  << "Logging service stopped." << std::endl;
        m_logFile.close();
    }
}

LoggingService& LoggingService::getInstance() {
    static LoggingService instance;
    return instance;
}

void LoggingService::setLogFile(const std::string& filePath) {
    std::lock_guard<std::mutex> lock(m_mutex);
    if (m_logFile.is_open()) {
        m_logFile.close();
    }

    m_logFilePath = filePath;
    
    std::filesystem::path pathObj(filePath);
    if (pathObj.has_parent_path()) {
        std::filesystem::path parentDir = pathObj.parent_path();
        if (!parentDir.empty() && !std::filesystem::exists(parentDir)) {
            try {
                std::filesystem::create_directories(parentDir);
            } catch (const std::filesystem::filesystem_error& e) {
                std::cerr << "Error creating directories for log file: " << e.what() << std::endl;
            }
        }
    }

    m_logFile.open(m_logFilePath, std::ios::out | std::ios::app);
    if (!m_logFile.is_open()) {
        std::cerr << "Error: Could not open log file: " << m_logFilePath << std::endl;
    } else {
         m_logFile << "[" << getCurrentTimestamp() << "] "
                  << "[" << logLevelToString(LogLevel::Info) << "] "
                  << "Logging service started. Log file: " + m_logFilePath << std::endl;
    }
}

std::string LoggingService::getCurrentTimestamp() const {
    auto now = std::chrono::system_clock::now();
    auto in_time_t = std::chrono::system_clock::to_time_t(now);
    
    std::stringstream ss;
    std::tm buf{}; // Initialize buf
    localtime_s(&buf, &in_time_t);
    ss << std::put_time(&buf, "%Y-%m-%d %H:%M:%S");
    
    auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()) % 1000;
    ss << '.' << std::setfill('0') << std::setw(3) << ms.count();
    
    return ss.str();
}

std::string LoggingService::logLevelToString(LogLevel level) const {
    switch (level) {
        case LogLevel::Info:    return "INFO";
        case LogLevel::Warning: return "WARN";
        case LogLevel::Error:   return "ERROR";
        case LogLevel::Debug:   return "DEBUG";
        default:                return "UNKNOWN";
    }
}

void LoggingService::log(LogLevel level, const std::string& message) {
    std::lock_guard<std::mutex> lock(m_mutex);
    
    std::string logEntry = "[" + getCurrentTimestamp() + "] " +
                           "[" + logLevelToString(level) + "] " +
                           message;

    // Log to terminal
    if (level == LogLevel::Error) {
        std::cerr << logEntry << std::endl;
    } else {
        std::cout << logEntry << std::endl;
    }

    // Log to file
    if (m_logFile.is_open()) {
        m_logFile << logEntry << std::endl;
    } else {
        // Optionally, if file isn't open, still print a specific message to console 
        // indicating file logging failure, though the main log already went to cout/cerr.
        // This part might be redundant now or could be adapted.
        if (level == LogLevel::Error) {
             std::cerr << "[NO_FILE_LOG] (Error opening log file: " << m_logFilePath << ")" << std::endl;
        } else {
             std::cout << "[NO_FILE_LOG] (Error opening log file: " << m_logFilePath << ")" << std::endl;
        }
    }
}

void LoggingService::info(const std::string& message) {
    log(LogLevel::Info, message);
}

void LoggingService::warn(const std::string& message) {
    log(LogLevel::Warning, message);
}

void LoggingService::error(const std::string& message) {
    log(LogLevel::Error, message);
}

void LoggingService::debug(const std::string& message) {
    log(LogLevel::Debug, message);
}
