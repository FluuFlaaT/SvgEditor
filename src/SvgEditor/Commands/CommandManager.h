#pragma once
#include <QStack>
#include <QObject>
#include <memory>
#include <QLoggingCategory>
#include "Command.h"

Q_DECLARE_LOGGING_CATEGORY(commandManagerLog)

/**
 * @brief The CommandManager class manages the undo/redo stacks
 *
 * This class is responsible for executing commands and maintaining
 * the undo and redo stacks. It is implemented as a singleton.
 */
class CommandManager : public QObject {
    Q_OBJECT

public:
    /**
     * @brief Get the singleton instance of the CommandManager
     * @return The CommandManager instance
     */
    static CommandManager* instance();

    /**
     * @brief Execute a command and add it to the undo stack
     * @param command The command to execute
     * @return true if the command was executed successfully, false otherwise
     */
    bool executeCommand(std::unique_ptr<Command> command);

    /**
     * @brief Undo the last executed command
     * @return true if a command was undone, false if there are no commands to undo
     */
    bool undo();

    /**
     * @brief Redo the last undone command
     * @return true if a command was redone, false if there are no commands to redo
     */
    bool redo();

    /**
     * @brief Check if there are commands that can be undone
     * @return true if there are commands in the undo stack, false otherwise
     */
    bool canUndo() const;

    /**
     * @brief Check if there are commands that can be redone
     * @return true if there are commands in the redo stack, false otherwise
     */
    bool canRedo() const;

    /**
     * @brief Get the description of the command that would be undone next
     * @return The description of the next command to undo, or an empty string if none
     */
    QString undoText() const;

    /**
     * @brief Get the description of the command that would be redone next
     * @return The description of the next command to redo, or an empty string if none
     */
    QString redoText() const;

    /**
     * @brief Clear both undo and redo stacks
     */
    void clear();

signals:
    /**
     * @brief Signal emitted when the undo/redo stacks change
     */
    void undoRedoChanged();

private:
    // Private constructor for singleton
    CommandManager();
    ~CommandManager();

    // Disable copy and move
    CommandManager(const CommandManager&) = delete;
    CommandManager& operator=(const CommandManager&) = delete;
    CommandManager(CommandManager&&) = delete;
    CommandManager& operator=(CommandManager&&) = delete;

    // Stacks for undo and redo commands
    QList<Command*> m_undoStack;
    QList<Command*> m_redoStack;

    // Singleton instance
    static CommandManager* m_instance;
};
