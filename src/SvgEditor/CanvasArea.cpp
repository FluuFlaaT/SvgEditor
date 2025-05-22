#define MAX_N 25565
#include "CanvasArea.h"
#include "../CoreSvgEngine/CoreSvgEngine.h"

Q_LOGGING_CATEGORY(canvasAreaLog, "CanvasArea")


CanvasArea::CanvasArea(QWidget *parent)
    : QGraphicsView(parent),
    m_backgroundItem(nullptr),
    m_outlineItem(nullptr),
    m_zoomFactor(1.0)
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

void CanvasArea::zoomIn()
{
    setZoom(m_zoomFactor * ZOOM_FACTOR_STEP);
}

void CanvasArea::zoomOut()
{
    setZoom(m_zoomFactor / ZOOM_FACTOR_STEP);
}

void CanvasArea::resetZoom()
{
    setZoom(1.0);
}

void CanvasArea::fitToView()
{
    if (m_scene->items().isEmpty()) {
        return;
    }

    // Reset the view scale
    resetTransform();
    m_zoomFactor = 1.0;

    // Fit the scene in the view
    QRectF sceneRect = m_scene->itemsBoundingRect();
    if (!sceneRect.isEmpty()) {
        // Call the parent class's fitInView method
        QGraphicsView::fitInView(sceneRect, Qt::KeepAspectRatio);

        // Calculate the new zoom factor
        QTransform transform = this->transform();
        m_zoomFactor = transform.m11(); // Use the horizontal scale factor

        // Emit the zoom changed signal
        emit zoomChanged(m_zoomFactor);
        qCDebug(canvasAreaLog) << "Fit to view, new zoom factor:" << m_zoomFactor;
    }
}

void CanvasArea::setZoom(qreal factor)
{
    // Clamp the zoom factor to the allowed range
    factor = qBound(MIN_ZOOM, factor, MAX_ZOOM);

    if (qFuzzyCompare(factor, m_zoomFactor)) {
        return; // No change in zoom
    }

    // Calculate the scale factor relative to the current transform
    qreal scaleFactor = factor / m_zoomFactor;

    // Apply the scale transformation
    scale(scaleFactor, scaleFactor);

    // Update the zoom factor
    m_zoomFactor = factor;

    // Emit the zoom changed signal
    emit zoomChanged(m_zoomFactor);
    qCDebug(canvasAreaLog) << "Zoom changed to:" << m_zoomFactor;
}

void CanvasArea::wheelEvent(QWheelEvent *event)
{
    // Check if Ctrl key is pressed for zoom
    if (event->modifiers() & Qt::ControlModifier) {
        // Calculate zoom factor based on wheel delta
        qreal factor = (event->angleDelta().y() > 0) ? ZOOM_FACTOR_STEP : 1.0 / ZOOM_FACTOR_STEP;
        setZoom(m_zoomFactor * factor);
        event->accept();
    } else {
        // Default wheel behavior (scroll)
        QGraphicsView::wheelEvent(event);
    }
}

void CanvasArea::resizeEvent(QResizeEvent *event)
{
    QGraphicsView::resizeEvent(event);

    // If we're in "fit in view" mode, we might want to adjust the view here
    // For now, we'll just maintain the current zoom level
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

bool CanvasArea::openFileWithEngine(CoreSvgEngine* engine) {
    qCDebug(canvasAreaLog) << "Opening SVG file with engine";
    QGraphicsScene *s = scene();

    // Clear the scene but don't delete the items yet
    // This prevents accessing deleted items later
    s->clear();

    // Get document dimensions and background color
    SvgDocument* doc = engine->getCurrentDocument();
    if (!doc) {
        qCWarning(canvasAreaLog) << "No document in engine";
        return false;
    }

    double width = doc->getWidth();
    double height = doc->getHeight();
    Color bgColor = doc->getBackgroundColor();

    qCDebug(canvasAreaLog) << "Document dimensions:" << width << "x" << height;
    qCDebug(canvasAreaLog) << "Background color:" << QString::fromStdString(bgColor.toString());

    // Create background rectangle
    QRectF docRect(0, 0, width, height);
    m_backgroundItem = new QGraphicsRectItem(docRect);
    m_backgroundItem->setBrush(QColor(bgColor.r, bgColor.g, bgColor.b, bgColor.alpha));
    m_backgroundItem->setPen(Qt::NoPen);
    m_backgroundItem->setZValue(-1); // Ensure it's behind all other items
    s->addItem(m_backgroundItem);

    // Add all graphics items from the document
    // Make sure we're not adding invalid items
    for(auto it = doc->m_graphicsItems.begin(); it != doc->m_graphicsItems.end(); ) {
        QGraphicsItem* item = *it;
        if (item) {
            s->addItem(item);
            ++it;
        } else {
            // Remove invalid items from the vector
            it = doc->m_graphicsItems.erase(it);
            qCWarning(canvasAreaLog) << "Removed invalid graphics item from document";
        }
    }

    // Create outline rectangle
    m_outlineItem = new QGraphicsRectItem(docRect);
    QPen outline(Qt::black, 2, Qt::DashLine);
    outline.setCosmetic(true);
    m_outlineItem->setPen(outline);
    m_outlineItem->setBrush(Qt::NoBrush);
    m_outlineItem->setZValue(MAX_N); // Ensure it's in front of all other items
    s->addItem(m_outlineItem);

    // Set scene rect with some padding
    s->setSceneRect(docRect.adjusted(-10, -10, 10, 10));

    qCDebug(canvasAreaLog) << "Scene setup complete with" << doc->m_graphicsItems.size() << "items";
    return true;
}
