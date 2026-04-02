#pragma once
#include <QGraphicsItem>
#include <QPointF>
#include <QPen>
#include <QBrush>
#include "ShapeType.h"

class IShapeFactory {
public:
    virtual ~IShapeFactory() = default;

    virtual void setDefaultPen(const QPen& pen) = 0;
    virtual void setDefaultBrush(const QBrush& brush) = 0;
    virtual QPen getDefaultPen() const = 0;
    virtual QBrush getDefaultBrush() const = 0;

    virtual QGraphicsItem* createShape(ShapeType type, const QPointF& startPoint, const QPointF& endPoint) = 0;
    virtual QGraphicsLineItem* createLine(const QPointF& startPoint, const QPointF& endPoint) = 0;
    virtual QGraphicsPathItem* createFreehandPath(const QList<QPointF>& points) = 0;
    virtual QGraphicsRectItem* createRectangle(const QPointF& startPoint, const QPointF& endPoint) = 0;
    virtual QGraphicsEllipseItem* createEllipse(const QPointF& startPoint, const QPointF& endPoint) = 0;
    virtual QGraphicsPolygonItem* createPolygon(const QPointF& center, qreal radius, int sides) = 0;
    virtual QGraphicsPolygonItem* createStar(const QPointF& center, qreal outerRadius, qreal innerRadius) = 0;
    virtual QGraphicsPolygonItem* createRegularPolygon(const QPointF& center, qreal radius, int sides) = 0;
};
