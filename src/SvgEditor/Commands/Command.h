#pragma once
#include <QString>
#include <QDebug>
#include <QLoggingCategory>

Q_DECLARE_LOGGING_CATEGORY(commandLog)

/**
 * @brief The Command abstract base class for the Command Pattern
 * 
 * This class defines the interface for all commands in the undo/redo system.
 * Each command must implement execute() and undo() methods.
 */
class Command {
public:
    Command(const QString& description = "");
    virtual ~Command() = default;

    /**
     * @brief Execute the command
     * @return true if the command was executed successfully, false otherwise
     */
    virtual bool execute() = 0;

    /**
     * @brief Undo the command
     * @return true if the command was undone successfully, false otherwise
     */
    virtual bool undo() = 0;

    /**
     * @brief Get the description of the command
     * @return The description of the command
     */
    QString getDescription() const { return m_description; }

protected:
    QString m_description; // Human-readable description of the command
};
