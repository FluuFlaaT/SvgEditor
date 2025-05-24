#include "AddShapeCommand.h"
#include <QLoggingCategory>
#include "CoreSvgEngine/svgdocument.h"
#include "CommandUtils.h"
#include "SvgEditor/canvasarea.h"

Q_LOGGING_CATEGORY(addShapeCommandLog, "AddShapeCommand")

AddShapeCommand::AddShapeCommand(CanvasArea* canvasArea, QGraphicsItem* item, ShapeType shapeType)
    : Command(QObject::tr("Add %1").arg(getShapeTypeName(shapeType))),
      m_canvasArea(canvasArea),
      m_item(item),
      m_shapeType(shapeType),
      m_itemOwned(false)
{
    qCDebug(addShapeCommandLog) << "AddShapeCommand created for shape type:" << static_cast<int>(shapeType);
}

AddShapeCommand::~AddShapeCommand()
{
    // Clean up orphaned items to prevent memory leaks in case of command stack rollback
    if (m_itemOwned && m_item && !m_item->scene()) {
        qCDebug(addShapeCommandLog) << "Deleting owned item in AddShapeCommand destructor";
        delete m_item;
        m_item = nullptr;
    }
}

bool AddShapeCommand::execute()
{
    if (!m_canvasArea || !m_item) {
        qCWarning(addShapeCommandLog) << "Cannot execute AddShapeCommand: canvas or item is null";
        return false;
    }

    qCDebug(addShapeCommandLog) << "Executing AddShapeCommand";

    // Check if the item is already in the scene to handle redo operations correctly
    bool alreadyInScene = (m_item->scene() != nullptr);

    if (!alreadyInScene) {
        m_canvasArea->scene()->addItem(m_item);
        qCDebug(addShapeCommandLog) << "Added item to scene";
    } else {
        qCDebug(addShapeCommandLog) << "Item is already in scene, skipping scene addition";
    }

    // Synchronize with SVG document model - essential for proper serialization
    m_canvasArea->addShapeToDocument(m_item);

    // Store element ID for reliable undo operation - we need this because 
    // the document structure might change between execute and undo
    CoreSvgEngine* engine = m_canvasArea->getCurrentEngine();
    if (engine && engine->getCurrentDocument()) {
        auto& elements = engine->getCurrentDocument()->getElements();
        if (!elements.empty()) {
            m_elementId = elements.back()->getID();
            qCDebug(addShapeCommandLog) << "Stored element ID for undo:" << QString::fromStdString(m_elementId);
        }
    }

    // Transfer ownership to the scene to ensure proper Qt object lifecycle
    m_itemOwned = false;

    emit m_canvasArea->shapeCreated(m_item);

    return true;
}

bool AddShapeCommand::undo()
{
    if (!m_canvasArea) {
        qCWarning(addShapeCommandLog) << "Cannot undo AddShapeCommand: canvas is null";
        return false;
    }

    qCDebug(addShapeCommandLog) << "Undoing AddShapeCommand";

    CoreSvgEngine* engine = m_canvasArea->getCurrentEngine();
    if (!engine || !engine->getCurrentDocument()) {
        qCWarning(addShapeCommandLog) << "Cannot undo AddShapeCommand: engine or document is null";
        return false;
    }

    // Remove from document first to maintain model-view consistency
    if (!m_elementId.empty()) {
        qCDebug(addShapeCommandLog) << "Removing element with ID:" << QString::fromStdString(m_elementId);
        engine->getCurrentDocument()->removeElementById(m_elementId);
    }

    if (m_item && m_item->scene()) {
        m_canvasArea->scene()->removeItem(m_item);
        // Reclaim ownership to prevent deletion until command is destroyed
        m_itemOwned = true;
    } else {
        qCWarning(addShapeCommandLog) << "Item not found in scene during undo";
    }

    return true;
}
