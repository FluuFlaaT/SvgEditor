#pragma once

#include <QColor>
#include <QPen>
#include <QBrush>
#include <QString>
#include <QFont>
#include <Qt>
#include <optional>

class QGraphicsItem;

class IShapeItemAdapter {
public:
    virtual ~IShapeItemAdapter() = default;

    virtual bool hasStroke() const = 0;
    virtual bool hasFill() const = 0;
    virtual bool hasText() const = 0;

    // Stroke properties
    virtual QColor strokeColor() const = 0;
    virtual void setStrokeColor(const QColor& color) = 0;
    virtual int strokeWidth() const = 0;
    virtual void setStrokeWidth(int width) = 0;
    virtual Qt::PenStyle strokeStyle() const = 0;
    virtual void setStrokeStyle(Qt::PenStyle style) = 0;

    // Fill properties
    virtual QColor fillColor() const = 0;
    virtual void setFillColor(const QColor& color) = 0;

    // Common
    virtual qreal opacity() const = 0;
    virtual void setOpacity(qreal opacity) = 0;

    // Text properties (only valid when hasText())
    virtual QString textContent() const = 0;
    virtual void setTextContent(const QString& text) = 0;
    virtual QFont textFont() const = 0;
    virtual void setTextFont(const QFont& font) = 0;
    virtual bool isBold() const = 0;
    virtual void setBold(bool bold) = 0;
    virtual bool isItalic() const = 0;
    virtual void setItalic(bool italic) = 0;
    virtual QColor textColor() const = 0;
    virtual void setTextColor(const QColor& color) = 0;
    virtual Qt::Alignment textAlignment() const = 0;
    virtual void setTextAlignment(Qt::Alignment alignment) = 0;
};
