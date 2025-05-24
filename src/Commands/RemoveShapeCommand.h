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
    bool m_itemOwned; // Track ownership to prevent double-deletion during command lifecycle

    // Preserve SVG element state for complete restoration
    std::unique_ptr<SvgElement> m_svgElement;
    int m_sceneIndex; // Preserve original Z-order for accurate undo positioning
};
