#pragma once
#include "Command.h"
#include <QGraphicsItem>
#include <QRectF>
#include "SvgEditorForwards.h"

class ResizeShapeCommand : public Command {
public:
    ResizeShapeCommand(CanvasArea* canvasArea, QGraphicsItem* item, const QRectF& oldRect, const QRectF& newRect);
    ~ResizeShapeCommand() override;

    bool execute() override;
    bool undo() override;

    bool canMergeWith(Command* other) const override;
    void merge(Command* other) override;
    CommandType getType() const override { return CommandType::Resize; }

private:
    CanvasArea* m_canvasArea;
    QGraphicsItem* m_item;
    QRectF m_oldRect;
    QRectF m_newRect;
};
