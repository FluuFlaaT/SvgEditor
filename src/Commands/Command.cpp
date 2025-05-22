#include "Command.h"

Q_LOGGING_CATEGORY(commandLog, "Command")

Command::Command(const QString& description)
    : m_description(description)
{
    qCDebug(commandLog) << "Command created:" << m_description;
}
