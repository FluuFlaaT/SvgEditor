#pragma once
#include "Command.h"
#include <QGraphicsItem>
#include <QPointF>
#include "SvgEditorForwards.h"

class MoveShapeCommand : public Command {
public:
    MoveShapeCommand(CanvasArea* canvasArea, QGraphicsItem* item, const QPointF& oldPos, const QPointF& newPos);
    ~MoveShapeCommand() override;

    bool execute() override;
    bool undo() override;

    bool canMergeWith(Command* other) const override;
    void merge(Command* other) override;
    CommandType getType() const override { return CommandType::Move; }

private:
    CanvasArea* m_canvasArea;
    QGraphicsItem* m_item;
    QPointF m_oldPos;
    QPointF m_newPos;
};
