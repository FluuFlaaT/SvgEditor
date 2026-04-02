#pragma once
#include "Command.h"
#include <vector>
#include <memory>

class GroupCommand : public Command {
public:
    GroupCommand(const QString& description = QObject::tr("Group"));
    ~GroupCommand() override;

    void addCommand(std::unique_ptr<Command> command);
    size_t getCommandCount() const { return m_commands.size(); }

    bool execute() override;
    bool undo() override;

    CommandType getType() const override { return CommandType::Group; }

private:
    std::vector<std::unique_ptr<Command>> m_commands;
};
