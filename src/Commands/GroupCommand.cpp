#include "GroupCommand.h"
#include <QLoggingCategory>

Q_LOGGING_CATEGORY(groupCommandLog, "GroupCommand")

GroupCommand::GroupCommand(const QString& description)
    : Command(description)
{
    qCDebug(groupCommandLog) << "GroupCommand created";
}

GroupCommand::~GroupCommand()
{
}

void GroupCommand::addCommand(std::unique_ptr<Command> command)
{
    if (command) {
        m_commands.push_back(std::move(command));
    }
}

bool GroupCommand::execute()
{
    qCDebug(groupCommandLog) << "Executing GroupCommand with" << m_commands.size() << "commands";
    
    for (auto& cmd : m_commands) {
        if (cmd && !cmd->execute()) {
            qCWarning(groupCommandLog) << "Failed to execute command in group";
            return false;
        }
    }
    return true;
}

bool GroupCommand::undo()
{
    qCDebug(groupCommandLog) << "Undoing GroupCommand with" << m_commands.size() << "commands";
    
    for (auto it = m_commands.rbegin(); it != m_commands.rend(); ++it) {
        if (*it && !(*it)->undo()) {
            qCWarning(groupCommandLog) << "Failed to undo command in group";
            return false;
        }
    }
    return true;
}
