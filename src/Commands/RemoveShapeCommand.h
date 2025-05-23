#pragma once
#include "Command.h"
#include <QGraphicsItem>
#include <memory>
#include "SvgEditorForwards.h"
#include "CoreSvgEngine/svgelement.h"
#include "CoreSvgEngine/coresvgengine.h"

class RemoveShapeCommand : public Command {
public:
    RemoveShapeCommand(CanvasArea* canvasArea, QGraphicsItem* item, ShapeType shapeType);
    ~RemoveShapeCommand() override;

    bool execute() override;

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
