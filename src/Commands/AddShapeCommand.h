#pragma once
#include "Command.h"
#include <QGraphicsItem>
#include <memory>
#include "SvgEditor/CanvasArea.h"
#include "CoreSvgEngine/SvgElement.h"
#include "CoreSvgEngine/CoreSvgEngine.h"

/**
 * @brief Command for adding a shape to the document
 */
class AddShapeCommand : public Command {
public:
    /**
     * @brief Construct an AddShapeCommand
     * @param canvasArea The canvas area where the shape will be added
     * @param item The graphics item to add
     * @param shapeType The type of shape being added
     */
    AddShapeCommand(CanvasArea* canvasArea, QGraphicsItem* item, ShapeType shapeType);
    ~AddShapeCommand() override;

    /**
     * @brief Execute the command to add the shape
     * @return true if successful, false otherwise
     */
    bool execute() override;

    /**
     * @brief Undo the command by removing the shape
     * @return true if successful, false otherwise
     */
    bool undo() override;

private:
    CanvasArea* m_canvasArea;
    QGraphicsItem* m_item;
    ShapeType m_shapeType;
    bool m_itemOwned; // Whether this command owns the item
    std::string m_elementId; // ID of the SVG element for removal
};
