#pragma once
#include <QGraphicsItem>
#include <QPointF>
#include "ShapeType.h"

class ICanvasController {
public:
    virtual ~ICanvasController() = default;

    virtual void setZoom(qreal factor) = 0;
    virtual void zoomIn() = 0;
    virtual void zoomOut() = 0;
    virtual void resetZoom() = 0;
    virtual void fitToView() = 0;
    virtual qreal currentZoom() const = 0;

    virtual void setTool(ShapeType type) = 0;
    virtual ShapeType currentTool() const = 0;

    virtual QGraphicsItem* getSelectedItem() const = 0;
    virtual ShapeType getSelectedItemType() const = 0;
    virtual void deleteSelectedItem() = 0;

    virtual void addShape(QGraphicsItem* item) = 0;
    virtual void removeShape(QGraphicsItem* item) = 0;

    virtual QGraphicsScene* getScene() const = 0;
};
