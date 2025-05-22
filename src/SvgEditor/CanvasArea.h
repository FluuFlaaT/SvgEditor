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
#include "XMLParser.h"

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

signals:
    void zoomChanged(qreal zoomFactor);

protected:
    // Override wheel event for mouse wheel zoom
    void wheelEvent(QWheelEvent *event) override;

    // Override resize event to maintain view
    void resizeEvent(QResizeEvent *event) override;

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

    // Helper method to set zoom
    void setZoom(qreal factor);
};