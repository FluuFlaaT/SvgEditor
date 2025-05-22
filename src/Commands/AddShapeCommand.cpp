#include "AddShapeCommand.h"
#include <QLoggingCategory>
#include "CoreSvgEngine/SvgDocument.h"
#include "CommandUtils.h"

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
    // If we own the item and it's not in a scene, delete it
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

    // Check if the item is already in the scene
    bool alreadyInScene = (m_item->scene() != nullptr);

    // If the item is not in the scene, add it
    if (!alreadyInScene) {
        m_canvasArea->scene()->addItem(m_item);
        qCDebug(addShapeCommandLog) << "Added item to scene";
    } else {
        qCDebug(addShapeCommandLog) << "Item is already in scene, skipping scene addition";
    }

    // Add the shape to the document if it's not already there
    // The addShapeToDocument method should handle this check internally
    m_canvasArea->addShapeToDocument(m_item);

    // Store the element ID for undo
    CoreSvgEngine* engine = m_canvasArea->getCurrentEngine();
    if (engine && engine->getCurrentDocument()) {
        // The element ID would be the last added element
        auto& elements = engine->getCurrentDocument()->getElements();
        if (!elements.empty()) {
            m_elementId = elements.back()->getID();
            qCDebug(addShapeCommandLog) << "Stored element ID for undo:" << QString::fromStdString(m_elementId);
        }
    }

    // We no longer own the item, it's now owned by the scene
    m_itemOwned = false;

    // Emit the shapeCreated signal
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

    // Get the engine and document
    CoreSvgEngine* engine = m_canvasArea->getCurrentEngine();
    if (!engine || !engine->getCurrentDocument()) {
        qCWarning(addShapeCommandLog) << "Cannot undo AddShapeCommand: engine or document is null";
        return false;
    }

    // Remove the element from the document by ID
    if (!m_elementId.empty()) {
        qCDebug(addShapeCommandLog) << "Removing element with ID:" << QString::fromStdString(m_elementId);
        engine->getCurrentDocument()->removeElementById(m_elementId);
    }

    // Find the item in the scene
    if (m_item && m_item->scene()) {
        // Remove the item from the scene
        m_canvasArea->scene()->removeItem(m_item);

        // We now own the item again
        m_itemOwned = true;
    } else {
        qCWarning(addShapeCommandLog) << "Item not found in scene during undo";
    }

    return true;
}
