#include "RemoveShapeCommand.h"
#include <QLoggingCategory>
#include "../../CoreSvgEngine/SvgDocument.h"
#include "../../CoreSvgEngine/SvgShapes.h"
#include "CommandUtils.h"

Q_LOGGING_CATEGORY(removeShapeCommandLog, "RemoveShapeCommand")

RemoveShapeCommand::RemoveShapeCommand(CanvasArea* canvasArea, QGraphicsItem* item, ShapeType shapeType)
    : Command(QObject::tr("Remove %1").arg(getShapeTypeName(shapeType))),
      m_canvasArea(canvasArea),
      m_item(item),
      m_shapeType(shapeType),
      m_itemOwned(false),
      m_sceneIndex(-1)
{
    qCDebug(removeShapeCommandLog) << "RemoveShapeCommand created for shape type:" << static_cast<int>(shapeType);
}

RemoveShapeCommand::~RemoveShapeCommand()
{
    // If we own the item and it's not in a scene, delete it
    if (m_itemOwned && m_item && !m_item->scene()) {
        qCDebug(removeShapeCommandLog) << "Deleting owned item in RemoveShapeCommand destructor";
        delete m_item;
        m_item = nullptr;
    }
}

bool RemoveShapeCommand::execute()
{
    if (!m_canvasArea || !m_item) {
        qCWarning(removeShapeCommandLog) << "Cannot execute RemoveShapeCommand: canvas or item is null";
        return false;
    }

    qCDebug(removeShapeCommandLog) << "Executing RemoveShapeCommand";

    // Get the engine and document
    CoreSvgEngine* engine = m_canvasArea->getCurrentEngine();
    if (!engine || !engine->getCurrentDocument()) {
        qCWarning(removeShapeCommandLog) << "Cannot execute RemoveShapeCommand: engine or document is null";
        return false;
    }

    // Store the scene index for proper restoration
    m_sceneIndex = m_canvasArea->scene()->items().indexOf(m_item);

    // TODO: Store the SVG element data for undo
    // This would require finding the corresponding SVG element for the graphics item
    // For now, we'll just remove the item from the scene

    // Remove the item from the scene
    m_canvasArea->scene()->removeItem(m_item);

    // We now own the item
    m_itemOwned = true;

    // TODO: Remove the element from the document
    // This would require finding the corresponding SVG element for the graphics item

    return true;
}

bool RemoveShapeCommand::undo()
{
    if (!m_canvasArea || !m_item) {
        qCWarning(removeShapeCommandLog) << "Cannot undo RemoveShapeCommand: canvas or item is null";
        return false;
    }

    qCDebug(removeShapeCommandLog) << "Undoing RemoveShapeCommand";

    // Add the item back to the scene
    m_canvasArea->scene()->addItem(m_item);

    // We no longer own the item
    m_itemOwned = false;

    // TODO: Add the element back to the document
    // This would require restoring the SVG element data

    return true;
}
