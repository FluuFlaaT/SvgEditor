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
#include <QFontDatabase>
#include <QCheckBox>
#include <QGraphicsSimpleTextItem>
#include "editabletextitem.h"
#include "shapetoolbar.h" // Contains ShapeType definitions

Q_DECLARE_LOGGING_CATEGORY(rightAttrBarLog)
class RightAttrBar : public QWidget
{
    Q_OBJECT

public:
    explicit RightAttrBar(QWidget *parent = nullptr);
    ~RightAttrBar();

    // Widget stack indices must match these enum values
    enum WidgetType {
        CommonAttributes = 0,
        CircleAttributes,
        RectangleAttributes,
        LineAttributes,
        TextAttributes
    };

public slots:
    void setCurrentWidget(int widgetType);
    void updateCanvasSize(int width, int height);
    void updateCanvasColor(const QColor& color);
    void updateZoomLevel(qreal zoomFactor);

    // Element selection drives the attribute panel display
    void updateForSelectedItem(QGraphicsItem* item, ShapeType type);
    void updateShapeProperties(QGraphicsItem* item, ShapeType type);
    void clearSelection();

public:
    QColor getCanvasColor() const { return m_canvasColor; }
    int getCanvasWidth() const { return m_canvasWidthSpinBox ? m_canvasWidthSpinBox->value() : 800; }
    int getCanvasHeight() const { return m_canvasHeightSpinBox ? m_canvasHeightSpinBox->value() : 600; }

    // Current visual properties for new shape creation
    QColor getBorderColor() const { return m_borderColor; }
    QColor getFillColor() const { return m_fillColor; }
    int getBorderWidth() const { return m_borderWidthSpinBox ? m_borderWidthSpinBox->value() : 1; }
    Qt::PenStyle getBorderStyle() const;

signals:
    void canvasSizeChanged(int width, int height);
    void canvasColorChanged(const QColor& color);

    // Shape property modification signals
    void borderColorChanged(const QColor& color);
    void fillColorChanged(const QColor& color);
    void borderWidthChanged(int width);
    void borderStyleChanged(Qt::PenStyle style);

    // Text property modification signals
    void fontFamilyChanged(const QString& family);
    void fontSizeChanged(int size);
    void fontBoldChanged(bool bold);
    void fontItalicChanged(bool italic);
    void textAlignmentChanged(int alignment);
    void textContentChanged(const QString& text);
    void textColorChanged(const QColor& color);

private:
    QVBoxLayout* m_mainLayout;
    QStackedWidget* m_stackedWidget;

    // Canvas configuration controls
    QSpinBox* m_canvasWidthSpinBox;
    QSpinBox* m_canvasHeightSpinBox;
    QLineEdit* m_zoomEdit;
    QPushButton* m_canvasColorButton;
    QColor m_canvasColor;

    // Shape visual property controls
    QSpinBox* m_borderWidthSpinBox;
    QComboBox* m_borderStyleComboBox;
    QPushButton* m_borderColorButton;
    QPushButton* m_fillColorButton;
    QColor m_borderColor;
    QColor m_fillColor;

    // Text formatting controls
    QComboBox* m_fontFamilyComboBox;
    QSpinBox* m_fontSizeSpinBox;
    QCheckBox* m_boldCheckBox;
    QCheckBox* m_italicCheckBox;
    QComboBox* m_textAlignComboBox;
    QLineEdit* m_textContentEdit;
    QPushButton* m_textColorButton;
    QColor m_textColor;

    // Selection state tracking
    QGraphicsItem* m_selectedItem;
    ShapeType m_selectedItemType;

    QWidget* createCommonAttributesWidget();
    QWidget* createCircleAttributesWidget();
    QWidget* createRectangleAttributesWidget();
    QWidget* createLineAttributesWidget();
    QWidget* createTextAttributesWidget();
};