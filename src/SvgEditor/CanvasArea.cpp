#define MAX_N 25565
#include "CanvasArea.h"
#include "../CoreSvgEngine/CoreSvgEngine.h"

Q_LOGGING_CATEGORY(canvasAreaLog, "CanvasArea")


CanvasArea::CanvasArea(QWidget *parent)
    : QGraphicsView(parent),
    m_backgroundItem(nullptr),
    m_outlineItem(nullptr),
    m_zoomFactor(1.0),
    m_currentShapeType(ShapeType::None),
    m_isDrawing(false),
    m_currentItem(nullptr),
    m_currentEngine(nullptr)
{
    m_scene = new QGraphicsScene(this);
    setScene(m_scene);
    setTransformationAnchor(AnchorUnderMouse);
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
    // Default to RubberBandDrag mode for selection
    setDragMode(QGraphicsView::RubberBandDrag);
    setOptimizationFlags(QGraphicsView::DontSavePainterState);
    setViewportUpdateMode(QGraphicsView::FullViewportUpdate);
    setTransformationAnchor(QGraphicsView::AnchorUnderMouse);

    setBackgroundBrush(tilePixmap);

    // Initialize default style properties
    m_defaultPen = QPen(Qt::black, 2, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin);
    m_defaultBrush = QBrush(Qt::white);
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

void CanvasArea::createShape(const QPointF& startPoint, const QPointF& endPoint)
{
    // Remove the current item if it exists
    if (m_currentItem) {
        m_scene->removeItem(m_currentItem);
        delete m_currentItem;
        m_currentItem = nullptr;
    }

    // Create a new shape based on the current shape type
    switch (m_currentShapeType) {
        case ShapeType::Line:
            m_currentItem = createLine(startPoint, endPoint);
            break;
        case ShapeType::Rectangle:
            m_currentItem = createRectangle(startPoint, endPoint);
            break;
        case ShapeType::Ellipse:
            m_currentItem = createEllipse(startPoint, endPoint);
            break;
        case ShapeType::Pentagon: {
            QPointF center = QPointF((startPoint.x() + endPoint.x()) / 2, (startPoint.y() + endPoint.y()) / 2);
            qreal radius = QLineF(center, endPoint).length();
            m_currentItem = createRegularPolygon(center, radius, 5);
            break;
        }
        case ShapeType::Star: {
            QPointF center = QPointF((startPoint.x() + endPoint.x()) / 2, (startPoint.y() + endPoint.y()) / 2);
            qreal outerRadius = QLineF(center, endPoint).length();
            qreal innerRadius = outerRadius * 0.4; // Inner radius is 40% of outer radius
            m_currentItem = createStar(center, outerRadius, innerRadius);
            break;
        }
        case ShapeType::Hexagon: {
            QPointF center = QPointF((startPoint.x() + endPoint.x()) / 2, (startPoint.y() + endPoint.y()) / 2);
            qreal radius = QLineF(center, endPoint).length();
            m_currentItem = createRegularPolygon(center, radius, 6);
            break;
        }
        default:
            break;
    }

    // Add the item to the scene
    if (m_currentItem) {
        m_scene->addItem(m_currentItem);
    }
}

void CanvasArea::updateShape(const QPointF& endPoint)
{
    // Update the current shape based on the new end point
    if (!m_currentItem) {
        return;
    }

    // Remove the current item
    m_scene->removeItem(m_currentItem);

    // Create a new shape with the updated end point
    createShape(m_startPoint, endPoint);
}

void CanvasArea::finalizeShape()
{
    // Finalize the current shape
    if (!m_currentItem) {
        return;
    }

    // Add the shape to the document
    addShapeToDocument(m_currentItem);

    // Reset the current item pointer (ownership transferred to the document)
    m_currentItem = nullptr;
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

QGraphicsLineItem* CanvasArea::createLine(const QPointF& startPoint, const QPointF& endPoint)
{
    QGraphicsLineItem* lineItem = new QGraphicsLineItem(QLineF(startPoint, endPoint));
    lineItem->setPen(m_defaultPen);
    lineItem->setFlag(QGraphicsItem::ItemIsSelectable, true);
    lineItem->setFlag(QGraphicsItem::ItemIsMovable, true);
    return lineItem;
}

QGraphicsPathItem* CanvasArea::createFreehandPath(const QList<QPointF>& points)
{
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

QGraphicsRectItem* CanvasArea::createRectangle(const QPointF& startPoint, const QPointF& endPoint)
{
    QRectF rect = QRectF(startPoint, endPoint).normalized();
    QGraphicsRectItem* rectItem = new QGraphicsRectItem(rect);
    rectItem->setPen(m_defaultPen);
    rectItem->setBrush(m_defaultBrush);
    rectItem->setFlag(QGraphicsItem::ItemIsSelectable, true);
    rectItem->setFlag(QGraphicsItem::ItemIsMovable, true);
    return rectItem;
}

QGraphicsEllipseItem* CanvasArea::createEllipse(const QPointF& startPoint, const QPointF& endPoint)
{
    QRectF rect = QRectF(startPoint, endPoint).normalized();
    QGraphicsEllipseItem* ellipseItem = new QGraphicsEllipseItem(rect);
    ellipseItem->setPen(m_defaultPen);
    ellipseItem->setBrush(m_defaultBrush);
    ellipseItem->setFlag(QGraphicsItem::ItemIsSelectable, true);
    ellipseItem->setFlag(QGraphicsItem::ItemIsMovable, true);
    return ellipseItem;
}

QGraphicsPolygonItem* CanvasArea::createRegularPolygon(const QPointF& center, qreal radius, int sides)
{
    if (sides < 3 || radius <= 0) {
        return nullptr;
    }

    QPolygonF polygon;
    const qreal angleStep = 2 * M_PI / sides;

    // Start from the top (90 degrees or PI/2)
    qreal startAngle = M_PI / 2;

    for (int i = 0; i < sides; ++i) {
        qreal angle = startAngle - i * angleStep;
        qreal x = center.x() + radius * cos(angle);
        qreal y = center.y() - radius * sin(angle);
        polygon << QPointF(x, y);
    }

    QGraphicsPolygonItem* polygonItem = new QGraphicsPolygonItem(polygon);
    polygonItem->setPen(m_defaultPen);
    polygonItem->setBrush(m_defaultBrush);
    polygonItem->setFlag(QGraphicsItem::ItemIsSelectable, true);
    polygonItem->setFlag(QGraphicsItem::ItemIsMovable, true);
    return polygonItem;
}

QGraphicsPolygonItem* CanvasArea::createStar(const QPointF& center, qreal outerRadius, qreal innerRadius)
{
    if (outerRadius <= 0 || innerRadius <= 0) {
        return nullptr;
    }

    const int points = 5;
    QPolygonF star;
    const qreal angleStep = M_PI / points;

    // Start from the top (90 degrees or PI/2)
    qreal startAngle = M_PI / 2;

    for (int i = 0; i < 2 * points; ++i) {
        qreal radius = (i % 2 == 0) ? outerRadius : innerRadius;
        qreal angle = startAngle - i * angleStep;
        qreal x = center.x() + radius * cos(angle);
        qreal y = center.y() - radius * sin(angle);
        star << QPointF(x, y);
    }

    QGraphicsPolygonItem* starItem = new QGraphicsPolygonItem(star);
    starItem->setPen(m_defaultPen);
    starItem->setBrush(m_defaultBrush);
    starItem->setFlag(QGraphicsItem::ItemIsSelectable, true);
    starItem->setFlag(QGraphicsItem::ItemIsMovable, true);
    return starItem;
}

void CanvasArea::setDragMode(bool enabled)
{
    if (enabled) {
        // Enable drag mode (pan/scroll)
        QGraphicsView::setDragMode(QGraphicsView::ScrollHandDrag);
        viewport()->setCursor(Qt::OpenHandCursor);
        m_currentShapeType = ShapeType::None;
        qCDebug(canvasAreaLog) << "Drag mode enabled";
    } else {
        // Disable drag mode (back to selection)
        QGraphicsView::setDragMode(QGraphicsView::RubberBandDrag);
        viewport()->setCursor(Qt::ArrowCursor);
        qCDebug(canvasAreaLog) << "Drag mode disabled";
    }
}

void CanvasArea::setShapeCreationMode(ShapeType type)
{
    m_currentShapeType = type;

    // Set appropriate cursor for shape creation
    switch (type) {
        case ShapeType::Line:
        case ShapeType::Rectangle:
        case ShapeType::Ellipse:
        case ShapeType::Pentagon:
        case ShapeType::Star:
        case ShapeType::Hexagon:
            viewport()->setCursor(Qt::CrossCursor);
            break;
        case ShapeType::Freehand:
            viewport()->setCursor(Qt::PointingHandCursor);
            break;
        default:
            viewport()->setCursor(Qt::ArrowCursor);
            break;
    }

    // Disable drag mode when in shape creation mode
    QGraphicsView::setDragMode(QGraphicsView::NoDrag);
    qCDebug(canvasAreaLog) << "Shape creation mode set to:" << static_cast<int>(type);
}

void CanvasArea::setSelectMode()
{
    m_currentShapeType = ShapeType::None;
    QGraphicsView::setDragMode(QGraphicsView::RubberBandDrag);
    viewport()->setCursor(Qt::ArrowCursor);
    qCDebug(canvasAreaLog) << "Select mode enabled";
}

void CanvasArea::addShapeToDocument(QGraphicsItem* item)
{
    if (!item || !m_currentEngine) {
        qCWarning(canvasAreaLog) << "Cannot add shape to document: item or engine is null";
        return;
    }

    // Add the item to the scene if it's not already there
    if (!item->scene()) {
        m_scene->addItem(item);
    }

    // Emit the shape created signal
    emit shapeCreated(item);
    qCDebug(canvasAreaLog) << "Shape added to document";
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

void CanvasArea::mousePressEvent(QMouseEvent *event)
{
    if (m_currentShapeType == ShapeType::None) {
        // If not in shape creation mode, use default behavior
        QGraphicsView::mousePressEvent(event);
        return;
    }

    if (event->button() == Qt::LeftButton) {
        // Start drawing
        m_isDrawing = true;
        m_startPoint = mapToScene(event->pos());
        m_endPoint = m_startPoint;

        if (m_currentShapeType == ShapeType::Freehand) {
            // For freehand drawing, start a new path
            m_freehandPoints.clear();
            m_freehandPoints.append(m_startPoint);

            // Create an initial path item
            m_currentItem = createFreehandPath(m_freehandPoints);
            m_scene->addItem(m_currentItem);
        } else {
            // For other shapes, create the initial shape
            createShape(m_startPoint, m_endPoint);
        }

        event->accept();
    } else {
        QGraphicsView::mousePressEvent(event);
    }
}

void CanvasArea::mouseMoveEvent(QMouseEvent *event)
{
    if (m_isDrawing && m_currentShapeType != ShapeType::None) {
        m_endPoint = mapToScene(event->pos());

        if (m_currentShapeType == ShapeType::Freehand) {
            // For freehand drawing, add the point to the path
            m_freehandPoints.append(m_endPoint);

            // Update the path
            if (m_currentItem) {
                m_scene->removeItem(m_currentItem);
                delete m_currentItem;
            }

            m_currentItem = createFreehandPath(m_freehandPoints);
            m_scene->addItem(m_currentItem);
        } else {
            // For other shapes, update the shape
            updateShape(m_endPoint);
        }

        event->accept();
    } else {
        QGraphicsView::mouseMoveEvent(event);
    }
}

void CanvasArea::mouseReleaseEvent(QMouseEvent *event)
{
    if (m_isDrawing && m_currentShapeType != ShapeType::None && event->button() == Qt::LeftButton) {
        m_endPoint = mapToScene(event->pos());

        // Finalize the shape
        finalizeShape();

        // Reset drawing state
        m_isDrawing = false;
        m_currentItem = nullptr;
        m_freehandPoints.clear();

        event->accept();
    } else {
        QGraphicsView::mouseReleaseEvent(event);
    }
}

bool CanvasArea::openFileWithEngine(CoreSvgEngine* engine) {
    qCDebug(canvasAreaLog) << "Opening SVG file with engine";
    QGraphicsScene *s = scene();

    // Store the current engine
    m_currentEngine = engine;

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
