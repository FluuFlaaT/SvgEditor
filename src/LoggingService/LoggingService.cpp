#include "LoggingService.h"
#include <QDebug>
#include <QFile>
#include <QTextStream>
#include <QDateTime>

Q_LOGGING_CATEGORY(logServiceLog, "LoggingService")

LoggingService::LoggingService(QObject* parent)
    : QObject(parent)
    , m_logLevel(LogLevel::Debug)
{
    qCDebug(logServiceLog) << "LoggingService initialized";
}

LoggingService::~LoggingService()
{
}

LoggingService& LoggingService::instance()
{
    static LoggingService instance;
    return instance;
}

void LoggingService::setLogLevel(LogLevel level)
{
    m_logLevel = level;
    qCDebug(logServiceLog) << "Log level changed to:" << levelToString(level);
}

void LoggingService::setCategoryEnabled(const QString& category, bool enabled)
{
    QLoggingCategory categoryObj(category.toUtf8().constData());
    categoryObj.setEnabled(QtDebugMsg, enabled);
    qCDebug(logServiceLog) << "Category" << category << "enabled:" << enabled;
}

bool LoggingService::isCategoryEnabled(const QString& category) const
{
    QLoggingCategory categoryObj(category.toUtf8().constData());
    return categoryObj.isEnabled(QtDebugMsg);
}

void LoggingService::log(LogLevel level, const QString& category, const QString& message)
{
    QString timestamp = QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss.zzz");
    QString formattedMessage = QString("[%1] [%2] [%3] %4")
        .arg(timestamp)
        .arg(levelToString(level))
        .arg(category)
        .arg(message);

    switch (level) {
        case LogLevel::Debug:
            qDebug().noquote() << formattedMessage;
            break;
        case LogLevel::Info:
            qInfo().noquote() << formattedMessage;
            break;
        case LogLevel::Warning:
            qWarning().noquote() << formattedMessage;
            break;
        case LogLevel::Critical:
            qCritical().noquote() << formattedMessage;
            break;
    }

    emit logMessageReceived(formattedMessage);
}

QString LoggingService::levelToString(LogLevel level)
{
    switch (level) {
        case LogLevel::Debug: return "DEBUG";
        case LogLevel::Info: return "INFO";
        case LogLevel::Warning: return "WARNING";
        case LogLevel::Critical: return "CRITICAL";
        default: return "UNKNOWN";
    }
}

LogLevel LoggingService::stringToLevel(const QString& level)
{
    QString upper = level.toUpper();
    if (upper == "DEBUG") return LogLevel::Debug;
    if (upper == "INFO") return LogLevel::Info;
    if (upper == "WARNING") return LogLevel::Warning;
    if (upper == "CRITICAL") return LogLevel::Critical;
    return LogLevel::Debug;
}
