#define MAX_N 25565
#define _USE_MATH_DEFINES
#include <cmath>
#include <QtMath>
#include <QTimer>
#include "canvasarea.h"
#include "../CoreSvgEngine/coresvgengine.h"
#include "../CoreSvgEngine/svgdocument.h"
#include "../CoreSvgEngine/svgshapes.h"
#include "../SvgText/SvgText.h"
#include "../Commands/AddShapeCommand.h"
#include "../Commands/RemoveShapeCommand.h"
#include "../Commands/ModifyTextCommand.h"
#include "QGraphicsItemAdapter.h"
#include "Core/ShapeFactory.h"

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

    // Initialize shape factory
    m_shapeFactory = std::make_unique<ShapeFactory>();
    m_shapeFactory->setDefaultPen(m_defaultPen);
    m_shapeFactory->setDefaultBrush(m_defaultBrush);
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
            // Preview rectangle helps users visualize text box boundaries before typing
            QRectF textRect(qMin(startPoint.x(), endPoint.x()),
                           qMin(startPoint.y(), endPoint.y()),
                           qAbs(endPoint.x() - startPoint.x()),
                           qAbs(endPoint.y() - startPoint.y()));
            
            // Minimum size ensures text remains readable and clickable
            if (textRect.width() < 50) textRect.setWidth(50);
            if (textRect.height() < 20) textRect.setHeight(20);
            
            m_textPreviewRect = textRect;
            
            // Dashed outline distinguishes preview from final text elements
            QGraphicsRectItem* previewRect = new QGraphicsRectItem(textRect);
            QPen dashedPen(Qt::gray, 2, Qt::DashLine);
            previewRect->setPen(dashedPen);
            previewRect->setBrush(Qt::NoBrush); // No fill, just outline
            
            m_currentItem = previewRect;
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

    qCDebug(canvasAreaLog) << "Updating shape from" << m_startPoint << "to" << endPoint;

    m_scene->removeItem(m_currentItem);
    createShape(m_startPoint, endPoint);
}

void CanvasArea::finalizeShape()
{
    if (!m_currentItem) {
        qCWarning(canvasAreaLog) << "Cannot finalize shape: current item is null";
        return;
    }

    qCDebug(canvasAreaLog) << "Finalizing shape of type" << static_cast<int>(m_currentShapeType);

    // Text elements require preview-to-editable conversion for proper interaction
    if (m_currentShapeType == ShapeType::Text) {
        m_scene->removeItem(m_currentItem);
        delete m_currentItem;
        
        // Actual text item enables in-place editing and proper text rendering
        EditableTextItem* textItem = createTextBox(m_textPreviewRect);
        m_currentItem = textItem;
        m_scene->addItem(m_currentItem);
        
        qCDebug(canvasAreaLog) << "Replaced preview rectangle with text item";
    }

    QGraphicsItem* finalizedItem = m_currentItem;

    // Command pattern enables undo/redo functionality for all shape operations
    auto command = std::make_unique<AddShapeCommand>(this, m_currentItem, m_currentShapeType);
    CommandManager::instance()->executeCommand(std::move(command));

    // Immediate editing improves user workflow for text creation
    if (m_currentShapeType == ShapeType::Text) {
        m_scene->clearSelection();
        finalizedItem->setSelected(true);
        emit itemSelected(finalizedItem, ShapeType::Text);
        
        // Slight delay ensures scene is fully updated before editing starts
        if (auto textItem = dynamic_cast<EditableTextItem*>(finalizedItem)) {
            QTimer::singleShot(50, [textItem]() {
                textItem->startEditing();
            });
        }
        
        qCDebug(canvasAreaLog) << "Text item selected and editing started after creation";
    }

    // Command now owns the item, preventing double-deletion
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

    SvgDocument* doc = m_currentEngine->getCurrentDocument();
    if (!doc) {
        qCWarning(canvasAreaLog) << "Cannot add shape to document: document is null";
        return;
    }

    if (doc->m_graphicsItems.contains(item)) {
        qCDebug(canvasAreaLog) << "Item is already in document, skipping addition";
        return;
    }

    QGraphicsItemAdapter a(item);
    auto toColor = [](const QColor& c) { return Color{c.red(), c.green(), c.blue(), c.alpha()}; };
    auto applyStyle = [&](SvgElement* e) {
        if (a.hasStroke()) {
            e->setStrokeColor(toColor(a.strokeColor()));
            e->setStrokeWidth(a.strokeWidth());
        }
        if (a.hasFill()) {
            e->setFillColor(toColor(a.fillColor()));
        }
        e->setOpacity(a.opacity());
    };

    std::unique_ptr<SvgElement> svgElement;

    switch (a.shapeType()) {
    case ShapeType::Line: {
        if (auto li = qgraphicsitem_cast<QGraphicsLineItem*>(item)) {
            QLineF line = li->line();
            svgElement = std::make_unique<SvgLine>(Point{line.x1(), line.y1()}, Point{line.x2(), line.y2()});
        }
        break;
    }
    case ShapeType::Rectangle: {
        if (auto ri = qgraphicsitem_cast<QGraphicsRectItem*>(item)) {
            QRectF r = ri->rect();
            svgElement = std::make_unique<SvgRectangle>(Point{r.x(), r.y()}, r.width(), r.height());
        }
        break;
    }
    case ShapeType::Ellipse: {
        if (auto ei = qgraphicsitem_cast<QGraphicsEllipseItem*>(item)) {
            QRectF r = ei->rect();
            Point center{r.x() + r.width()/2, r.y() + r.height()/2};
            if (qFuzzyCompare(r.width(), r.height()))
                svgElement = std::make_unique<SvgCircle>(center, r.width()/2);
            else
                svgElement = std::make_unique<SvgEllipse>(center, r.width()/2, r.height()/2);
        }
        break;
    }
    case ShapeType::Pentagon:
    case ShapeType::Hexagon:
    case ShapeType::Star: {
        if (auto pi = qgraphicsitem_cast<QGraphicsPolygonItem*>(item)) {
            QPolygonF polygon = pi->polygon();
            QPointF center = polygon.boundingRect().center();
            if (a.shapeType() == ShapeType::Pentagon) {
                svgElement = std::make_unique<SvgPentagon>(Point{center.x(), center.y()},
                    QLineF(center, polygon.at(0)).length());
            } else if (a.shapeType() == ShapeType::Hexagon) {
                svgElement = std::make_unique<SvgHexagon>(Point{center.x(), center.y()},
                    QLineF(center, polygon.at(0)).length());
            } else if (polygon.size() >= 10) {
                svgElement = std::make_unique<SvgStar>(Point{center.x(), center.y()},
                    QLineF(center, polygon.at(0)).length(),
                    QLineF(center, polygon.at(1)).length());
            }
        }
        break;
    }
    case ShapeType::Freehand: {
        if (auto pi = qgraphicsitem_cast<QGraphicsPathItem*>(item)) {
            std::vector<Point> pts;
            QPainterPath path = pi->path();
            for (int i = 0; i < path.elementCount(); ++i)
                pts.push_back({path.elementAt(i).x, path.elementAt(i).y});
            svgElement = std::make_unique<SvgPolyline>(pts);
        }
        break;
    }
    case ShapeType::Text: {
        QPointF pos = item->pos();
        auto svgText = std::make_unique<SvgText>(Point{pos.x(), pos.y()}, a.textContent().toStdString());
        svgText->setFontFamily(a.textFont().family().toStdString());
        svgText->setFontSize(a.textFont().pointSizeF());
        svgText->setBold(a.isBold());
        svgText->setItalic(a.isItalic());
        Qt::Alignment alignment = a.textAlignment();
        if (alignment & Qt::AlignCenter)
            svgText->setTextAnchor(TextAnchor::Middle);
        else if (alignment & Qt::AlignRight)
            svgText->setTextAnchor(TextAnchor::End);
        else
            svgText->setTextAnchor(TextAnchor::Start);
        svgText->setFillColor(toColor(a.textColor()));
        svgText->setOpacity(a.opacity());
        svgElement = std::move(svgText);
        break;
    }
    default:
        qCWarning(canvasAreaLog) << "Unknown shape type, cannot add to document";
        return;
    }

    if (svgElement) {
        applyStyle(svgElement.get());
        doc->addElement(std::move(svgElement));
        doc->m_graphicsItems.push_back(item);
        emit shapeCreated(item);
        qCDebug(canvasAreaLog) << "Shape added to document";
    }
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
        } else {
            // For all other shapes including Text, update the shape
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
    return QGraphicsItemAdapter::resolveShapeType(item);
}

ShapeType CanvasArea::getSelectedItemType() const
{
    QGraphicsItem* selectedItem = getSelectedItem();
    return getItemType(selectedItem);
}

EditableTextItem* CanvasArea::createTextBox(const QRectF& textRect) {
    // Create the editable text item with placeholder text
    EditableTextItem* textItem = new EditableTextItem(tr("Type"));
    textItem->setPos(textRect.topLeft());
    
    // Set the text box size to match the dragged area
    textItem->setTextWidth(textRect.width());
    
    // Set default font size based on the height of the text box
    QFont font = textItem->font();
    double fontSize = qMax(10.0, qMin(textRect.height() * 0.6, 72.0));
    font.setPointSizeF(fontSize);
    textItem->setFont(font);

    // Connect signals
    connect(textItem, &EditableTextItem::textChanged, this, [this, textItem](const QString& newText) {
        // When text changes, update the document
        if (m_currentEngine && m_currentEngine->getCurrentDocument()) {
            // The document will be marked as modified when the text is finalized
            qCDebug(canvasAreaLog) << "Text changed to:" << newText;
        }
    });
    
    // Connect the history signal to create undo/redo commands
    connect(textItem, &EditableTextItem::textChangedWithHistory, this, [this](const QString& oldText, const QString& newText) {
        if (EditableTextItem* textItem = qobject_cast<EditableTextItem*>(sender())) {
            // Create and execute the command for text content change
            auto command = std::make_unique<ModifyTextCommand>(textItem, oldText, newText);
            CommandManager::instance()->executeCommand(std::move(command));
            
            qCDebug(canvasAreaLog) << "Created ModifyTextCommand for text change:" << oldText << "->" << newText;
        }
    });

    return textItem;
}

EditableTextItem* CanvasArea::createText(const QPointF& position, const QString& text) {
    // Create a default sized text box at the clicked position for backward compatibility
    QRectF defaultRect(position.x(), position.y(), 100, 30);
    return createTextBox(defaultRect);
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



