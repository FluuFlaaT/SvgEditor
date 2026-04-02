#pragma once
#include "IShapeFactory.h"
#include <QGraphicsLineItem>
#include <QGraphicsPathItem>
#include <QGraphicsRectItem>
#include <QGraphicsEllipseItem>
#include <QGraphicsPolygonItem>
#include <QPainterPath>
#include <QtMath>

class ShapeFactory : public IShapeFactory {
public:
    ShapeFactory() {
        m_defaultPen = QPen(Qt::black, 2, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin);
        m_defaultBrush = QBrush(Qt::white);
    }

    void setDefaultPen(const QPen& pen) override { m_defaultPen = pen; }
    void setDefaultBrush(const QBrush& brush) override { m_defaultBrush = brush; }
    QPen getDefaultPen() const override { return m_defaultPen; }
    QBrush getDefaultBrush() const override { return m_defaultBrush; }

    QGraphicsItem* createShape(ShapeType type, const QPointF& startPoint, const QPointF& endPoint) override {
        switch (type) {
            case ShapeType::Line:
                return createLine(startPoint, endPoint);
            case ShapeType::Rectangle:
                return createRectangle(startPoint, endPoint);
            case ShapeType::Ellipse:
                return createEllipse(startPoint, endPoint);
            default:
                return nullptr;
        }
    }

    QGraphicsLineItem* createLine(const QPointF& startPoint, const QPointF& endPoint) override {
        QGraphicsLineItem* lineItem = new QGraphicsLineItem(QLineF(startPoint, endPoint));
        lineItem->setPen(m_defaultPen);
        lineItem->setFlag(QGraphicsItem::ItemIsSelectable, true);
        lineItem->setFlag(QGraphicsItem::ItemIsMovable, true);
        return lineItem;
    }

    QGraphicsPathItem* createFreehandPath(const QList<QPointF>& points) override {
        if (points.isEmpty()) {
            return nullptr;
        }

        QPainterPath path;
        path.moveTo(points.first());

        for (int i = 1; i < points.size(); ++i) {
            path.lineTo(points[i]);
        }

        QGraphicsPathItem* pathItem = new QGraphicsPathItem(path);
        pathItem->setPen(m_defaultPen);
        pathItem->setFlag(QGraphicsItem::ItemIsSelectable, true);
        pathItem->setFlag(QGraphicsItem::ItemIsMovable, true);
        return pathItem;
    }

    QGraphicsRectItem* createRectangle(const QPointF& startPoint, const QPointF& endPoint) override {
        QRectF rect = QRectF(startPoint, endPoint).normalized();
        QGraphicsRectItem* rectItem = new QGraphicsRectItem(rect);
        rectItem->setPen(m_defaultPen);
        rectItem->setBrush(m_defaultBrush);
        rectItem->setFlag(QGraphicsItem::ItemIsSelectable, true);
        rectItem->setFlag(QGraphicsItem::ItemIsMovable, true);
        return rectItem;
    }

    QGraphicsEllipseItem* createEllipse(const QPointF& startPoint, const QPointF& endPoint) override {
        QRectF rect = QRectF(startPoint, endPoint).normalized();
        QGraphicsEllipseItem* ellipseItem = new QGraphicsEllipseItem(rect);
        ellipseItem->setPen(m_defaultPen);
        ellipseItem->setBrush(m_defaultBrush);
        ellipseItem->setFlag(QGraphicsItem::ItemIsSelectable, true);
        ellipseItem->setFlag(QGraphicsItem::ItemIsMovable, true);
        return ellipseItem;
    }

    QGraphicsPolygonItem* createPolygon(const QPointF& center, qreal radius, int sides) override {
        return createRegularPolygon(center, radius, sides);
    }

    QGraphicsPolygonItem* createStar(const QPointF& center, qreal outerRadius, qreal innerRadius) override {
        if (outerRadius <= 0 || innerRadius <= 0) {
            return nullptr;
        }

        const int points = 5;
        QPolygonF star;
        const qreal angleStep = M_PI / points;

        qreal startAngle = M_PI / 2.0;

        for (int i = 0; i < 2 * points; ++i) {
            qreal radius = (i % 2 == 0) ? outerRadius : innerRadius;
            qreal angle = startAngle - i * angleStep;
            qreal x = center.x() + radius * qCos(angle);
            qreal y = center.y() - radius * qSin(angle);
            star << QPointF(x, y);
        }

        QGraphicsPolygonItem* starItem = new QGraphicsPolygonItem(star);
        starItem->setPen(m_defaultPen);
        starItem->setBrush(m_defaultBrush);
        starItem->setFlag(QGraphicsItem::ItemIsSelectable, true);
        starItem->setFlag(QGraphicsItem::ItemIsMovable, true);
        return starItem;
    }

    QGraphicsPolygonItem* createRegularPolygon(const QPointF& center, qreal radius, int sides) override {
        if (sides < 3 || radius <= 0) {
            return nullptr;
        }

        QPolygonF polygon;
        const qreal angleStep = 2.0 * M_PI / sides;

        qreal startAngle = M_PI / 2.0;

        for (int i = 0; i < sides; ++i) {
            qreal angle = startAngle - i * angleStep;
            qreal x = center.x() + radius * qCos(angle);
            qreal y = center.y() - radius * qSin(angle);
            polygon << QPointF(x, y);
        }

        QGraphicsPolygonItem* polygonItem = new QGraphicsPolygonItem(polygon);
        polygonItem->setPen(m_defaultPen);
        polygonItem->setBrush(m_defaultBrush);
        polygonItem->setFlag(QGraphicsItem::ItemIsSelectable, true);
        polygonItem->setFlag(QGraphicsItem::ItemIsMovable, true);
        return polygonItem;
    }

private:
    QPen m_defaultPen;
    QBrush m_defaultBrush;
};
