#include "MoveShapeCommand.h"
#include <QLoggingCategory>
#include "SvgEditor/canvasarea.h"

Q_LOGGING_CATEGORY(moveShapeCommandLog, "MoveShapeCommand")

MoveShapeCommand::MoveShapeCommand(CanvasArea* canvasArea, QGraphicsItem* item, const QPointF& oldPos, const QPointF& newPos)
    : Command(QObject::tr("Move Shape")),
      m_canvasArea(canvasArea),
      m_item(item),
      m_oldPos(oldPos),
      m_newPos(newPos)
{
    qCDebug(moveShapeCommandLog) << "MoveShapeCommand created from" << oldPos << "to" << newPos;
}

MoveShapeCommand::~MoveShapeCommand()
{
}

bool MoveShapeCommand::execute()
{
    if (!m_canvasArea || !m_item) {
        qCWarning(moveShapeCommandLog) << "Cannot execute MoveShapeCommand: canvas or item is null";
        return false;
    }

    qCDebug(moveShapeCommandLog) << "Executing move to" << m_newPos;
    m_item->setPos(m_newPos);
    return true;
}

bool MoveShapeCommand::undo()
{
    if (!m_canvasArea || !m_item) {
        qCWarning(moveShapeCommandLog) << "Cannot undo MoveShapeCommand: canvas or item is null";
        return false;
    }

    qCDebug(moveShapeCommandLog) << "Undoing move to" << m_oldPos;
    m_item->setPos(m_oldPos);
    return true;
}

bool MoveShapeCommand::canMergeWith(Command* other) const
{
    if (!other || other->getType() != CommandType::Move) {
        return false;
    }
    auto* otherMove = static_cast<MoveShapeCommand*>(other);
    return otherMove->m_item == this->m_item;
}

void MoveShapeCommand::merge(Command* other)
{
    if (canMergeWith(other)) {
        auto* otherMove = static_cast<MoveShapeCommand*>(other);
        m_newPos = otherMove->m_newPos;
        qCDebug(moveShapeCommandLog) << "Merged with another move command, new position:" << m_newPos;
    }
}
