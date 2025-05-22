#pragma once

#include <QWidget>
#include <QVBoxLayout>
#include <QLabel>
#include <QStackedWidget>
#include <QLoggingCategory>
#include <QColor>
#include <QColorDialog>
#include <QSpinBox>
#include <QPushButton>
#include <QLineEdit>
#include <QComboBox>
#include <QVariant>
#include <QPen>
#include <QGraphicsItem>
#include "ShapeToolBar.h" // 包含ShapeType定义

Q_DECLARE_LOGGING_CATEGORY(rightAttrBarLog)
class RightAttrBar : public QWidget
{
    Q_OBJECT

public:
    explicit RightAttrBar(QWidget *parent = nullptr);
    ~RightAttrBar();

    // Enum for widget types
    enum WidgetType {
        CommonAttributes = 0,
        CircleAttributes,
        RectangleAttributes,
        LineAttributes
    };

public slots:
    void setCurrentWidget(int widgetType);
    void updateCanvasSize(int width, int height);
    void updateCanvasColor(const QColor& color);
    void updateZoomLevel(qreal zoomFactor);

    // New slots for handling selected elements
    void updateForSelectedItem(QGraphicsItem* item, ShapeType type);
    void updateShapeProperties(QGraphicsItem* item, ShapeType type);
    void clearSelection();

public:
    QColor getCanvasColor() const { return m_canvasColor; }
    int getCanvasWidth() const { return m_canvasWidthSpinBox ? m_canvasWidthSpinBox->value() : 800; }
    int getCanvasHeight() const { return m_canvasHeightSpinBox ? m_canvasHeightSpinBox->value() : 600; }

    // Get current shape properties
    QColor getBorderColor() const { return m_borderColor; }
    QColor getFillColor() const { return m_fillColor; }
    int getBorderWidth() const { return m_borderWidthSpinBox ? m_borderWidthSpinBox->value() : 1; }
    Qt::PenStyle getBorderStyle() const;

signals:
    void canvasSizeChanged(int width, int height);
    void canvasColorChanged(const QColor& color);

    // New signals for shape property changes
    void borderColorChanged(const QColor& color);
    void fillColorChanged(const QColor& color);
    void borderWidthChanged(int width);
    void borderStyleChanged(Qt::PenStyle style);

private:
    QVBoxLayout* m_mainLayout;
    QStackedWidget* m_stackedWidget;

    // Common attributes controls
    QSpinBox* m_canvasWidthSpinBox;
    QSpinBox* m_canvasHeightSpinBox;
    QLineEdit* m_zoomEdit;
    QPushButton* m_canvasColorButton;
    QColor m_canvasColor;

    // Shape attributes controls
    QSpinBox* m_borderWidthSpinBox;
    QComboBox* m_borderStyleComboBox;
    QPushButton* m_borderColorButton;
    QPushButton* m_fillColorButton;
    QColor m_borderColor;
    QColor m_fillColor;

    // Currently selected item
    QGraphicsItem* m_selectedItem;
    ShapeType m_selectedItemType;

    QWidget* createCommonAttributesWidget();
    QWidget* createCircleAttributesWidget();
    QWidget* createRectangleAttributesWidget();
    QWidget* createLineAttributesWidget();
};