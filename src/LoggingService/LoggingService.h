#pragma once
#include <QObject>
#include <QString>
#include <QLoggingCategory>
#include <memory>

class QLogCategory;

enum class LogLevel {
    Debug,
    Info,
    Warning,
    Critical
};

class LoggingService : public QObject {
    Q_OBJECT

public:
    static LoggingService& instance();

    void setLogLevel(LogLevel level);
    LogLevel getLogLevel() const { return m_logLevel; }

    void setCategoryEnabled(const QString& category, bool enabled);
    bool isCategoryEnabled(const QString& category) const;

    void log(LogLevel level, const QString& category, const QString& message);

    static QString levelToString(LogLevel level);
    static LogLevel stringToLevel(const QString& level);

signals:
    void logMessageReceived(const QString& message);

private:
    LoggingService(QObject* parent = nullptr);
    ~LoggingService();
    LoggingService(const LoggingService&) = delete;
    LoggingService& operator=(const LoggingService&) = delete;

    LogLevel m_logLevel;
};

#define LOG_DEBUG(category, msg) \
    LoggingService::instance().log(LogLevel::Debug, category, msg)

#define LOG_INFO(category, msg) \
    LoggingService::instance().log(LogLevel::Info, category, msg)

#define LOG_WARNING(category, msg) \
    LoggingService::instance().log(LogLevel::Warning, category, msg)

#define LOG_CRITICAL(category, msg) \
    LoggingService::instance().log(LogLevel::Critical, category, msg)
