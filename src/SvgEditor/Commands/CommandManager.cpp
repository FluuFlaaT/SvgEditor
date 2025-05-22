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

    // Execute the command
    if (command->execute()) {
        // Clear the redo stack when a new command is executed
        qDeleteAll(m_redoStack);
        m_redoStack.clear();

        // Add the command to the undo stack (transfer ownership)
        m_undoStack.append(command.release());

        // Notify listeners that the undo/redo state has changed
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

    // Get the command from the undo stack
    Command* command = m_undoStack.takeLast();

    qCDebug(commandManagerLog) << "Undoing command:" << command->getDescription();

    // Undo the command
    if (command->undo()) {
        // Add the command to the redo stack
        m_redoStack.append(command);

        // Notify listeners that the undo/redo state has changed
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

    // Get the command from the redo stack
    Command* command = m_redoStack.takeLast();

    qCDebug(commandManagerLog) << "Redoing command:" << command->getDescription();

    // Execute the command again
    if (command->execute()) {
        // Add the command back to the undo stack
        m_undoStack.append(command);

        // Notify listeners that the undo/redo state has changed
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
