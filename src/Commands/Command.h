#pragma once
#include <QString>
#include <QDebug>
#include <QLoggingCategory>

Q_DECLARE_LOGGING_CATEGORY(commandLog)


class Command {
public:
    Command(const QString& description = "");
    virtual ~Command() = default;

    virtual bool execute() = 0;

    virtual bool undo() = 0;

    QString getDescription() const { return m_description; }

protected:
    QString m_description; // Human-readable description of the command
};
