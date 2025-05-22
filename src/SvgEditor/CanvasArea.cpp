#define MAX_N 25565
#include "CanvasArea.h"

Q_LOGGING_CATEGORY(canvasAreaLog, "CanvasArea")


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
    auto extractedIds = XMLParser::getInstance().processIds(fileName);

    QGraphicsScene *s = scene();
    qCDebug(canvasAreaLog) << "Opening SVG file:" << fileName;

    // reset scene
    s->clear();
    m_backgroundItem = nullptr;
    m_svgItem = nullptr;
    m_outlineItem = nullptr;
    
    const bool drawBackground = (m_backgroundItem ? m_backgroundItem->isVisible() : false);
    const bool drawOutline = (m_outlineItem ? m_outlineItem->isVisible() : true);


    QScopedPointer<QGraphicsSvgItem> svgItem(new QGraphicsSvgItem(fileName));
    if(!svgItem->renderer()->isValid()) {
        qCWarning(canvasAreaLog) << "Invalid SVG renderer for file:" << fileName;
        return false;
    }
    qCDebug(canvasAreaLog) << "SVG renderer is valid";

    m_svgItem = svgItem.take();
    m_svgItem->setFlags(QGraphicsItem::ItemClipsToShape);
    m_svgItem->setCacheMode(QGraphicsItem::NoCache);
    m_svgItem->setZValue(0);
    // s->addItem(m_svgItem);
    qCDebug(canvasAreaLog) << "Added main SVG item to scene, bounding rect:" << m_svgItem->boundingRect();

    m_backgroundItem = new QGraphicsRectItem(m_svgItem->boundingRect());
    m_backgroundItem->setBrush(Qt::white);
    m_backgroundItem->setPen(Qt::NoPen);
    m_backgroundItem->setVisible(drawBackground);
    m_backgroundItem->setZValue(-1);
    s->addItem(m_backgroundItem);
    qCDebug(canvasAreaLog) << "Added background item, visible:" << drawBackground;

    // Extract and add individual SVG items based on extracted IDs
    int layerCount = 0;
    for (const QString& id : extractedIds) {
        QGraphicsSvgItem* svgChild = new QGraphicsSvgItem();
        svgChild->setSharedRenderer(m_svgItem->renderer());
        svgChild->setElementId(id);
        
        // Copy important properties from the parent SVG item
        svgChild->setFlags(QGraphicsItem::ItemClipsToShape);
        svgChild->setCacheMode(QGraphicsItem::NoCache);
        svgChild->setZValue(layerCount++);
        svgChild->setPos(m_svgItem->pos());
        svgChild->setTransform(m_svgItem->transform());
        svgChild->setOpacity(m_svgItem->opacity());
        
        s->addItem(svgChild);
        qCDebug(canvasAreaLog) << "Added SVG item with ID:" << id 
                              << "Layer:" << layerCount 
                              << "Bounds:" << svgChild->boundingRect();
    }
    qCDebug(canvasAreaLog) << "Added" << layerCount << "SVG items to scene";

    m_outlineItem = new QGraphicsRectItem(m_svgItem->boundingRect());
    QPen outline(Qt::black, 2, Qt::DashLine);
    outline.setCosmetic(true);
    m_outlineItem->setPen(outline);
    m_outlineItem->setBrush(Qt::NoBrush);
    m_outlineItem->setVisible(drawOutline);
    m_outlineItem->setZValue(MAX_N);
    s->addItem(m_outlineItem);
    qCDebug(canvasAreaLog) << "Added outline item, visible:" << drawOutline;

    s->setSceneRect(m_outlineItem->boundingRect().adjusted(-10, -10, 10, 10));
    qCDebug(canvasAreaLog) << "Set scene rect to:" << s->sceneRect();
    
    return true;
}