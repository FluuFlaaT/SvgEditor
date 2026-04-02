#include "ChangeStyleCommand.h"
#include <QLoggingCategory>
#include <QGraphicsItem>
#include <QGraphicsPathItem>
#include "SvgEditor/canvasarea.h"

Q_LOGGING_CATEGORY(changeStyleCommandLog, "ChangeStyleCommand")

ChangeStyleCommand::ChangeStyleCommand(CanvasArea* canvasArea, QGraphicsItem* item, 
                                       const QPen& oldPen, const QBrush& oldBrush, 
                                       const QPen& newPen, const QBrush& newBrush)
    : Command(QObject::tr("Change Style")),
      m_canvasArea(canvasArea),
      m_item(item),
      m_oldPen(oldPen),
      m_oldBrush(oldBrush),
      m_newPen(newPen),
      m_newBrush(newBrush)
{
    qCDebug(changeStyleCommandLog) << "ChangeStyleCommand created";
}

ChangeStyleCommand::~ChangeStyleCommand()
{
}

bool ChangeStyleCommand::execute()
{
    if (!m_canvasArea || !m_item) {
        qCWarning(changeStyleCommandLog) << "Cannot execute ChangeStyleCommand: canvas or item is null";
        return false;
    }

    qCDebug(changeStyleCommandLog) << "Executing style change";
    if (auto* pathItem = dynamic_cast<QGraphicsPathItem*>(m_item)) {
        pathItem->setPen(m_newPen);
        pathItem->setBrush(m_newBrush);
    }
    return true;
}

bool ChangeStyleCommand::undo()
{
    if (!m_canvasArea || !m_item) {
        qCWarning(changeStyleCommandLog) << "Cannot undo ChangeStyleCommand: canvas or item is null";
        return false;
    }

    qCDebug(changeStyleCommandLog) << "Undoing style change";
    if (auto* pathItem = dynamic_cast<QGraphicsPathItem*>(m_item)) {
        pathItem->setPen(m_oldPen);
        pathItem->setBrush(m_oldBrush);
    }
    return true;
}
