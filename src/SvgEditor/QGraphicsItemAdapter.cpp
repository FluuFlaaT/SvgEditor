#include "QGraphicsItemAdapter.h"
#include "EditableTextItem.h"
#include <QGraphicsLineItem>
#include <QGraphicsRectItem>
#include <QGraphicsEllipseItem>
#include <QGraphicsPolygonItem>
#include <QGraphicsPathItem>
#include <QGraphicsSimpleTextItem>

QGraphicsItemAdapter::QGraphicsItemAdapter(QGraphicsItem* item)
    : m_item(item), m_type(resolveShapeType(item))
{
}

ShapeType QGraphicsItemAdapter::resolveShapeType(QGraphicsItem* item) {
    if (!item) return ShapeType::None;

    if (qgraphicsitem_cast<QGraphicsLineItem*>(item))
        return ShapeType::Line;
    if (qgraphicsitem_cast<QGraphicsEllipseItem*>(item))
        return ShapeType::Ellipse;
    if (auto rectItem = qgraphicsitem_cast<QGraphicsRectItem*>(item))
        return ShapeType::Rectangle;
    if (auto polygonItem = qgraphicsitem_cast<QGraphicsPolygonItem*>(item)) {
        int n = polygonItem->polygon().size();
        if (n == 5) return ShapeType::Pentagon;
        if (n == 6) return ShapeType::Hexagon;
        if (n == 10) return ShapeType::Star;
        return ShapeType::None;
    }
    if (qgraphicsitem_cast<QGraphicsPathItem*>(item))
        return ShapeType::Freehand;
    if (qgraphicsitem_cast<EditableTextItem*>(item))
        return ShapeType::Text;
    if (qgraphicsitem_cast<QGraphicsSimpleTextItem*>(item))
        return ShapeType::Text;

    return ShapeType::None;
}

bool QGraphicsItemAdapter::hasStroke() const {
    return m_type != ShapeType::None && m_type != ShapeType::Text;
}

bool QGraphicsItemAdapter::hasFill() const {
    switch (m_type) {
        case ShapeType::Rectangle:
        case ShapeType::Ellipse:
        case ShapeType::Pentagon:
        case ShapeType::Hexagon:
        case ShapeType::Star:
            return true;
        default:
            return false;
    }
}

bool QGraphicsItemAdapter::hasText() const {
    return m_type == ShapeType::Text;
}

QPen QGraphicsItemAdapter::getPen() const {
    if (auto p = qgraphicsitem_cast<QGraphicsLineItem*>(m_item)) return p->pen();
    if (auto p = qgraphicsitem_cast<QGraphicsRectItem*>(m_item)) return p->pen();
    if (auto p = qgraphicsitem_cast<QGraphicsEllipseItem*>(m_item)) return p->pen();
    if (auto p = qgraphicsitem_cast<QGraphicsPolygonItem*>(m_item)) return p->pen();
    if (auto p = qgraphicsitem_cast<QGraphicsPathItem*>(m_item)) return p->pen();
    return QPen();
}

void QGraphicsItemAdapter::setPen(const QPen& pen) {
    if (auto p = qgraphicsitem_cast<QGraphicsLineItem*>(m_item)) { p->setPen(pen); return; }
    if (auto p = qgraphicsitem_cast<QGraphicsRectItem*>(m_item)) { p->setPen(pen); return; }
    if (auto p = qgraphicsitem_cast<QGraphicsEllipseItem*>(m_item)) { p->setPen(pen); return; }
    if (auto p = qgraphicsitem_cast<QGraphicsPolygonItem*>(m_item)) { p->setPen(pen); return; }
    if (auto p = qgraphicsitem_cast<QGraphicsPathItem*>(m_item)) { p->setPen(pen); return; }
}

QBrush QGraphicsItemAdapter::getBrush() const {
    if (auto p = qgraphicsitem_cast<QGraphicsRectItem*>(m_item)) return p->brush();
    if (auto p = qgraphicsitem_cast<QGraphicsEllipseItem*>(m_item)) return p->brush();
    if (auto p = qgraphicsitem_cast<QGraphicsPolygonItem*>(m_item)) return p->brush();
    if (auto p = qgraphicsitem_cast<QGraphicsPathItem*>(m_item)) return p->brush();
    return QBrush();
}

void QGraphicsItemAdapter::setBrush(const QBrush& brush) {
    if (auto p = qgraphicsitem_cast<QGraphicsRectItem*>(m_item)) { p->setBrush(brush); return; }
    if (auto p = qgraphicsitem_cast<QGraphicsEllipseItem*>(m_item)) { p->setBrush(brush); return; }
    if (auto p = qgraphicsitem_cast<QGraphicsPolygonItem*>(m_item)) { p->setBrush(brush); return; }
    if (auto p = qgraphicsitem_cast<QGraphicsPathItem*>(m_item)) { p->setBrush(brush); return; }
}

EditableTextItem* QGraphicsItemAdapter::editableText() const {
    return qgraphicsitem_cast<EditableTextItem*>(m_item);
}

// --- Stroke ---

QColor QGraphicsItemAdapter::strokeColor() const {
    return getPen().color();
}

void QGraphicsItemAdapter::setStrokeColor(const QColor& color) {
    QPen pen = getPen();
    pen.setColor(color);
    setPen(pen);
}

int QGraphicsItemAdapter::strokeWidth() const {
    return getPen().width();
}

void QGraphicsItemAdapter::setStrokeWidth(int width) {
    QPen pen = getPen();
    pen.setWidth(width);
    setPen(pen);
}

Qt::PenStyle QGraphicsItemAdapter::strokeStyle() const {
    return getPen().style();
}

void QGraphicsItemAdapter::setStrokeStyle(Qt::PenStyle style) {
    QPen pen = getPen();
    pen.setStyle(style);
    setPen(pen);
}

// --- Fill ---

QColor QGraphicsItemAdapter::fillColor() const {
    return getBrush().color();
}

void QGraphicsItemAdapter::setFillColor(const QColor& color) {
    setBrush(QBrush(color));
}

// --- Opacity ---

qreal QGraphicsItemAdapter::opacity() const {
    return m_item->opacity();
}

void QGraphicsItemAdapter::setOpacity(qreal opacity) {
    m_item->setOpacity(opacity);
}

// --- Text ---

QString QGraphicsItemAdapter::textContent() const {
    if (auto et = editableText()) return et->toPlainString();
    if (auto st = qgraphicsitem_cast<QGraphicsSimpleTextItem*>(m_item)) return st->text();
    return {};
}

void QGraphicsItemAdapter::setTextContent(const QString& text) {
    if (auto et = editableText()) { et->setPlainText(text); return; }
    if (auto st = qgraphicsitem_cast<QGraphicsSimpleTextItem*>(m_item)) { st->setText(text); return; }
}

QFont QGraphicsItemAdapter::textFont() const {
    if (auto et = editableText()) return et->font();
    if (auto st = qgraphicsitem_cast<QGraphicsSimpleTextItem*>(m_item)) return st->font();
    return QFont();
}

void QGraphicsItemAdapter::setTextFont(const QFont& font) {
    if (auto et = editableText()) { et->setFont(font); return; }
    if (auto st = qgraphicsitem_cast<QGraphicsSimpleTextItem*>(m_item)) { st->setFont(font); return; }
}

bool QGraphicsItemAdapter::isBold() const {
    if (auto et = editableText()) return et->isBold();
    if (auto st = qgraphicsitem_cast<QGraphicsSimpleTextItem*>(m_item)) return st->font().bold();
    return false;
}

void QGraphicsItemAdapter::setBold(bool bold) {
    if (auto et = editableText()) { et->setBold(bold); return; }
    if (auto st = qgraphicsitem_cast<QGraphicsSimpleTextItem*>(m_item)) {
        QFont font = st->font();
        font.setBold(bold);
        st->setFont(font);
    }
}

bool QGraphicsItemAdapter::isItalic() const {
    if (auto et = editableText()) return et->isItalic();
    if (auto st = qgraphicsitem_cast<QGraphicsSimpleTextItem*>(m_item)) return st->font().italic();
    return false;
}

void QGraphicsItemAdapter::setItalic(bool italic) {
    if (auto et = editableText()) { et->setItalic(italic); return; }
    if (auto st = qgraphicsitem_cast<QGraphicsSimpleTextItem*>(m_item)) {
        QFont font = st->font();
        font.setItalic(italic);
        st->setFont(font);
    }
}

QColor QGraphicsItemAdapter::textColor() const {
    if (auto et = editableText()) return et->defaultTextColor();
    if (auto st = qgraphicsitem_cast<QGraphicsSimpleTextItem*>(m_item)) return st->brush().color();
    return QColor();
}

void QGraphicsItemAdapter::setTextColor(const QColor& color) {
    if (auto et = editableText()) { et->setDefaultTextColor(color); return; }
    if (auto st = qgraphicsitem_cast<QGraphicsSimpleTextItem*>(m_item)) { st->setBrush(QBrush(color)); return; }
}

Qt::Alignment QGraphicsItemAdapter::textAlignment() const {
    if (auto et = editableText()) return et->textAlignment();
    return Qt::AlignLeft;
}

void QGraphicsItemAdapter::setTextAlignment(Qt::Alignment alignment) {
    if (auto et = editableText()) { et->setTextAlignment(alignment); return; }
}

ShapeType QGraphicsItemAdapter::shapeType() const {
    return m_type;
}
