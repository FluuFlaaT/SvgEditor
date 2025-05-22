#define MAX_N 25565
#define _USE_MATH_DEFINES
#include <cmath>
#include <QtMath>
#include "CanvasArea.h"
#include "../CoreSvgEngine/CoreSvgEngine.h"
#include "../CoreSvgEngine/SvgDocument.h"
#include "../CoreSvgEngine/SvgShapes.h"
#include "../CoreSvgEngine/SvgText.h"
#include "../Commands/AddShapeCommand.h"
#include "../Commands/RemoveShapeCommand.h"

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
    qCDebug(canvasAreaLog) << "Creating shape of type" << static_cast<int>(m_currentShapeType)
                          << "from" << startPoint << "to" << endPoint;

    if (m_currentShapeType != ShapeType::Text) {
        if (m_currentItem) {
            m_scene->removeItem(m_currentItem);
            delete m_currentItem;
            m_currentItem = nullptr;
        }
    } else {
        m_currentItem = nullptr;
    }

    switch (m_currentShapeType) {
        case ShapeType::Line:
            qCDebug(canvasAreaLog) << "Creating Line";
            m_currentItem = createLine(startPoint, endPoint);
            break;
        case ShapeType::Rectangle:
            qCDebug(canvasAreaLog) << "Creating Rectangle";
            m_currentItem = createRectangle(startPoint, endPoint);
            break;
        case ShapeType::Ellipse:
            qCDebug(canvasAreaLog) << "Creating Ellipse";
            m_currentItem = createEllipse(startPoint, endPoint);
            break;
        case ShapeType::Pentagon: {
            qCDebug(canvasAreaLog) << "Creating Pentagon";
            QPointF center = QPointF((startPoint.x() + endPoint.x()) / 2, (startPoint.y() + endPoint.y()) / 2);
            qreal radius = QLineF(center, endPoint).length();

            if (radius < 10.0) {
                radius = 10.0;
                qCDebug(canvasAreaLog) << "  Using minimum radius of 10.0";
            }

            qCDebug(canvasAreaLog) << "  Center:" << center << "Radius:" << radius;
            m_currentItem = createRegularPolygon(center, radius, 5);
            break;
        }
        case ShapeType::Star: {
            qCDebug(canvasAreaLog) << "Creating Star";
            QPointF center = QPointF((startPoint.x() + endPoint.x()) / 2, (startPoint.y() + endPoint.y()) / 2);
            qreal outerRadius = QLineF(center, endPoint).length();

            // Ensure a minimum outer radius of 20 pixels
            if (outerRadius < 20.0) {
                outerRadius = 20.0;
                qCDebug(canvasAreaLog) << "  Using minimum outer radius of 20.0";
            }

            qreal innerRadius = outerRadius * 0.4; // Inner radius is 40% of outer radius
            qCDebug(canvasAreaLog) << "  Center:" << center << "Outer Radius:" << outerRadius << "Inner Radius:" << innerRadius;
            m_currentItem = createStar(center, outerRadius, innerRadius);
            break;
        }
        case ShapeType::Hexagon: {
            qCDebug(canvasAreaLog) << "Creating Hexagon";
            QPointF center = QPointF((startPoint.x() + endPoint.x()) / 2, (startPoint.y() + endPoint.y()) / 2);
            qreal radius = QLineF(center, endPoint).length();

            // Ensure a minimum radius of 10 pixels
            if (radius < 10.0) {
                radius = 10.0;
                qCDebug(canvasAreaLog) << "  Using minimum radius of 10.0";
            }

            qCDebug(canvasAreaLog) << "  Center:" << center << "Radius:" << radius;
            m_currentItem = createRegularPolygon(center, radius, 6);
            break;
        }
        case ShapeType::Text: {
            qCDebug(canvasAreaLog) << "Creating Text";
            m_currentItem = createText(startPoint);
            break;
        }
        default:
            qCWarning(canvasAreaLog) << "Unknown shape type:" << static_cast<int>(m_currentShapeType);
            break;
    }

    if (m_currentItem) {
        m_scene->addItem(m_currentItem);
        qCDebug(canvasAreaLog) << "Added item to scene:" << m_currentItem;
    } else {
        qCWarning(canvasAreaLog) << "Failed to create item for shape type:" << static_cast<int>(m_currentShapeType);
    }
}

void CanvasArea::updateShape(const QPointF& endPoint)
{
    if (!m_currentItem) {
        qCWarning(canvasAreaLog) << "Cannot update shape: current item is null";
        return;
    }

    if (m_currentShapeType == ShapeType::Text) {
        return;
    }

    qCDebug(canvasAreaLog) << "Updating shape from" << m_startPoint << "to" << endPoint;

    m_scene->removeItem(m_currentItem);
    createShape(m_startPoint, endPoint);
}

void CanvasArea::finalizeShape()
{
    // Finalize the current shape
    if (!m_currentItem) {
        qCWarning(canvasAreaLog) << "Cannot finalize shape: current item is null";
        return;
    }

    qCDebug(canvasAreaLog) << "Finalizing shape of type" << static_cast<int>(m_currentShapeType);

    // Store a reference to the current item
    QGraphicsItem* finalizedItem = m_currentItem;

    // Create and execute an AddShapeCommand
    auto command = std::make_unique<AddShapeCommand>(this, m_currentItem, m_currentShapeType);
    CommandManager::instance()->executeCommand(std::move(command));

    // For text items, ensure they're selected so the properties panel updates
    if (m_currentShapeType == ShapeType::Text) {
        // Clear any existing selection
        m_scene->clearSelection();
        // Select the newly created text item
        finalizedItem->setSelected(true);
        // Emit the selection signal
        emit itemSelected(finalizedItem, ShapeType::Text);
        qCDebug(canvasAreaLog) << "Text item selected after creation";
    }

    // Reset the current item pointer (ownership transferred to the command)
    m_currentItem = nullptr;

    qCDebug(canvasAreaLog) << "Shape finalized and added to document via command";
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
        qCWarning(canvasAreaLog) << "Invalid parameters for regular polygon: sides=" << sides << ", radius=" << radius;
        return nullptr;
    }

    qCDebug(canvasAreaLog) << "Creating regular polygon with" << sides << "sides at" << center << "with radius" << radius;

    QPolygonF polygon;
    const qreal angleStep = 2.0 * M_PI / sides;

    // Start from the top (90 degrees or PI/2)
    qreal startAngle = M_PI / 2.0;

    for (int i = 0; i < sides; ++i) {
        qreal angle = startAngle - i * angleStep;
        qreal x = center.x() + radius * qCos(angle);
        qreal y = center.y() - radius * qSin(angle); // Correct for Qt's coordinate system (y-axis down)
        polygon << QPointF(x, y);
        qCDebug(canvasAreaLog) << "  Point" << i << ":" << QPointF(x, y);
    }

    QGraphicsPolygonItem* polygonItem = new QGraphicsPolygonItem(polygon);
    polygonItem->setPen(m_defaultPen);
    polygonItem->setBrush(m_defaultBrush);
    polygonItem->setFlag(QGraphicsItem::ItemIsSelectable, true);
    polygonItem->setFlag(QGraphicsItem::ItemIsMovable, true);

    qCDebug(canvasAreaLog) << "Created regular polygon with" << sides << "sides, polygon has" << polygon.size() << "points";
    return polygonItem;
}

QGraphicsPolygonItem* CanvasArea::createStar(const QPointF& center, qreal outerRadius, qreal innerRadius)
{
    if (outerRadius <= 0 || innerRadius <= 0) {
        qCWarning(canvasAreaLog) << "Invalid parameters for star: outerRadius=" << outerRadius << ", innerRadius=" << innerRadius;
        return nullptr;
    }

    qCDebug(canvasAreaLog) << "Creating star at" << center << "with outer radius" << outerRadius << "and inner radius" << innerRadius;

    const int points = 5;
    QPolygonF star;
    const qreal angleStep = M_PI / points;

    // Start from the top (90 degrees or PI/2)
    qreal startAngle = M_PI / 2.0;

    for (int i = 0; i < 2 * points; ++i) {
        qreal radius = (i % 2 == 0) ? outerRadius : innerRadius;
        qreal angle = startAngle - i * angleStep;
        qreal x = center.x() + radius * qCos(angle);
        qreal y = center.y() - radius * qSin(angle); // Correct for Qt's coordinate system (y-axis down)
        star << QPointF(x, y);
        qCDebug(canvasAreaLog) << "  Point" << i << ":" << QPointF(x, y) << "(radius=" << radius << ")";
    }

    QGraphicsPolygonItem* starItem = new QGraphicsPolygonItem(star);
    starItem->setPen(m_defaultPen);
    starItem->setBrush(m_defaultBrush);
    starItem->setFlag(QGraphicsItem::ItemIsSelectable, true);
    starItem->setFlag(QGraphicsItem::ItemIsMovable, true);

    qCDebug(canvasAreaLog) << "Created star with" << points << "points, polygon has" << star.size() << "vertices";
    return starItem;
}

void CanvasArea::setDragMode(QGraphicsView::DragMode mode)
{
    QGraphicsView::setDragMode(mode);

    if (mode == QGraphicsView::ScrollHandDrag) {
        viewport()->setCursor(Qt::OpenHandCursor);
        m_currentShapeType = ShapeType::None;
        qCDebug(canvasAreaLog) << "Drag mode set to ScrollHandDrag";
    } else if (mode == QGraphicsView::RubberBandDrag) {
        viewport()->setCursor(Qt::ArrowCursor);
        qCDebug(canvasAreaLog) << "Drag mode set to RubberBandDrag";
    } else {
        viewport()->setCursor(Qt::ArrowCursor);
        qCDebug(canvasAreaLog) << "Drag mode set to NoDrag";
    }
}

void CanvasArea::setShapeCreationMode(ShapeType type)
{
    m_currentShapeType = type;

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
        case ShapeType::Text:
            viewport()->setCursor(Qt::IBeamCursor);
            break;
        default:
            viewport()->setCursor(Qt::ArrowCursor);
            break;
    }

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

    // Get the current document
    SvgDocument* doc = m_currentEngine->getCurrentDocument();
    if (!doc) {
        qCWarning(canvasAreaLog) << "Cannot add shape to document: document is null";
        return;
    }

    // Check if the item is already in the document's graphics items list
    if (doc->m_graphicsItems.contains(item)) {
        qCDebug(canvasAreaLog) << "Item is already in document, skipping addition";
        return;
    }

    // Convert the QGraphicsItem to an SvgElement based on its type
    if (QGraphicsLineItem* lineItem = dynamic_cast<QGraphicsLineItem*>(item)) {
        // Create an SvgLine element
        QLineF line = lineItem->line();
        Point p1 = {line.x1(), line.y1()};
        Point p2 = {line.x2(), line.y2()};

        auto svgLine = std::make_unique<SvgLine>(p1, p2);

        // Set style properties
        QPen pen = lineItem->pen();
        svgLine->setStrokeColor({pen.color().red(), pen.color().green(), pen.color().blue(), pen.color().alpha()});
        svgLine->setStrokeWidth(pen.width());
        svgLine->setOpacity(item->opacity());

        // Add the element to the document
        doc->addElement(std::move(svgLine));

        // Add the graphics item to the document's graphics items list
        doc->m_graphicsItems.push_back(item);

        qCDebug(canvasAreaLog) << "Added line to document";
    }
    else if (QGraphicsRectItem* rectItem = dynamic_cast<QGraphicsRectItem*>(item)) {
        // Create an SvgRectangle element
        QRectF rect = rectItem->rect();
        Point topLeft = {rect.x(), rect.y()};

        auto svgRect = std::make_unique<SvgRectangle>(topLeft, rect.width(), rect.height());

        // Set style properties
        QPen pen = rectItem->pen();
        QBrush brush = rectItem->brush();
        svgRect->setStrokeColor({pen.color().red(), pen.color().green(), pen.color().blue(), pen.color().alpha()});
        svgRect->setFillColor({brush.color().red(), brush.color().green(), brush.color().blue(), brush.color().alpha()});
        svgRect->setStrokeWidth(pen.width());
        svgRect->setOpacity(item->opacity());

        // Add the element to the document
        doc->addElement(std::move(svgRect));

        // Add the graphics item to the document's graphics items list
        doc->m_graphicsItems.push_back(item);

        qCDebug(canvasAreaLog) << "Added rectangle to document";
    }
    else if (QGraphicsEllipseItem* ellipseItem = dynamic_cast<QGraphicsEllipseItem*>(item)) {
        // Create an SvgEllipse element
        QRectF rect = ellipseItem->rect();
        Point center = {rect.x() + rect.width()/2, rect.y() + rect.height()/2};

        // Check if it's a circle (equal width and height)
        if (qFuzzyCompare(rect.width(), rect.height())) {
            auto svgCircle = std::make_unique<SvgCircle>(center, rect.width()/2);

            // Set style properties
            QPen pen = ellipseItem->pen();
            QBrush brush = ellipseItem->brush();
            svgCircle->setStrokeColor(Color(pen.color().red(), pen.color().green(), pen.color().blue(), pen.color().alpha()));
            svgCircle->setFillColor(Color(brush.color().red(), brush.color().green(), brush.color().blue(), brush.color().alpha()));
            svgCircle->setStrokeWidth(pen.width());
            svgCircle->setOpacity(item->opacity());

            // Add the element to the document
            doc->addElement(std::move(svgCircle));
        } else {
            auto svgEllipse = std::make_unique<SvgEllipse>(center, rect.width()/2, rect.height()/2);

            // Set style properties
            QPen pen = ellipseItem->pen();
            QBrush brush = ellipseItem->brush();
            svgEllipse->setStrokeColor(Color(pen.color().red(), pen.color().green(), pen.color().blue(), pen.color().alpha()));
            svgEllipse->setFillColor(Color(brush.color().red(), brush.color().green(), brush.color().blue(), brush.color().alpha()));
            svgEllipse->setStrokeWidth(pen.width());
            svgEllipse->setOpacity(item->opacity());

            // Add the element to the document
            doc->addElement(std::move(svgEllipse));
        }

        // Add the graphics item to the document's graphics items list
        doc->m_graphicsItems.push_back(item);

        qCDebug(canvasAreaLog) << "Added ellipse to document";
    }
    else if (QGraphicsPolygonItem* polygonItem = dynamic_cast<QGraphicsPolygonItem*>(item)) {
        QPolygonF polygon = polygonItem->polygon();

        // Convert QPolygonF to vector of Points
        std::vector<Point> points;
        for (const QPointF& point : polygon) {
            points.push_back({point.x(), point.y()});
        }

        // Determine if it's a regular polygon (pentagon, hexagon) or a star
        if (m_currentShapeType == ShapeType::Pentagon) {
            // Calculate center and radius
            QPointF center = polygon.boundingRect().center();
            qreal radius = QLineF(center, polygon.at(0)).length();

            auto svgPentagon = std::make_unique<SvgPentagon>(Point{center.x(), center.y()}, radius);

            // Set style properties
            QPen pen = polygonItem->pen();
            QBrush brush = polygonItem->brush();
            svgPentagon->setStrokeColor(Color(pen.color().red(), pen.color().green(), pen.color().blue(), pen.color().alpha()));
            svgPentagon->setFillColor(Color(brush.color().red(), brush.color().green(), brush.color().blue(), brush.color().alpha()));
            svgPentagon->setStrokeWidth(pen.width());
            svgPentagon->setOpacity(item->opacity());

            // Add the element to the document
            doc->addElement(std::move(svgPentagon));
        }
        else if (m_currentShapeType == ShapeType::Hexagon) {
            // Calculate center and radius
            QPointF center = polygon.boundingRect().center();
            qreal radius = QLineF(center, polygon.at(0)).length();

            auto svgHexagon = std::make_unique<SvgHexagon>(Point{center.x(), center.y()}, radius);

            // Set style properties
            QPen pen = polygonItem->pen();
            QBrush brush = polygonItem->brush();
            svgHexagon->setStrokeColor(Color(pen.color().red(), pen.color().green(), pen.color().blue(), pen.color().alpha()));
            svgHexagon->setFillColor(Color(brush.color().red(), brush.color().green(), brush.color().blue(), brush.color().alpha()));
            svgHexagon->setStrokeWidth(pen.width());
            svgHexagon->setOpacity(item->opacity());

            // Add the element to the document
            doc->addElement(std::move(svgHexagon));
        }
        else if (m_currentShapeType == ShapeType::Star) {
            // Calculate center and radii
            QPointF center = polygon.boundingRect().center();
            qreal outerRadius = 0;
            qreal innerRadius = 0;

            // For a star, alternate points are at outer and inner radii
            if (polygon.size() >= 10) { // 5-pointed star has 10 points
                outerRadius = QLineF(center, polygon.at(0)).length();
                innerRadius = QLineF(center, polygon.at(1)).length();
            }

            auto svgStar = std::make_unique<SvgStar>(Point{center.x(), center.y()}, outerRadius, innerRadius);

            // Set style properties
            QPen pen = polygonItem->pen();
            QBrush brush = polygonItem->brush();
            svgStar->setStrokeColor(Color(pen.color().red(), pen.color().green(), pen.color().blue(), pen.color().alpha()));
            svgStar->setFillColor(Color(brush.color().red(), brush.color().green(), brush.color().blue(), brush.color().alpha()));
            svgStar->setStrokeWidth(pen.width());
            svgStar->setOpacity(item->opacity());

            // Add the element to the document
            doc->addElement(std::move(svgStar));
        }
        else {
            // Generic polygon
            auto svgPolygon = std::make_unique<SvgPolygon>(points);

            // Set style properties
            QPen pen = polygonItem->pen();
            QBrush brush = polygonItem->brush();
            svgPolygon->setStrokeColor(Color(pen.color().red(), pen.color().green(), pen.color().blue(), pen.color().alpha()));
            svgPolygon->setFillColor(Color(brush.color().red(), brush.color().green(), brush.color().blue(), brush.color().alpha()));
            svgPolygon->setStrokeWidth(pen.width());
            svgPolygon->setOpacity(item->opacity());

            // Add the element to the document
            doc->addElement(std::move(svgPolygon));
        }

        // Add the graphics item to the document's graphics items list
        doc->m_graphicsItems.push_back(item);

        qCDebug(canvasAreaLog) << "Added polygon to document";
    }
    else if (QGraphicsPathItem* pathItem = dynamic_cast<QGraphicsPathItem*>(item)) {
        // For freehand drawing, convert to polyline
        QPainterPath path = pathItem->path();
        std::vector<Point> points;

        // Extract points from the path
        for (int i = 0; i < path.elementCount(); ++i) {
            QPainterPath::Element element = path.elementAt(i);
            points.push_back({element.x, element.y});
        }

        auto svgPolyline = std::make_unique<SvgPolyline>(points);

        // Set style properties
        QPen pen = pathItem->pen();
        svgPolyline->setStrokeColor(Color(pen.color().red(), pen.color().green(), pen.color().blue(), pen.color().alpha()));
        svgPolyline->setStrokeWidth(pen.width());
        svgPolyline->setOpacity(item->opacity());

        // Add the element to the document
        doc->addElement(std::move(svgPolyline));

        // Add the graphics item to the document's graphics items list
        doc->m_graphicsItems.push_back(item);

        qCDebug(canvasAreaLog) << "Added path to document";
    }
    else if (EditableTextItem* textItem = dynamic_cast<EditableTextItem*>(item)) {
        // Create an SvgText element
        QPointF pos = textItem->pos();
        QString text = textItem->toPlainString();

        auto svgText = std::make_unique<SvgText>(Point{pos.x(), pos.y()}, text.toStdString());

        // Set font properties
        QFont font = textItem->font();
        svgText->setFontFamily(font.family().toStdString());
        svgText->setFontSize(font.pointSizeF());
        svgText->setBold(textItem->isBold());
        svgText->setItalic(textItem->isItalic());

        // Set text alignment
        Qt::Alignment alignment = textItem->textAlignment();
        if (alignment & Qt::AlignCenter) {
            svgText->setTextAnchor(TextAnchor::Middle);
        } else if (alignment & Qt::AlignRight) {
            svgText->setTextAnchor(TextAnchor::End);
        } else {
            svgText->setTextAnchor(TextAnchor::Start);
        }

        // Set color
        QColor color = textItem->defaultTextColor();
        svgText->setFillColor(Color(color.red(), color.green(), color.blue(), color.alpha()));
        svgText->setOpacity(item->opacity());

        // Add the element to the document
        doc->addElement(std::move(svgText));

        // Add the graphics item to the document's graphics items list
        doc->m_graphicsItems.push_back(item);

        qCDebug(canvasAreaLog) << "Added text to document";
    }
    // For backward compatibility, also handle QGraphicsSimpleTextItem
    else if (QGraphicsSimpleTextItem* textItem = dynamic_cast<QGraphicsSimpleTextItem*>(item)) {
        // Create an SvgText element
        QPointF pos = textItem->pos();
        QString text = textItem->text();

        auto svgText = std::make_unique<SvgText>(Point{pos.x(), pos.y()}, text.toStdString());

        // Set font properties
        QFont font = textItem->font();
        svgText->setFontFamily(font.family().toStdString());
        svgText->setFontSize(font.pointSizeF());
        svgText->setBold(font.bold());
        svgText->setItalic(font.italic());

        // Set color
        QBrush brush = textItem->brush();
        QColor color = brush.color();
        svgText->setFillColor(Color(color.red(), color.green(), color.blue(), color.alpha()));
        svgText->setOpacity(item->opacity());

        // Add the element to the document
        doc->addElement(std::move(svgText));

        // Add the graphics item to the document's graphics items list
        doc->m_graphicsItems.push_back(item);

        qCDebug(canvasAreaLog) << "Added simple text to document";
    }

    // Emit the shape created signal
    emit shapeCreated(item);
    qCDebug(canvasAreaLog) << "Shape added to document";
}



void CanvasArea::mousePressEvent(QMouseEvent *event)
{
    if (m_currentShapeType == ShapeType::None) {
        // If not in shape creation mode, use default behavior
        QGraphicsView::mousePressEvent(event);

        // Check if an item was selected after the mouse press
        if (event->button() == Qt::LeftButton) {
            QList<QGraphicsItem*> selectedItems = m_scene->selectedItems();
            if (!selectedItems.isEmpty()) {
                QGraphicsItem* selectedItem = selectedItems.first();
                ShapeType itemType = getItemType(selectedItem);

                // Emit the selection signal
                emit itemSelected(selectedItem, itemType);
                qCDebug(canvasAreaLog) << "Item selected on mouse press, type:" << static_cast<int>(itemType);
            }
        }

        return;
    }

    if (event->button() == Qt::LeftButton) {
        // Start drawing
        m_isDrawing = true;
        m_startPoint = mapToScene(event->pos());

        // For polygon shapes, add a small offset to ensure we don't have zero radius
        // when the user just clicks without dragging
        if (m_currentShapeType == ShapeType::Pentagon ||
            m_currentShapeType == ShapeType::Star ||
            m_currentShapeType == ShapeType::Hexagon) {
            // Set end point slightly offset from start point
            m_endPoint = QPointF(m_startPoint.x() + 5, m_startPoint.y() + 5);
            qCDebug(canvasAreaLog) << "Using offset end point for polygon shape:" << m_endPoint;
        } else {
            m_endPoint = m_startPoint;
        }

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
        } else if (m_currentShapeType != ShapeType::Text) {
            // For other shapes (except Text), update the shape
            updateShape(m_endPoint);
        }
        // For Text, we don't update during mouse movement to avoid dialog popup

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

        // Check if an item was selected after the mouse release
        if (event->button() == Qt::LeftButton && m_currentShapeType == ShapeType::None) {
            QList<QGraphicsItem*> selectedItems = m_scene->selectedItems();
            if (!selectedItems.isEmpty()) {
                QGraphicsItem* selectedItem = selectedItems.first();
                ShapeType itemType = getItemType(selectedItem);

                // Emit the selection signal
                emit itemSelected(selectedItem, itemType);
                qCDebug(canvasAreaLog) << "Item selected on mouse release, type:" << static_cast<int>(itemType);
            }
        }
    }
}

QGraphicsItem* CanvasArea::getSelectedItem() const
{
    QList<QGraphicsItem*> selectedItems = m_scene->selectedItems();
    if (!selectedItems.isEmpty()) {
        return selectedItems.first();
    }
    return nullptr;
}

ShapeType CanvasArea::getItemType(QGraphicsItem* item) const
{
    if (!item) {
        return ShapeType::None;
    }

    // Determine the type of the item
    if (dynamic_cast<QGraphicsLineItem*>(item)) {
        return ShapeType::Line;
    } else if (dynamic_cast<QGraphicsRectItem*>(item) &&
               !dynamic_cast<QGraphicsEllipseItem*>(item)) {
        return ShapeType::Rectangle;
    } else if (dynamic_cast<QGraphicsEllipseItem*>(item)) {
        return ShapeType::Ellipse;
    } else if (dynamic_cast<QGraphicsPolygonItem*>(item)) {
        // For polygon items, we need to determine if it's a pentagon, hexagon, or star
        QGraphicsPolygonItem* polygonItem = dynamic_cast<QGraphicsPolygonItem*>(item);
        int pointCount = polygonItem->polygon().size();
        if (pointCount == 5) {
            return ShapeType::Pentagon;
        } else if (pointCount == 6) {
            return ShapeType::Hexagon;
        } else if (pointCount == 10) {
            return ShapeType::Star;
        }
    } else if (dynamic_cast<QGraphicsPathItem*>(item)) {
        return ShapeType::Freehand;
    } else if (dynamic_cast<EditableTextItem*>(item)) {
        return ShapeType::Text;
    } else if (dynamic_cast<QGraphicsSimpleTextItem*>(item)) {
        return ShapeType::Text;
    }

    return ShapeType::None;
}

ShapeType CanvasArea::getSelectedItemType() const
{
    QGraphicsItem* selectedItem = getSelectedItem();
    return getItemType(selectedItem);
}

EditableTextItem* CanvasArea::createText(const QPointF& position, const QString& text) {
    // If text is empty, prompt the user for text input
    QString textContent = text;
    if (textContent.isEmpty()) {
        bool ok;
        textContent = QInputDialog::getText(this, tr("Enter Text"),
                                         tr("Text content:"), QLineEdit::Normal,
                                         tr("Text"), &ok);
        if (!ok || textContent.isEmpty()) {
            textContent = tr("Text"); // Default text if user cancels or enters empty text
        }
    }

    // Create the editable text item
    EditableTextItem* textItem = new EditableTextItem(textContent);
    textItem->setPos(position);

    // Connect signals
    connect(textItem, &EditableTextItem::textChanged, this, [this, textItem](const QString& newText) {
        // When text changes, update the document
        if (m_currentEngine && m_currentEngine->getCurrentDocument()) {
            // The document will be marked as modified when the text is finalized
            qCDebug(canvasAreaLog) << "Text changed to:" << newText;
        }
    });

    return textItem;
}

void CanvasArea::keyPressEvent(QKeyEvent *event)
{
    // Check if Delete key was pressed
    if (event->key() == Qt::Key_Delete) {
        deleteSelectedItem();
        event->accept();
    } else {
        // Pass the event to the parent class
        QGraphicsView::keyPressEvent(event);
    }
}

void CanvasArea::contextMenuEvent(QContextMenuEvent *event)
{
    // Get the item under the cursor
    QPointF scenePos = mapToScene(event->pos());
    QGraphicsItem* itemUnderCursor = scene()->itemAt(scenePos, transform());

    // Create the context menu
    QMenu contextMenu(this);

    // If there's an item under the cursor, add a delete option
    if (itemUnderCursor && itemUnderCursor != m_backgroundItem && itemUnderCursor != m_outlineItem) {
        // Select the item under cursor if it's not already selected
        if (!itemUnderCursor->isSelected()) {
            scene()->clearSelection();
            itemUnderCursor->setSelected(true);

            // Emit the selection signal
            ShapeType itemType = getItemType(itemUnderCursor);
            emit itemSelected(itemUnderCursor, itemType);
            qCDebug(canvasAreaLog) << "Item selected via context menu, type:" << static_cast<int>(itemType);
        }

        // Add delete action
        QAction* deleteAction = contextMenu.addAction(tr("Delete"));
        connect(deleteAction, &QAction::triggered, this, &CanvasArea::deleteSelectedItem);

        // Show the context menu
        contextMenu.exec(event->globalPos());
        event->accept();
    } else {
        // If no item under cursor, pass the event to the parent class
        QGraphicsView::contextMenuEvent(event);
    }
}

void CanvasArea::deleteSelectedItem()
{
    // Get the currently selected item
    QGraphicsItem* selectedItem = getSelectedItem();

    if (selectedItem && selectedItem != m_backgroundItem && selectedItem != m_outlineItem) {
        // Get the type of the selected item
        ShapeType itemType = getItemType(selectedItem);

        qCDebug(canvasAreaLog) << "Deleting selected item of type:" << static_cast<int>(itemType);

        // Create and execute a RemoveShapeCommand
        auto command = std::make_unique<RemoveShapeCommand>(this, selectedItem, itemType);
        CommandManager::instance()->executeCommand(std::move(command));

        qCDebug(canvasAreaLog) << "Item deleted via command";
    } else {
        qCDebug(canvasAreaLog) << "No item selected for deletion";
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
            // Set the item to be selectable
            item->setFlag(QGraphicsItem::ItemIsSelectable, true);
            // Also make it movable when selected
            item->setFlag(QGraphicsItem::ItemIsMovable, true);
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
