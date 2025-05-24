#include "RemoveShapeCommand.h"
#include <QLoggingCategory>
#include <algorithm> // For std::find
#include "CoreSvgEngine/svgdocument.h"
#include "CoreSvgEngine/svgshapes.h"
#include "CommandUtils.h"
#include "SvgEditor/canvasarea.h"

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
    // Clean up orphaned items to prevent memory leaks during command stack operations
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

    CoreSvgEngine* engine = m_canvasArea->getCurrentEngine();
    SvgDocument* doc = engine ? engine->getCurrentDocument() : nullptr;
    if (!engine || !doc) {
        qCWarning(removeShapeCommandLog) << "Cannot execute RemoveShapeCommand: engine or document is null";
        return false;
    }

    // Preserve original position for accurate restoration during undo
    m_sceneIndex = m_canvasArea->scene()->items().indexOf(m_item);

    // Maintain synchronization between document model and graphics scene
    auto it = std::find(doc->m_graphicsItems.begin(), doc->m_graphicsItems.end(), m_item);
    if (it != doc->m_graphicsItems.end()) {
        doc->m_graphicsItems.erase(it);
        qCDebug(removeShapeCommandLog) << "Removed item from document's graphics items list";
    } else {
        qCWarning(removeShapeCommandLog) << "Item not found in document's graphics items list";
    }

    m_canvasArea->scene()->removeItem(m_item);

    // Take ownership to prevent premature deletion
    m_itemOwned = true;

    return true;
}

bool RemoveShapeCommand::undo()
{
    if (!m_canvasArea || !m_item) {
        qCWarning(removeShapeCommandLog) << "Cannot undo RemoveShapeCommand: canvas or item is null";
        return false;
    }

    qCDebug(removeShapeCommandLog) << "Undoing RemoveShapeCommand";

    CoreSvgEngine* engine = m_canvasArea->getCurrentEngine();
    SvgDocument* doc = engine ? engine->getCurrentDocument() : nullptr;
    if (!engine || !doc) {
        qCWarning(removeShapeCommandLog) << "Cannot undo RemoveShapeCommand: engine or document is null";
        return false;
    }

    m_canvasArea->scene()->addItem(m_item);

    // Restore document model consistency - avoid duplicates
    if (std::find(doc->m_graphicsItems.begin(), doc->m_graphicsItems.end(), m_item) == doc->m_graphicsItems.end()) {
        doc->m_graphicsItems.push_back(m_item);
        qCDebug(removeShapeCommandLog) << "Added item back to document's graphics items list";
    } else {
        qCWarning(removeShapeCommandLog) << "Item already in document's graphics items list";
    }

    // Transfer ownership back to the scene
    m_itemOwned = false;

    return true;
}
