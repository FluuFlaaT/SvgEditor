#pragma once
#include "Command.h"
#include <QGraphicsItem>
#include <memory>
#include "SvgEditorForwards.h"
#include "CoreSvgEngine/svgelement.h"
#include "CoreSvgEngine/coresvgengine.h"

class AddShapeCommand : public Command {
public:
    AddShapeCommand(CanvasArea* canvasArea, QGraphicsItem* item, ShapeType shapeType);
    ~AddShapeCommand() override;

    bool execute() override;

    bool undo() override;

private:
    CanvasArea* m_canvasArea;
    QGraphicsItem* m_item;
    ShapeType m_shapeType;
    bool m_itemOwned; // Whether this command owns the item
    std::string m_elementId; // ID of the SVG element for removal
};
