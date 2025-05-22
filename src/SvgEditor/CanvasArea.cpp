#define MAX_N 25565
#include "CanvasArea.h"
#include <QGraphicsScene>
#include <QFileInfo>
#include <QScopedPointer>
#include <QSvgRenderer>

CanvasArea::CanvasArea(QWidget *parent)
    : QGraphicsView(parent),
    m_backgroundItem(nullptr),
    m_outlineItem(nullptr)
{
    m_scene = new QGraphicsScene(this);
    setScene(m_scene);
    setTransformationAnchor(AnchorUnderMouse);
    setDragMode(ScrollHandDrag);
    setViewportUpdateMode(FullViewportUpdate);

    // Init Background
    QPixmap tilePixmap(64, 64);
    tilePixmap.fill(Qt::white);
    QPainter tilePainter(&tilePixmap);
    QColor color(220, 220, 220);
    tilePainter.fillRect(0, 0, 32, 32, color);
    tilePainter.fillRect(32, 32, 32, 32, color);
    tilePainter.end();
    
    setRenderHint(QPainter::Antialiasing);
    setDragMode(QGraphicsView::RubberBandDrag);
    setOptimizationFlags(QGraphicsView::DontSavePainterState);
    setViewportUpdateMode(QGraphicsView::FullViewportUpdate);
    setTransformationAnchor(QGraphicsView::AnchorUnderMouse);

    setBackgroundBrush(tilePixmap);
}

CanvasArea::~CanvasArea()
{
    delete m_scene;
}

bool CanvasArea::openFile(const QString& fileName) {
    QGraphicsScene *s = scene();
    
    const bool drawBackground = (m_backgroundItem ? m_backgroundItem->isVisible() : false);
    const bool drawOutline = (m_outlineItem ? m_outlineItem->isVisible() : true);

    // 文件不存在时 return false
    if (!QFileInfo::exists(fileName)) {
        return false;
    }
    
    QScopedPointer<QGraphicsSvgItem> svgItem(new QGraphicsSvgItem(fileName));
    if(!svgItem->renderer()->isValid()) {
        return false;
    }

    m_svgItem = svgItem.take();
    m_svgItem->setFlags(QGraphicsItem::ItemClipsToShape);
    m_svgItem->setCacheMode(QGraphicsItem::NoCache);
    m_svgItem->setZValue(0);

    m_backgroundItem = new QGraphicsRectItem(m_svgItem->boundingRect());
    m_backgroundItem->setBrush(Qt::white);
    m_backgroundItem->setPen(Qt::NoPen);
    m_backgroundItem->setVisible(drawBackground);
    m_backgroundItem->setZValue(-1);
    s->addItem(m_backgroundItem);

    // 将svgItem中的物品遍历分层添加到QVector中去
    QList<QGraphicsItem *> items = m_svgItem->childItems();
    int layerCount = 0;
    for (QGraphicsItem *item : items) {
        if (item->type() == QGraphicsSvgItem::Type) {
            QGraphicsSvgItem *svgChild = qgraphicsitem_cast<QGraphicsSvgItem *>(item);
            svgChild->setSharedRenderer(svgItem->renderer());
            svgChild->setZValue(layerCount++);
            s->addItem(svgChild);
        }
    }

    m_outlineItem = new QGraphicsRectItem(m_svgItem->boundingRect());
    QPen outline(Qt::black, 2, Qt::DashLine);
    outline.setCosmetic(true);
    m_outlineItem->setPen(outline);
    m_outlineItem->setBrush(Qt::NoBrush);
    m_outlineItem->setVisible(drawOutline);
    m_outlineItem->setZValue(MAX_N);
    s->addItem(m_outlineItem);

    s->setSceneRect(m_outlineItem->boundingRect().adjusted(-10, -10, 10, 10));
    return true;
}