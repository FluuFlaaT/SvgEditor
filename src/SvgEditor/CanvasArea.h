#pragma once
#include <QGraphicsView>
#include <QGraphicsSvgItem>
#include <QGraphicsRectItem>
#include <QGraphicsScene>
#include <QFileInfo>
#include <QScopedPointer>
#include <QSvgRenderer>
#include <QLoggingCategory>
#include <QWheelEvent>
#include <QMouseEvent>
#include <QGraphicsLineItem>
#include <QGraphicsEllipseItem>
#include <QGraphicsPolygonItem>
#include <QGraphicsPathItem>
#include <QPainterPath>
#include <QPolygonF>
#include <QPointF>
#include <QList>
#include <QColor>
#include <QPen>
#include <QBrush>
#include <QCursor>
#include <QApplication>
#include <QInputDialog>
#include <cmath>
#include "XMLParser.h"
#include "ShapeToolBar.h"
#include "EditableTextItem.h"

// 前向声明CoreSvgEngine类
class CoreSvgEngine;

Q_DECLARE_LOGGING_CATEGORY(canvasAreaLog)

class CanvasArea : public QGraphicsView
{
    Q_OBJECT

public:
    explicit CanvasArea(QWidget *parent = nullptr);
    ~CanvasArea();

    bool openFile(const QString&);
    bool openFileWithEngine(CoreSvgEngine* engine);

    QGraphicsScene* scene() const { return m_scene; }

    // Zoom methods
    void zoomIn();
    void zoomOut();
    void resetZoom();
    void fitToView(); // Changed from fitInView to avoid conflict with QGraphicsView::fitInView
    qreal currentZoom() const { return m_zoomFactor; }

    // Tool methods
    void setDragMode(bool enabled);  // Enable/disable drag mode
    void setShapeCreationMode(ShapeType type); // Set the shape creation mode
    void setSelectMode(); // Set selection mode

    // Add a shape to the document
    void addShapeToDocument(QGraphicsItem* item);

    // Get the current CoreSvgEngine
    CoreSvgEngine* getCurrentEngine() const { return m_currentEngine; }
    void setCurrentEngine(CoreSvgEngine* engine) { m_currentEngine = engine; }

    // Get the currently selected item
    QGraphicsItem* getSelectedItem() const;
    ShapeType getSelectedItemType() const;

    // Helper method to determine the type of an item
    ShapeType getItemType(QGraphicsItem* item) const;

signals:
    void zoomChanged(qreal zoomFactor);
    void shapeCreated(QGraphicsItem* item);
    void itemSelected(QGraphicsItem* item, ShapeType type);

protected:
    // Override wheel event for mouse wheel zoom
    void wheelEvent(QWheelEvent *event) override;

    // Override resize event to maintain view
    void resizeEvent(QResizeEvent *event) override;

    // Mouse event handlers for shape creation
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;

private:
    QGraphicsScene* m_scene;
    QGraphicsRectItem* m_backgroundItem;
    QGraphicsSvgItem* m_svgItem;
    QGraphicsRectItem* m_outlineItem;

    // Zoom properties
    qreal m_zoomFactor;
    static constexpr qreal ZOOM_FACTOR_STEP = 1.2;
    static constexpr qreal MIN_ZOOM = 0.1;
    static constexpr qreal MAX_ZOOM = 10.0;

    // Shape creation properties
    ShapeType m_currentShapeType;
    bool m_isDrawing;
    QPointF m_startPoint;
    QPointF m_endPoint;
    QGraphicsItem* m_currentItem;
    QList<QPointF> m_freehandPoints;
    CoreSvgEngine* m_currentEngine;

    // Default style properties
    QPen m_defaultPen;
    QBrush m_defaultBrush;

    // Helper methods
    void setZoom(qreal factor);
    void createShape(const QPointF& startPoint, const QPointF& endPoint);
    void updateShape(const QPointF& endPoint);
    void finalizeShape();

    // Shape creation methods
    QGraphicsLineItem* createLine(const QPointF& startPoint, const QPointF& endPoint);
    QGraphicsPathItem* createFreehandPath(const QList<QPointF>& points);
    QGraphicsRectItem* createRectangle(const QPointF& startPoint, const QPointF& endPoint);
    QGraphicsEllipseItem* createEllipse(const QPointF& startPoint, const QPointF& endPoint);
    QGraphicsPolygonItem* createPolygon(const QPointF& center, qreal radius, int sides);
    QGraphicsPolygonItem* createStar(const QPointF& center, qreal outerRadius, qreal innerRadius);
    QGraphicsPolygonItem* createRegularPolygon(const QPointF& center, qreal radius, int sides);
    EditableTextItem* createText(const QPointF& position, const QString& text = "");
};