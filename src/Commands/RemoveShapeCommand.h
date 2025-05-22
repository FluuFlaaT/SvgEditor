#pragma once
#include "Command.h"
#include <QGraphicsItem>
#include <memory>
#include "SvgEditor/CanvasArea.h"
#include "CoreSvgEngine/SvgElement.h"
#include "CoreSvgEngine/CoreSvgEngine.h"

/**
 * @brief Command for removing a shape from the document
 */
class RemoveShapeCommand : public Command {
public:
    /**
     * @brief Construct a RemoveShapeCommand
     * @param canvasArea The canvas area where the shape will be removed
     * @param item The graphics item to remove
     * @param shapeType The type of shape being removed
     */
    RemoveShapeCommand(CanvasArea* canvasArea, QGraphicsItem* item, ShapeType shapeType);
    ~RemoveShapeCommand() override;

    /**
     * @brief Execute the command to remove the shape
     * @return true if successful, false otherwise
     */
    bool execute() override;

    /**
     * @brief Undo the command by adding the shape back
     * @return true if successful, false otherwise
     */
    bool undo() override;

private:
    CanvasArea* m_canvasArea;
    QGraphicsItem* m_item;
    ShapeType m_shapeType;
    bool m_itemOwned; // Whether this command owns the item

    // Store the SVG element data for undo
    std::unique_ptr<SvgElement> m_svgElement;
    int m_sceneIndex; // Index in the scene for proper restoration
};
