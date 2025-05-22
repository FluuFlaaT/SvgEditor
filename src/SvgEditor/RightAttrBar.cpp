#include "RightAttrBar.h"

Q_LOGGING_CATEGORY(rightAttrBarLog, "RightAttrBar")

RightAttrBar::RightAttrBar(QWidget *parent)
    : QWidget(parent),
    m_canvasColor(Qt::white),
    m_borderColor(Qt::black),
    m_fillColor(Qt::white),
    m_selectedItem(nullptr),
    m_selectedItemType(ShapeType::None)
{
    // Widget initialization
    setFixedWidth(300);

    m_mainLayout = new QVBoxLayout(this);
    m_mainLayout->setContentsMargins(5, 10, 5, 10);
    m_mainLayout->setSpacing(10);

    m_stackedWidget = new QStackedWidget(this);

    // Add all widget types to the stacked widget
    m_stackedWidget->addWidget(createCommonAttributesWidget());
    m_stackedWidget->addWidget(createCircleAttributesWidget());
    // Uncomment when implemented
    // m_stackedWidget->addWidget(createRectangleAttributesWidget());
    // m_stackedWidget->addWidget(createLineAttributesWidget());

    // Set the common attributes widget as the default
    m_stackedWidget->setCurrentIndex(CommonAttributes);

    m_mainLayout->addWidget(m_stackedWidget);
    m_mainLayout->addStretch();

    qCDebug(rightAttrBarLog) << "RightAttrBar construct success.";
}

RightAttrBar::~RightAttrBar()
{
    // Cleanup placeholder
}

void RightAttrBar::setCurrentWidget(int widgetType)
{
    if (widgetType >= 0 && widgetType < m_stackedWidget->count()) {
        m_stackedWidget->setCurrentIndex(widgetType);
        qCDebug(rightAttrBarLog) << "Switched to widget type:" << widgetType;
    } else {
        qCWarning(rightAttrBarLog) << "Invalid widget type:" << widgetType;
    }
}

void RightAttrBar::updateCanvasSize(int width, int height)
{
    // Only update if the values have actually changed
    if (m_canvasWidthSpinBox->value() != width || m_canvasHeightSpinBox->value() != height) {
        // Update the spinbox values without triggering signals
        m_canvasWidthSpinBox->blockSignals(true);
        m_canvasHeightSpinBox->blockSignals(true);

        m_canvasWidthSpinBox->setValue(width);
        m_canvasHeightSpinBox->setValue(height);

        m_canvasWidthSpinBox->blockSignals(false);
        m_canvasHeightSpinBox->blockSignals(false);

        qCDebug(rightAttrBarLog) << "Canvas size updated to:" << width << "x" << height;
    }
}

void RightAttrBar::updateCanvasColor(const QColor& color)
{
    if (color.isValid()) {
        // Only update if the color has actually changed
        if (m_canvasColor != color) {
            m_canvasColor = color;
            QString qss = QString("background-color: %1").arg(color.name());
            m_canvasColorButton->setStyleSheet(qss);
            qCDebug(rightAttrBarLog) << "Canvas color updated to:" << color.name()
                                    << "RGBA:" << color.red() << color.green() << color.blue() << color.alpha();
        }
    }
}

void RightAttrBar::updateZoomLevel(qreal zoomFactor)
{
    if (m_zoomEdit) {
        m_zoomEdit->setText(tr("%1%").arg(int(zoomFactor * 100)));
        qCDebug(rightAttrBarLog) << "Zoom level updated to:" << zoomFactor;
    }
}

Qt::PenStyle RightAttrBar::getBorderStyle() const
{
    if (!m_borderStyleComboBox) {
        return Qt::SolidLine;
    }

    int index = m_borderStyleComboBox->currentIndex();
    switch (index) {
        case 0: return Qt::SolidLine;
        case 1: return Qt::DashLine;
        case 2: return Qt::DotLine;
        case 3: return Qt::DashDotLine;
        case 4: return Qt::DashDotDotLine;
        default: return Qt::SolidLine;
    }
}

void RightAttrBar::updateForSelectedItem(QGraphicsItem* item, ShapeType type)
{
    m_selectedItem = item;
    m_selectedItemType = type;

    if (!item) {
        clearSelection();
        return;
    }

    // Switch to the appropriate widget based on the item type
    switch (type) {
        case ShapeType::Rectangle:
            setCurrentWidget(RectangleAttributes);
            break;
        case ShapeType::Ellipse:
            setCurrentWidget(CircleAttributes);
            break;
        case ShapeType::Line:
            setCurrentWidget(LineAttributes);
            break;
        case ShapeType::Pentagon:
        case ShapeType::Hexagon:
        case ShapeType::Star:
        case ShapeType::Freehand:
            // For now, use circle attributes for these shapes
            setCurrentWidget(CircleAttributes);
            break;
        default:
            setCurrentWidget(CommonAttributes);
            break;
    }

    // Update the UI with the item's properties
    updateShapeProperties(item, type);

    qCDebug(rightAttrBarLog) << "Updated UI for selected item, type:" << static_cast<int>(type);
}

void RightAttrBar::updateShapeProperties(QGraphicsItem* item, ShapeType type)
{
    if (!item) {
        return;
    }

    // Extract properties from the item based on its type
    QPen pen;
    QBrush brush;

    if (auto lineItem = dynamic_cast<QGraphicsLineItem*>(item)) {
        pen = lineItem->pen();
    } else if (auto rectItem = dynamic_cast<QGraphicsRectItem*>(item)) {
        pen = rectItem->pen();
        brush = rectItem->brush();
    } else if (auto ellipseItem = dynamic_cast<QGraphicsEllipseItem*>(item)) {
        pen = ellipseItem->pen();
        brush = ellipseItem->brush();
    } else if (auto polygonItem = dynamic_cast<QGraphicsPolygonItem*>(item)) {
        pen = polygonItem->pen();
        brush = polygonItem->brush();
    } else if (auto pathItem = dynamic_cast<QGraphicsPathItem*>(item)) {
        pen = pathItem->pen();
        brush = pathItem->brush();
    } else {
        return;
    }

    // Update the UI controls with the extracted properties
    // Border width
    if (m_borderWidthSpinBox) {
        m_borderWidthSpinBox->blockSignals(true);
        m_borderWidthSpinBox->setValue(pen.width());
        m_borderWidthSpinBox->blockSignals(false);
    }

    // Border style
    if (m_borderStyleComboBox) {
        m_borderStyleComboBox->blockSignals(true);
        int styleIndex = 0;
        switch (pen.style()) {
            case Qt::SolidLine: styleIndex = 0; break;
            case Qt::DashLine: styleIndex = 1; break;
            case Qt::DotLine: styleIndex = 2; break;
            case Qt::DashDotLine: styleIndex = 3; break;
            case Qt::DashDotDotLine: styleIndex = 4; break;
            default: styleIndex = 0; break;
        }
        m_borderStyleComboBox->setCurrentIndex(styleIndex);
        m_borderStyleComboBox->blockSignals(false);
    }

    // Border color
    m_borderColor = pen.color();
    if (m_borderColorButton) {
        QString qss = QString("background-color: %1").arg(m_borderColor.name());
        m_borderColorButton->setStyleSheet(qss);
    }

    // Fill color
    m_fillColor = brush.color();
    if (m_fillColorButton) {
        QString qss = QString("background-color: %1").arg(m_fillColor.name());
        m_fillColorButton->setStyleSheet(qss);
    }

    qCDebug(rightAttrBarLog) << "Updated shape properties: border width=" << pen.width()
                           << ", border style=" << pen.style()
                           << ", border color=" << pen.color().name()
                           << ", fill color=" << brush.color().name();
}

void RightAttrBar::clearSelection()
{
    m_selectedItem = nullptr;
    m_selectedItemType = ShapeType::None;
    setCurrentWidget(CommonAttributes);
    qCDebug(rightAttrBarLog) << "Selection cleared";
}

QWidget* RightAttrBar::createCommonAttributesWidget() {
    QWidget* widget = new QWidget();
    QVBoxLayout* layout = new QVBoxLayout(widget);
    layout->setContentsMargins(5, 10, 5, 10);
    layout->setSpacing(10);

    // 1. Canvas width
    QHBoxLayout* widthLayout = new QHBoxLayout();
    QLabel* widthLabel = new QLabel(tr("Canvas Width:"));
    m_canvasWidthSpinBox = new QSpinBox();
    m_canvasWidthSpinBox->setRange(1, 9999);
    m_canvasWidthSpinBox->setValue(800); // Default value
    m_canvasWidthSpinBox->setSuffix(" px");
    widthLayout->addWidget(widthLabel);
    widthLayout->addWidget(m_canvasWidthSpinBox);

    // 2. Canvas height
    QHBoxLayout* heightLayout = new QHBoxLayout();
    QLabel* heightLabel = new QLabel(tr("Canvas Height:"));
    m_canvasHeightSpinBox = new QSpinBox();
    m_canvasHeightSpinBox->setRange(1, 9999);
    m_canvasHeightSpinBox->setValue(600); // Default value
    m_canvasHeightSpinBox->setSuffix(" px");
    heightLayout->addWidget(heightLabel);
    heightLayout->addWidget(m_canvasHeightSpinBox);

    // 3. Zoom level (read-only)
    QHBoxLayout* zoomLayout = new QHBoxLayout();
    QLabel* zoomLabel = new QLabel(tr("Zoom:"));
    m_zoomEdit = new QLineEdit("100%");
    m_zoomEdit->setReadOnly(true);
    zoomLayout->addWidget(zoomLabel);
    zoomLayout->addWidget(m_zoomEdit);

    // 4. Canvas color
    QHBoxLayout* colorLayout = new QHBoxLayout();
    QLabel* colorLabel = new QLabel(tr("Canvas Color:"));
    m_canvasColorButton = new QPushButton();
    m_canvasColorButton->setFixedSize(30, 30);
    QString qss = QString("background-color: %1").arg(m_canvasColor.name());
    m_canvasColorButton->setStyleSheet(qss);

    // Connect color button click to color dialog
    connect(m_canvasColorButton, &QPushButton::clicked, this, [this]() {
        QColor color = QColorDialog::getColor(m_canvasColor, this, tr("Select Canvas Color"));
        if (color.isValid()) {
            m_canvasColor = color;
            QString qss = QString("background-color: %1").arg(color.name());
            m_canvasColorButton->setStyleSheet(qss);

            // Emit signal to notify about color change
            emit canvasColorChanged(color);
        }
    });

    colorLayout->addWidget(colorLabel);
    colorLayout->addWidget(m_canvasColorButton);

    // Connect size spinboxes to emit signals when values change
    connect(m_canvasWidthSpinBox, QOverload<int>::of(&QSpinBox::valueChanged), this, [this](int width) {
        emit canvasSizeChanged(width, m_canvasHeightSpinBox->value());
    });

    connect(m_canvasHeightSpinBox, QOverload<int>::of(&QSpinBox::valueChanged), this, [this](int height) {
        emit canvasSizeChanged(m_canvasWidthSpinBox->value(), height);
    });

    layout->addLayout(widthLayout);
    layout->addLayout(heightLayout);
    layout->addLayout(zoomLayout);
    layout->addLayout(colorLayout);
    layout->addStretch();

    return widget;
}

QWidget* RightAttrBar::createCircleAttributesWidget() {
    QWidget* widget = new QWidget();
    QVBoxLayout* layout = new QVBoxLayout(widget);
    layout->setContentsMargins(5, 10, 5, 10);
    layout->setSpacing(10);

    // 1. Border width
    QHBoxLayout* borderWidthLayout = new QHBoxLayout();
    QLabel* borderWidthLabel = new QLabel(tr("Border Width:"));
    m_borderWidthSpinBox = new QSpinBox();
    m_borderWidthSpinBox->setRange(0, 9);
    m_borderWidthSpinBox->setValue(1); // Default value
    m_borderWidthSpinBox->setSuffix(" px");

    // Connect border width change
    connect(m_borderWidthSpinBox, QOverload<int>::of(&QSpinBox::valueChanged), this, [this](int width) {
        emit borderWidthChanged(width);
        qCDebug(rightAttrBarLog) << "Border width changed to:" << width;
    });

    borderWidthLayout->addWidget(borderWidthLabel);
    borderWidthLayout->addWidget(m_borderWidthSpinBox);

    // 2. Border style
    QHBoxLayout* borderStyleLayout = new QHBoxLayout();
    QLabel* borderStyleLabel = new QLabel(tr("Border Style:"));
    m_borderStyleComboBox = new QComboBox();
    m_borderStyleComboBox->addItem(tr("Solid Line"), static_cast<int>(Qt::SolidLine));
    m_borderStyleComboBox->addItem(tr("Dash Line"), static_cast<int>(Qt::DashLine));
    m_borderStyleComboBox->addItem(tr("Dot Line"), static_cast<int>(Qt::DotLine));
    m_borderStyleComboBox->addItem(tr("Dash Dot Line"), static_cast<int>(Qt::DashDotLine));
    m_borderStyleComboBox->addItem(tr("Dash Dot Dot Line"), static_cast<int>(Qt::DashDotDotLine));

    // Connect border style change
    connect(m_borderStyleComboBox, QOverload<int>::of(&QComboBox::currentIndexChanged), this, [this](int index) {
        Qt::PenStyle style = Qt::SolidLine;
        switch (index) {
            case 0: style = Qt::SolidLine; break;
            case 1: style = Qt::DashLine; break;
            case 2: style = Qt::DotLine; break;
            case 3: style = Qt::DashDotLine; break;
            case 4: style = Qt::DashDotDotLine; break;
            default: style = Qt::SolidLine; break;
        }
        emit borderStyleChanged(style);
        qCDebug(rightAttrBarLog) << "Border style changed to index:" << index;
    });

    borderStyleLayout->addWidget(borderStyleLabel);
    borderStyleLayout->addWidget(m_borderStyleComboBox);

    // 3. Border color
    QHBoxLayout* borderColorLayout = new QHBoxLayout();
    QLabel* borderColorLabel = new QLabel(tr("Border Color:"));
    m_borderColorButton = new QPushButton();
    m_borderColorButton->setFixedSize(30, 30);
    QString borderQss = QString("background-color: %1").arg(m_borderColor.name());
    m_borderColorButton->setStyleSheet(borderQss);

    connect(m_borderColorButton, &QPushButton::clicked, this, [this]() {
        QColor color = QColorDialog::getColor(m_borderColor, this, tr("Select Border Color"));
        if (color.isValid()) {
            m_borderColor = color;
            QString qss = QString("background-color: %1").arg(color.name());
            m_borderColorButton->setStyleSheet(qss);
            emit borderColorChanged(color);
            qCDebug(rightAttrBarLog) << "Border color changed to:" << color.name();
        }
    });

    borderColorLayout->addWidget(borderColorLabel);
    borderColorLayout->addWidget(m_borderColorButton);

    // 4. Fill color
    QHBoxLayout* fillColorLayout = new QHBoxLayout();
    QLabel* fillColorLabel = new QLabel(tr("Fill Color:"));
    m_fillColorButton = new QPushButton();
    m_fillColorButton->setFixedSize(30, 30);
    QString fillQss = QString("background-color: %1").arg(m_fillColor.name());
    m_fillColorButton->setStyleSheet(fillQss);

    connect(m_fillColorButton, &QPushButton::clicked, this, [this]() {
        QColor color = QColorDialog::getColor(m_fillColor, this, tr("Select Fill Color"));
        if (color.isValid()) {
            m_fillColor = color;
            QString qss = QString("background-color: %1").arg(color.name());
            m_fillColorButton->setStyleSheet(qss);
            emit fillColorChanged(color);
            qCDebug(rightAttrBarLog) << "Fill color changed to:" << color.name();
        }
    });

    fillColorLayout->addWidget(fillColorLabel);
    fillColorLayout->addWidget(m_fillColorButton);

    layout->addLayout(borderWidthLayout);
    layout->addLayout(borderStyleLayout);
    layout->addLayout(borderColorLayout);
    layout->addLayout(fillColorLayout);
    layout->addStretch();

    return widget;
}
