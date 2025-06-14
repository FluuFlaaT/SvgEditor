﻿#include "rightattrbar.h"

Q_LOGGING_CATEGORY(rightAttrBarLog, "RightAttrBar")

RightAttrBar::RightAttrBar(QWidget *parent)
    : QWidget(parent),
    m_canvasColor(Qt::white),
    m_borderColor(Qt::black),
    m_fillColor(Qt::white),
    m_selectedItem(nullptr),
    m_selectedItemType(ShapeType::None)
{
    // Fixed width ensures consistent UI layout regardless of content changes
    setFixedWidth(300);

    m_mainLayout = new QVBoxLayout(this);
    m_mainLayout->setContentsMargins(5, 10, 5, 10);
    m_mainLayout->setSpacing(10);

    m_stackedWidget = new QStackedWidget(this);

    // Order matters here - enum values must match widget indices
    m_stackedWidget->addWidget(createCommonAttributesWidget());
    m_stackedWidget->addWidget(createCircleAttributesWidget());
    m_stackedWidget->addWidget(createRectangleAttributesWidget());
    m_stackedWidget->addWidget(createLineAttributesWidget());
    m_stackedWidget->addWidget(createTextAttributesWidget());

    // Default to common attributes when no shape is selected
    m_stackedWidget->setCurrentIndex(CommonAttributes);

    m_mainLayout->addWidget(m_stackedWidget);
    m_mainLayout->addStretch();

    qCDebug(rightAttrBarLog) << "RightAttrBar construct success.";
}

RightAttrBar::~RightAttrBar()
{
    // Qt handles widget cleanup automatically through parent-child relationships
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
    // Prevent unnecessary UI updates and infinite signal loops
    if (m_canvasWidthSpinBox->value() != width || m_canvasHeightSpinBox->value() != height) {
        // Signal blocking prevents recursive updates from user modifications
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
        // Avoid redundant updates that cause unnecessary repaints
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

    // Shape type determines which attribute panel is most relevant to the user
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
        case ShapeType::Text:
            setCurrentWidget(TextAttributes);
            break;
        case ShapeType::Pentagon:
        case ShapeType::Hexagon:
        case ShapeType::Star:
            // Polygon shapes share the same visual properties as circles
            setCurrentWidget(CircleAttributes);
            break;
        case ShapeType::Freehand:
            // Freehand drawing has limited customizable properties
            setCurrentWidget(CommonAttributes);
            break;
        default:
            setCurrentWidget(CommonAttributes);
            break;
    }

    updateShapeProperties(item, type);

    qCDebug(rightAttrBarLog) << "Updated UI for selected item, type:" << static_cast<int>(type);
}

void RightAttrBar::updateShapeProperties(QGraphicsItem* item, ShapeType type)
{
    if (!item) {
        return;
    }

    // Extract visual properties based on Qt's graphics item hierarchy
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
    } else if (auto textItem = dynamic_cast<EditableTextItem*>(item)) {
        // Text items require special handling due to font and alignment properties
        if (type == ShapeType::Text && m_textContentEdit && m_fontFamilyComboBox &&
            m_fontSizeSpinBox && m_boldCheckBox && m_italicCheckBox && m_textAlignComboBox) {

            // Prevent cascading events during bulk property updates
            m_textContentEdit->blockSignals(true);
            m_fontFamilyComboBox->blockSignals(true);
            m_fontSizeSpinBox->blockSignals(true);
            m_boldCheckBox->blockSignals(true);
            m_italicCheckBox->blockSignals(true);
            m_textAlignComboBox->blockSignals(true);

            m_textContentEdit->setText(textItem->toPlainString());

            QFont font = textItem->font();

            int fontIndex = m_fontFamilyComboBox->findText(font.family());
            if (fontIndex >= 0) {
                m_fontFamilyComboBox->setCurrentIndex(fontIndex);
            }

            m_fontSizeSpinBox->setValue(font.pointSize());

            m_boldCheckBox->setChecked(textItem->isBold());
            m_italicCheckBox->setChecked(textItem->isItalic());

            int alignmentIndex = 0; // Default to left alignment
            Qt::Alignment alignment = textItem->textAlignment();
            if (alignment & Qt::AlignCenter) {
                alignmentIndex = 1;
            } else if (alignment & Qt::AlignRight) {
                alignmentIndex = 2;
            }
            m_textAlignComboBox->setCurrentIndex(alignmentIndex);

            m_textColor = textItem->defaultTextColor();
            QString qss = QString("background-color: %1").arg(m_textColor.name());
            m_textColorButton->setStyleSheet(qss);

            m_textContentEdit->blockSignals(false);
            m_fontFamilyComboBox->blockSignals(false);
            m_fontSizeSpinBox->blockSignals(false);
            m_boldCheckBox->blockSignals(false);
            m_italicCheckBox->blockSignals(false);
            m_textAlignComboBox->blockSignals(false);

            qCDebug(rightAttrBarLog) << "Updated editable text properties: content=" << textItem->toPlainString()
                                   << ", font=" << font.family()
                                   << ", size=" << font.pointSize()
                                   << ", bold=" << textItem->isBold()
                                   << ", italic=" << textItem->isItalic()
                                   << ", color=" << m_textColor.name();

            // Text items have unique properties that don't apply to other shapes
            return;
        }
    } else if (auto textItem = dynamic_cast<QGraphicsSimpleTextItem*>(item)) {
        // Legacy support for applications that might use QGraphicsSimpleTextItem
        if (type == ShapeType::Text && m_textContentEdit && m_fontFamilyComboBox &&
            m_fontSizeSpinBox && m_boldCheckBox && m_italicCheckBox && m_textAlignComboBox) {

            // Prevent cascading events during bulk property updates
            m_textContentEdit->blockSignals(true);
            m_fontFamilyComboBox->blockSignals(true);
            m_fontSizeSpinBox->blockSignals(true);
            m_boldCheckBox->blockSignals(true);
            m_italicCheckBox->blockSignals(true);
            m_textAlignComboBox->blockSignals(true);

            m_textContentEdit->setText(textItem->text());

            QFont font = textItem->font();

            int fontIndex = m_fontFamilyComboBox->findText(font.family());
            if (fontIndex >= 0) {
                m_fontFamilyComboBox->setCurrentIndex(fontIndex);
            }

            m_fontSizeSpinBox->setValue(font.pointSize());

            m_boldCheckBox->setChecked(font.bold());
            m_italicCheckBox->setChecked(font.italic());

            m_textColor = textItem->brush().color();
            QString qss = QString("background-color: %1").arg(m_textColor.name());
            m_textColorButton->setStyleSheet(qss);

            m_textContentEdit->blockSignals(false);
            m_fontFamilyComboBox->blockSignals(false);
            m_fontSizeSpinBox->blockSignals(false);
            m_boldCheckBox->blockSignals(false);
            m_italicCheckBox->blockSignals(false);
            m_textAlignComboBox->blockSignals(false);

            qCDebug(rightAttrBarLog) << "Updated simple text properties: content=" << textItem->text()
                                   << ", font=" << font.family()
                                   << ", size=" << font.pointSize()
                                   << ", bold=" << font.bold()
                                   << ", italic=" << font.italic()
                                   << ", color=" << m_textColor.name();

            // Text items have unique properties that don't apply to other shapes
            return;
        }
    } else {
        return;
    }

    // Apply properties common to all drawable shapes (pen and brush)
    if (m_borderWidthSpinBox) {
        m_borderWidthSpinBox->blockSignals(true);
        m_borderWidthSpinBox->setValue(pen.width());
        m_borderWidthSpinBox->blockSignals(false);
    }

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

    m_borderColor = pen.color();
    if (m_borderColorButton) {
        QString qss = QString("background-color: %1").arg(m_borderColor.name());
        m_borderColorButton->setStyleSheet(qss);
    }

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

    QHBoxLayout* widthLayout = new QHBoxLayout();
    QLabel* widthLabel = new QLabel(tr("Canvas Width:"));
    m_canvasWidthSpinBox = new QSpinBox();
    m_canvasWidthSpinBox->setRange(1, 9999);
    m_canvasWidthSpinBox->setValue(800); // 800x600 is a common default resolution
    m_canvasWidthSpinBox->setSuffix(" px");
    widthLayout->addWidget(widthLabel);
    widthLayout->addWidget(m_canvasWidthSpinBox);

    QHBoxLayout* heightLayout = new QHBoxLayout();
    QLabel* heightLabel = new QLabel(tr("Canvas Height:"));
    m_canvasHeightSpinBox = new QSpinBox();
    m_canvasHeightSpinBox->setRange(1, 9999);
    m_canvasHeightSpinBox->setValue(600); // 800x600 is a common default resolution
    m_canvasHeightSpinBox->setSuffix(" px");
    heightLayout->addWidget(heightLabel);
    heightLayout->addWidget(m_canvasHeightSpinBox);

    // Read-only display prevents user confusion about edit capabilities
    QHBoxLayout* zoomLayout = new QHBoxLayout();
    QLabel* zoomLabel = new QLabel(tr("Zoom:"));
    m_zoomEdit = new QLineEdit("100%");
    m_zoomEdit->setReadOnly(true);
    zoomLayout->addWidget(zoomLabel);
    zoomLayout->addWidget(m_zoomEdit);

    QHBoxLayout* colorLayout = new QHBoxLayout();
    QLabel* colorLabel = new QLabel(tr("Canvas Color:"));
    m_canvasColorButton = new QPushButton();
    m_canvasColorButton->setFixedSize(30, 30);
    QString qss = QString("background-color: %1").arg(m_canvasColor.name());
    m_canvasColorButton->setStyleSheet(qss);

    // Lambda captures this to ensure proper signal emission in Qt's event system
    connect(m_canvasColorButton, &QPushButton::clicked, this, [this]() {
        QColor color = QColorDialog::getColor(m_canvasColor, this, tr("Select Canvas Color"));
        if (color.isValid()) {
            m_canvasColor = color;
            QString qss = QString("background-color: %1").arg(color.name());
            m_canvasColorButton->setStyleSheet(qss);

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

    QHBoxLayout* borderWidthLayout = new QHBoxLayout();
    QLabel* borderWidthLabel = new QLabel(tr("Border Width:"));
    m_borderWidthSpinBox = new QSpinBox();
    m_borderWidthSpinBox->setRange(0, 9);
    m_borderWidthSpinBox->setValue(1);
    m_borderWidthSpinBox->setSuffix(" px");

    connect(m_borderWidthSpinBox, QOverload<int>::of(&QSpinBox::valueChanged), this, [this](int width) {
        emit borderWidthChanged(width);
        qCDebug(rightAttrBarLog) << "Border width changed to:" << width;
    });

    borderWidthLayout->addWidget(borderWidthLabel);
    borderWidthLayout->addWidget(m_borderWidthSpinBox);

    QHBoxLayout* borderStyleLayout = new QHBoxLayout();
    QLabel* borderStyleLabel = new QLabel(tr("Border Style:"));
    m_borderStyleComboBox = new QComboBox();
    m_borderStyleComboBox->addItem(tr("Solid Line"), static_cast<int>(Qt::SolidLine));
    m_borderStyleComboBox->addItem(tr("Dash Line"), static_cast<int>(Qt::DashLine));
    m_borderStyleComboBox->addItem(tr("Dot Line"), static_cast<int>(Qt::DotLine));
    m_borderStyleComboBox->addItem(tr("Dash Dot Line"), static_cast<int>(Qt::DashDotLine));
    m_borderStyleComboBox->addItem(tr("Dash Dot Dot Line"), static_cast<int>(Qt::DashDotDotLine));

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

QWidget* RightAttrBar::createRectangleAttributesWidget()
{
    QWidget* widget = new QWidget();
    QVBoxLayout* layout = new QVBoxLayout(widget);
    layout->setContentsMargins(5, 10, 5, 10);
    layout->setSpacing(10);

    QHBoxLayout* borderWidthLayout = new QHBoxLayout();
    QLabel* borderWidthLabel = new QLabel(tr("Border Width:"));
    m_borderWidthSpinBox = new QSpinBox();
    m_borderWidthSpinBox->setRange(0, 9);
    m_borderWidthSpinBox->setValue(1);
    m_borderWidthSpinBox->setSuffix(" px");

    connect(m_borderWidthSpinBox, QOverload<int>::of(&QSpinBox::valueChanged), this, [this](int width) {
        emit borderWidthChanged(width);
        qCDebug(rightAttrBarLog) << "Border width changed to:" << width;
    });

    borderWidthLayout->addWidget(borderWidthLabel);
    borderWidthLayout->addWidget(m_borderWidthSpinBox);

    QHBoxLayout* borderStyleLayout = new QHBoxLayout();
    QLabel* borderStyleLabel = new QLabel(tr("Border Style:"));
    m_borderStyleComboBox = new QComboBox();
    m_borderStyleComboBox->addItem(tr("Solid Line"), static_cast<int>(Qt::SolidLine));
    m_borderStyleComboBox->addItem(tr("Dash Line"), static_cast<int>(Qt::DashLine));
    m_borderStyleComboBox->addItem(tr("Dot Line"), static_cast<int>(Qt::DotLine));
    m_borderStyleComboBox->addItem(tr("Dash Dot Line"), static_cast<int>(Qt::DashDotLine));
    m_borderStyleComboBox->addItem(tr("Dash Dot Dot Line"), static_cast<int>(Qt::DashDotDotLine));

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

QWidget* RightAttrBar::createLineAttributesWidget()
{
    QWidget* widget = new QWidget();
    QVBoxLayout* layout = new QVBoxLayout(widget);
    layout->setContentsMargins(5, 10, 5, 10);
    layout->setSpacing(10);

    QHBoxLayout* borderWidthLayout = new QHBoxLayout();
    QLabel* borderWidthLabel = new QLabel(tr("Line Width:"));
    m_borderWidthSpinBox = new QSpinBox();
    m_borderWidthSpinBox->setRange(0, 9);
    m_borderWidthSpinBox->setValue(1);
    m_borderWidthSpinBox->setSuffix(" px");

    connect(m_borderWidthSpinBox, QOverload<int>::of(&QSpinBox::valueChanged), this, [this](int width) {
        emit borderWidthChanged(width);
        qCDebug(rightAttrBarLog) << "Line width changed to:" << width;
    });

    borderWidthLayout->addWidget(borderWidthLabel);
    borderWidthLayout->addWidget(m_borderWidthSpinBox);

    QHBoxLayout* borderStyleLayout = new QHBoxLayout();
    QLabel* borderStyleLabel = new QLabel(tr("Line Style:"));
    m_borderStyleComboBox = new QComboBox();
    m_borderStyleComboBox->addItem(tr("Solid Line"), static_cast<int>(Qt::SolidLine));
    m_borderStyleComboBox->addItem(tr("Dash Line"), static_cast<int>(Qt::DashLine));
    m_borderStyleComboBox->addItem(tr("Dot Line"), static_cast<int>(Qt::DotLine));
    m_borderStyleComboBox->addItem(tr("Dash Dot Line"), static_cast<int>(Qt::DashDotLine));
    m_borderStyleComboBox->addItem(tr("Dash Dot Dot Line"), static_cast<int>(Qt::DashDotDotLine));

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
        qCDebug(rightAttrBarLog) << "Line style changed to index:" << index;
    });

    borderStyleLayout->addWidget(borderStyleLabel);
    borderStyleLayout->addWidget(m_borderStyleComboBox);

    QHBoxLayout* borderColorLayout = new QHBoxLayout();
    QLabel* borderColorLabel = new QLabel(tr("Line Color:"));
    m_borderColorButton = new QPushButton();
    m_borderColorButton->setFixedSize(30, 30);
    QString borderQss = QString("background-color: %1").arg(m_borderColor.name());
    m_borderColorButton->setStyleSheet(borderQss);

    connect(m_borderColorButton, &QPushButton::clicked, this, [this]() {
        QColor color = QColorDialog::getColor(m_borderColor, this, tr("Select Line Color"));
        if (color.isValid()) {
            m_borderColor = color;
            QString qss = QString("background-color: %1").arg(color.name());
            m_borderColorButton->setStyleSheet(qss);
            emit borderColorChanged(color);
            qCDebug(rightAttrBarLog) << "Line color changed to:" << color.name();
        }
    });

    borderColorLayout->addWidget(borderColorLabel);
    borderColorLayout->addWidget(m_borderColorButton);

    layout->addLayout(borderWidthLayout);
    layout->addLayout(borderStyleLayout);
    layout->addLayout(borderColorLayout);
    layout->addStretch();

    return widget;
}

QWidget* RightAttrBar::createTextAttributesWidget()
{
    QWidget* widget = new QWidget();
    QVBoxLayout* layout = new QVBoxLayout(widget);
    layout->setContentsMargins(5, 10, 5, 10);
    layout->setSpacing(10);

    QHBoxLayout* textContentLayout = new QHBoxLayout();
    QLabel* textContentLabel = new QLabel(tr("Text:"));
    m_textContentEdit = new QLineEdit();
    m_textContentEdit->setPlaceholderText(tr("Enter text"));

    textContentLayout->addWidget(textContentLabel);
    textContentLayout->addWidget(m_textContentEdit);

    QHBoxLayout* fontFamilyLayout = new QHBoxLayout();
    QLabel* fontFamilyLabel = new QLabel(tr("Font:"));
    m_fontFamilyComboBox = new QComboBox();

    // System fonts provide platform-appropriate text rendering
    QFontDatabase fontDatabase;
    QStringList fontFamilies = fontDatabase.families();
    m_fontFamilyComboBox->addItems(fontFamilies);

    // Arial provides consistent cross-platform appearance
    int defaultIndex = fontFamilies.indexOf("Arial");
    if (defaultIndex >= 0) {
        m_fontFamilyComboBox->setCurrentIndex(defaultIndex);
    }

    fontFamilyLayout->addWidget(fontFamilyLabel);
    fontFamilyLayout->addWidget(m_fontFamilyComboBox);

    QHBoxLayout* fontSizeLayout = new QHBoxLayout();
    QLabel* fontSizeLabel = new QLabel(tr("Size:"));
    m_fontSizeSpinBox = new QSpinBox();
    m_fontSizeSpinBox->setRange(6, 72);
    m_fontSizeSpinBox->setValue(12); // 12pt is standard for readability
    m_fontSizeSpinBox->setSuffix(" pt");

    fontSizeLayout->addWidget(fontSizeLabel);
    fontSizeLayout->addWidget(m_fontSizeSpinBox);

    QHBoxLayout* fontStyleLayout = new QHBoxLayout();
    m_boldCheckBox = new QCheckBox(tr("Bold"));
    m_italicCheckBox = new QCheckBox(tr("Italic"));

    fontStyleLayout->addWidget(m_boldCheckBox);
    fontStyleLayout->addWidget(m_italicCheckBox);

    QHBoxLayout* textAlignLayout = new QHBoxLayout();
    QLabel* textAlignLabel = new QLabel(tr("Alignment:"));
    m_textAlignComboBox = new QComboBox();
    m_textAlignComboBox->addItem(tr("Left"), 0);
    m_textAlignComboBox->addItem(tr("Center"), 1);
    m_textAlignComboBox->addItem(tr("Right"), 2);

    textAlignLayout->addWidget(textAlignLabel);
    textAlignLayout->addWidget(m_textAlignComboBox);

    QHBoxLayout* textColorLayout = new QHBoxLayout();
    QLabel* textColorLabel = new QLabel(tr("Color:"));
    m_textColorButton = new QPushButton();
    m_textColorButton->setFixedWidth(50);
    m_textColor = Qt::black; // Black provides maximum contrast and readability
    QString qss = QString("background-color: %1").arg(m_textColor.name());
    m_textColorButton->setStyleSheet(qss);

    textColorLayout->addWidget(textColorLabel);
    textColorLayout->addWidget(m_textColorButton);

    connect(m_textContentEdit, &QLineEdit::textChanged, this, [this](const QString& text) {
        emit textContentChanged(text);
        qCDebug(rightAttrBarLog) << "Text content changed to:" << text;
    });

    connect(m_fontFamilyComboBox, QOverload<int>::of(&QComboBox::currentIndexChanged), this, [this](int index) {
        QString family = m_fontFamilyComboBox->itemText(index);
        emit fontFamilyChanged(family);
        qCDebug(rightAttrBarLog) << "Font family changed to:" << family;
    });

    connect(m_fontSizeSpinBox, QOverload<int>::of(&QSpinBox::valueChanged), this, [this](int size) {
        emit fontSizeChanged(size);
        qCDebug(rightAttrBarLog) << "Font size changed to:" << size;
    });

    connect(m_boldCheckBox, &QCheckBox::toggled, this, [this](bool checked) {
        emit fontBoldChanged(checked);
        qCDebug(rightAttrBarLog) << "Font bold changed to:" << (checked ? "true" : "false");
    });

    connect(m_italicCheckBox, &QCheckBox::toggled, this, [this](bool checked) {
        emit fontItalicChanged(checked);
        qCDebug(rightAttrBarLog) << "Font italic changed to:" << (checked ? "true" : "false");
    });

    connect(m_textAlignComboBox, QOverload<int>::of(&QComboBox::currentIndexChanged), this, [this](int index) {
        emit textAlignmentChanged(index);
        qCDebug(rightAttrBarLog) << "Text alignment changed to:" << index;
    });

    connect(m_textColorButton, &QPushButton::clicked, this, [this]() {
        QColor color = QColorDialog::getColor(m_textColor, this, tr("Select Text Color"));
        if (color.isValid()) {
            m_textColor = color;
            QString qss = QString("background-color: %1").arg(color.name());
            m_textColorButton->setStyleSheet(qss);
            emit textColorChanged(color);
            qCDebug(rightAttrBarLog) << "Text color changed to:" << color.name();
        }
    });

    // Add all layouts to the main layout
    layout->addLayout(textContentLayout);
    layout->addLayout(fontFamilyLayout);
    layout->addLayout(fontSizeLayout);
    layout->addLayout(fontStyleLayout);
    layout->addLayout(textAlignLayout);
    layout->addLayout(textColorLayout);
    layout->addStretch();

    return widget;
}