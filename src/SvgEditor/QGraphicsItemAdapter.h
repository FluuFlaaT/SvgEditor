#pragma once

#include "Core/IShapeItemAdapter.h"
#include "Core/ShapeType.h"
#include <QGraphicsItem>

class EditableTextItem;

class QGraphicsItemAdapter : public IShapeItemAdapter {
public:
    explicit QGraphicsItemAdapter(QGraphicsItem* item);

    bool hasStroke() const override;
    bool hasFill() const override;
    bool hasText() const override;

    QColor strokeColor() const override;
    void setStrokeColor(const QColor& color) override;
    int strokeWidth() const override;
    void setStrokeWidth(int width) override;
    Qt::PenStyle strokeStyle() const override;
    void setStrokeStyle(Qt::PenStyle style) override;

    QColor fillColor() const override;
    void setFillColor(const QColor& color) override;

    qreal opacity() const override;
    void setOpacity(qreal opacity) override;

    QString textContent() const override;
    void setTextContent(const QString& text) override;
    QFont textFont() const override;
    void setTextFont(const QFont& font) override;
    bool isBold() const override;
    void setBold(bool bold) override;
    bool isItalic() const override;
    void setItalic(bool italic) override;
    QColor textColor() const override;
    void setTextColor(const QColor& color) override;
    Qt::Alignment textAlignment() const override;
    void setTextAlignment(Qt::Alignment alignment) override;

    ShapeType shapeType() const;
    QGraphicsItem* item() const { return m_item; }

    static ShapeType resolveShapeType(QGraphicsItem* item);

private:
    QGraphicsItem* m_item;
    ShapeType m_type;

    QPen getPen() const;
    void setPen(const QPen& pen);
    QBrush getBrush() const;
    void setBrush(const QBrush& brush);
    EditableTextItem* editableText() const;
};
