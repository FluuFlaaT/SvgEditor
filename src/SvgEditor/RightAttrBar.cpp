#include "RightAttrBar.h"

Q_LOGGING_CATEGORY(rightAttrBarLog, "RightAttrBar")

RightAttrBar::RightAttrBar(QWidget *parent)
    : QWidget(parent),
    m_canvasColor(Qt::white),
    m_borderColor(Qt::black),
    m_fillColor(Qt::white)
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
            // TODO: Emit signal for border color change
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
            // TODO: Emit signal for fill color change
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
