#include "CommandManager.h"

Q_LOGGING_CATEGORY(commandManagerLog, "CommandManager")

// Initialize the static instance pointer
CommandManager* CommandManager::m_instance = nullptr;

CommandManager* CommandManager::instance()
{
    if (!m_instance) {
        m_instance = new CommandManager();
    }
    return m_instance;
}

CommandManager::CommandManager()
{
    qCDebug(commandManagerLog) << "CommandManager created";
}

CommandManager::~CommandManager()
{
    qCDebug(commandManagerLog) << "CommandManager destroyed";
    clear();
}

bool CommandManager::executeCommand(std::unique_ptr<Command> command)
{
    if (!command) {
        qCWarning(commandManagerLog) << "Attempted to execute null command";
        return false;
    }

    qCDebug(commandManagerLog) << "Executing command:" << command->getDescription();

    if (command->execute()) {
        // Executing a new command invalidates any previous redo history
        qDeleteAll(m_redoStack);
        m_redoStack.clear();

        m_undoStack.append(command.release());

        emit undoRedoChanged();

        return true;
    }

    qCWarning(commandManagerLog) << "Command execution failed";
    return false;
}

bool CommandManager::undo()
{
    if (m_undoStack.isEmpty()) {
        qCDebug(commandManagerLog) << "Nothing to undo";
        return false;
    }

    Command* command = m_undoStack.takeLast();

    qCDebug(commandManagerLog) << "Undoing command:" << command->getDescription();

    if (command->undo()) {
        m_redoStack.append(command);

        emit undoRedoChanged();

        return true;
    }

    qCWarning(commandManagerLog) << "Command undo failed";
    delete command;
    return false;
}

bool CommandManager::redo()
{
    if (m_redoStack.isEmpty()) {
        qCDebug(commandManagerLog) << "Nothing to redo";
        return false;
    }

    Command* command = m_redoStack.takeLast();

    qCDebug(commandManagerLog) << "Redoing command:" << command->getDescription();

    if (command->execute()) {
        m_undoStack.append(command);

        emit undoRedoChanged();

        return true;
    }

    qCWarning(commandManagerLog) << "Command redo failed";
    delete command;
    return false;
}

bool CommandManager::canUndo() const
{
    return !m_undoStack.isEmpty();
}

bool CommandManager::canRedo() const
{
    return !m_redoStack.isEmpty();
}

QString CommandManager::undoText() const
{
    if (m_undoStack.isEmpty()) {
        return QString();
    }
    return m_undoStack.last()->getDescription();
}

QString CommandManager::redoText() const
{
    if (m_redoStack.isEmpty()) {
        return QString();
    }
    return m_redoStack.last()->getDescription();
}

void CommandManager::clear()
{
    qCDebug(commandManagerLog) << "Clearing undo/redo stacks";
    qDeleteAll(m_undoStack);
    m_undoStack.clear();
    qDeleteAll(m_redoStack);
    m_redoStack.clear();
    emit undoRedoChanged();
}
