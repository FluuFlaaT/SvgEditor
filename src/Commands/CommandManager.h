#pragma once
#include <QStack>
#include <QObject>
#include <memory>
#include <QLoggingCategory>
#include "Command.h"

Q_DECLARE_LOGGING_CATEGORY(commandManagerLog)

class CommandManager : public QObject {
    Q_OBJECT

public:
    static CommandManager* instance();

    bool executeCommand(std::unique_ptr<Command> command);

    bool undo();

    bool redo();

    bool canUndo() const;

    bool canRedo() const;

    QString undoText() const;

    QString redoText() const;

    void clear();

signals:

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
