#include "DrawingLayer.h"
#include <QSvgRenderer>
#include <QGraphicsSvgItem>
#include <QResizeEvent>
#include <QEvent>
#include "LoggingService.h"

DrawingLayer::DrawingLayer(QWidget *parent)
    : QGraphicsView(parent)
{
    m_scene = new QGraphicsScene(this);
    setScene(m_scene);
    
    // 设置视图的基本属性
    setRenderHint(QPainter::Antialiasing);
    setRenderHint(QPainter::SmoothPixmapTransform);
    setViewportUpdateMode(QGraphicsView::FullViewportUpdate);
    
    // 允许拖拽和缩放
    setDragMode(QGraphicsView::ScrollHandDrag);
    setTransformationAnchor(QGraphicsView::AnchorUnderMouse);
    setResizeAnchor(QGraphicsView::AnchorUnderMouse);
    
    LoggingService::getInstance().debug("DrawingLayer constructed");
}

DrawingLayer::~DrawingLayer()
{
    LoggingService::getInstance().debug("DrawingLayer destroyed");
}

void DrawingLayer::setSvgContent(const QString &svgContent)
{
    m_scene->clear();
    
    QSvgRenderer* renderer = new QSvgRenderer(svgContent.toUtf8());
    if (renderer->isValid()) {
        QGraphicsSvgItem* svgItem = new QGraphicsSvgItem();
        svgItem->setSharedRenderer(renderer);
        svgItem->setFlags(QGraphicsItem::ItemClipsToShape);
        
        m_scene->addItem(svgItem);
        m_scene->setSceneRect(renderer->viewBox());
        
        LoggingService::getInstance().debug("SVG content loaded in DrawingLayer");
    } else {
        LoggingService::getInstance().error("Failed to load SVG content in DrawingLayer");
        delete renderer;
    }
    
    updateView();
}

void DrawingLayer::updateView()
{

    resetTransform();
    setSceneRect(m_scene->itemsBoundingRect());
    fitInView(m_scene->itemsBoundingRect(), Qt::KeepAspectRatio);
    
    viewport()->update();
}

void DrawingLayer::changeEvent(QEvent* event)
{
    if (event->type() == QEvent::LanguageChange) {
        LoggingService::getInstance().debug("DrawingLayer language changed");
    }
    
    QGraphicsView::changeEvent(event);
}
