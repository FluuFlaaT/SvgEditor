#include "ResizeShapeCommand.h"
#include <QLoggingCategory>
#include <QGraphicsRectItem>
#include <QGraphicsEllipseItem>
#include "SvgEditor/canvasarea.h"

Q_LOGGING_CATEGORY(resizeShapeCommandLog, "ResizeShapeCommand")

ResizeShapeCommand::ResizeShapeCommand(CanvasArea* canvasArea, QGraphicsItem* item, const QRectF& oldRect, const QRectF& newRect)
    : Command(QObject::tr("Resize Shape")),
      m_canvasArea(canvasArea),
      m_item(item),
      m_oldRect(oldRect),
      m_newRect(newRect)
{
    qCDebug(resizeShapeCommandLog) << "ResizeShapeCommand created from" << oldRect << "to" << newRect;
}

ResizeShapeCommand::~ResizeShapeCommand()
{
}

bool ResizeShapeCommand::execute()
{
    if (!m_canvasArea || !m_item) {
        qCWarning(resizeShapeCommandLog) << "Cannot execute ResizeShapeCommand: canvas or item is null";
        return false;
    }

    qCDebug(resizeShapeCommandLog) << "Executing resize to" << m_newRect;
    
    if (auto* rectItem = dynamic_cast<QGraphicsRectItem*>(m_item)) {
        rectItem->setRect(m_newRect);
    } else if (auto* ellipseItem = dynamic_cast<QGraphicsEllipseItem*>(m_item)) {
        ellipseItem->setRect(m_newRect);
    }
    
    return true;
}

bool ResizeShapeCommand::undo()
{
    if (!m_canvasArea || !m_item) {
        qCWarning(resizeShapeCommandLog) << "Cannot undo ResizeShapeCommand: canvas or item is null";
        return false;
    }

    qCDebug(resizeShapeCommandLog) << "Undoing resize to" << m_oldRect;
    
    if (auto* rectItem = dynamic_cast<QGraphicsRectItem*>(m_item)) {
        rectItem->setRect(m_oldRect);
    } else if (auto* ellipseItem = dynamic_cast<QGraphicsEllipseItem*>(m_item)) {
        ellipseItem->setRect(m_oldRect);
    }
    
    return true;
}

bool ResizeShapeCommand::canMergeWith(Command* other) const
{
    if (!other || other->getType() != CommandType::Resize) {
        return false;
    }
    auto* otherResize = static_cast<ResizeShapeCommand*>(other);
    return otherResize->m_item == this->m_item;
}

void ResizeShapeCommand::merge(Command* other)
{
    if (canMergeWith(other)) {
        auto* otherResize = static_cast<ResizeShapeCommand*>(other);
        m_newRect = otherResize->m_newRect;
        qCDebug(resizeShapeCommandLog) << "Merged with another resize command, new rect:" << m_newRect;
    }
}
