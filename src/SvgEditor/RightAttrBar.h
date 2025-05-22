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

public:
    QColor getCanvasColor() const { return m_canvasColor; }
    int getCanvasWidth() const { return m_canvasWidthSpinBox ? m_canvasWidthSpinBox->value() : 800; }
    int getCanvasHeight() const { return m_canvasHeightSpinBox ? m_canvasHeightSpinBox->value() : 600; }

signals:
    void canvasSizeChanged(int width, int height);
    void canvasColorChanged(const QColor& color);

private:
    QVBoxLayout* m_mainLayout;
    QStackedWidget* m_stackedWidget;

    // Common attributes controls
    QSpinBox* m_canvasWidthSpinBox;
    QSpinBox* m_canvasHeightSpinBox;
    QLineEdit* m_zoomEdit;
    QPushButton* m_canvasColorButton;
    QColor m_canvasColor;

    // Circle attributes controls
    QSpinBox* m_borderWidthSpinBox;
    QComboBox* m_borderStyleComboBox;
    QPushButton* m_borderColorButton;
    QPushButton* m_fillColorButton;
    QColor m_borderColor;
    QColor m_fillColor;

    QWidget* createCommonAttributesWidget();
    QWidget* createCircleAttributesWidget();
    QWidget* createRectangleAttributesWidget();
    QWidget* createLineAttributesWidget();
};