#pragma once
#include <QString>
#include <QDebug>
#include <QLoggingCategory>

Q_DECLARE_LOGGING_CATEGORY(commandLog)

enum class CommandType {
    Unknown,
    Add,
    Remove,
    Move,
    Resize,
    Style,
    Group
};

class Command {
public:
    Command(const QString& description = "");
    virtual ~Command() = default;

    virtual bool execute() = 0;
    virtual bool undo() = 0;

    virtual bool canMergeWith(Command* other) const { return false; }
    virtual void merge(Command* other) { Q_UNUSED(other); }

    virtual CommandType getType() const { return CommandType::Unknown; }
    QString getDescription() const { return m_description; }

protected:
    QString m_description;
};



