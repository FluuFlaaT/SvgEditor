#pragma once
#include "Command.h"
#include <QGraphicsItem>
#include <QPen>
#include <QBrush>
#include "SvgEditorForwards.h"

class ChangeStyleCommand : public Command {
public:
    ChangeStyleCommand(CanvasArea* canvasArea, QGraphicsItem* item, const QPen& oldPen, const QBrush& oldBrush, const QPen& newPen, const QBrush& newBrush);
    ~ChangeStyleCommand() override;

    bool execute() override;
    bool undo() override;

    CommandType getType() const override { return CommandType::Style; }

private:
    CanvasArea* m_canvasArea;
    QGraphicsItem* m_item;
    QPen m_oldPen;
    QBrush m_oldBrush;
    QPen m_newPen;
    QBrush m_newBrush;
};
