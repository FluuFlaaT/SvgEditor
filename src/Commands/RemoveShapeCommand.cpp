#include "RemoveShapeCommand.h"
#include <QLoggingCategory>
#include <algorithm> // For std::find
#include "CoreSvgEngine/SvgDocument.h"
#include "CoreSvgEngine/SvgShapes.h"
#include "CommandUtils.h"
#include "SvgEditor/CanvasArea.h"

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
    SvgDocument* doc = engine ? engine->getCurrentDocument() : nullptr;
    if (!engine || !doc) {
        qCWarning(removeShapeCommandLog) << "Cannot execute RemoveShapeCommand: engine or document is null";
        return false;
    }

    // Store the scene index for proper restoration
    m_sceneIndex = m_canvasArea->scene()->items().indexOf(m_item);

    // Remove the item from the document's graphics items list
    auto it = std::find(doc->m_graphicsItems.begin(), doc->m_graphicsItems.end(), m_item);
    if (it != doc->m_graphicsItems.end()) {
        doc->m_graphicsItems.erase(it);
        qCDebug(removeShapeCommandLog) << "Removed item from document's graphics items list";
    } else {
        qCWarning(removeShapeCommandLog) << "Item not found in document's graphics items list";
    }

    // Remove the item from the scene
    m_canvasArea->scene()->removeItem(m_item);

    // We now own the item
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

    // Get the engine and document
    CoreSvgEngine* engine = m_canvasArea->getCurrentEngine();
    SvgDocument* doc = engine ? engine->getCurrentDocument() : nullptr;
    if (!engine || !doc) {
        qCWarning(removeShapeCommandLog) << "Cannot undo RemoveShapeCommand: engine or document is null";
        return false;
    }

    // Add the item back to the scene
    m_canvasArea->scene()->addItem(m_item);

    // Add the item back to the document's graphics items list
    if (std::find(doc->m_graphicsItems.begin(), doc->m_graphicsItems.end(), m_item) == doc->m_graphicsItems.end()) {
        doc->m_graphicsItems.push_back(m_item);
        qCDebug(removeShapeCommandLog) << "Added item back to document's graphics items list";
    } else {
        qCWarning(removeShapeCommandLog) << "Item already in document's graphics items list";
    }

    // We no longer own the item
    m_itemOwned = false;

    return true;
}
