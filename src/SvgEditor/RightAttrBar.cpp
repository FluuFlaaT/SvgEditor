#include "RightAttrBar.h"

Q_LOGGING_CATEGORY(rightAttrBarLog, "RightAttrBar")

RightAttrBar::RightAttrBar(QWidget *parent)
    : QWidget(parent)
{
    // Widget initialization placeholder
    setFixedWidth(300);

    m_mainLayout = new QVBoxLayout(this);
    m_mainLayout->setContentsMargins(5, 10, 5, 10);
    m_mainLayout->setSpacing(10);

    m_stackedWidget = new QStackedWidget(this);

    // m_stackedWidget->addWidget(createCommonAttributesWidget());
    m_stackedWidget->addWidget(createCircleAttributesWidget());
    // m_stackedWidget->addWidget(createRectangleAttributesWidget());
    // m_stackedWidget->addWidget(createLineAttributesWidget());

    m_mainLayout->addWidget(m_stackedWidget);
    m_mainLayout->addStretch();

    qCDebug(rightAttrBarLog) << "RightAttrBar construct success.";
}

RightAttrBar::~RightAttrBar()
{
    // Cleanup placeholder
}

QWidget* RightAttrBar::createCommonAttributesWidget() {
    QWidget* widget = new QWidget();
    QVBoxLayout* layout = new QVBoxLayout(widget);
    layout->setContentsMargins(5, 10, 5, 10);
    layout->setSpacing(10);
    
    // 1. 画布宽度
    QHBoxLayout* widthLayout = new QHBoxLayout();
    QLabel* widthLabel = new QLabel(tr("Canvas Width:"));
    QSpinBox* widthSpinBox = new QSpinBox();
    widthSpinBox->setRange(1, 9999);
    widthSpinBox->setValue(800); // Default value
    widthSpinBox->setSuffix(" px");
    widthLayout->addWidget(widthLabel);
    widthLayout->addWidget(widthSpinBox);
    
    // 2. 画布高度
    QHBoxLayout* heightLayout = new QHBoxLayout();
    QLabel* heightLabel = new QLabel(tr("Canvas Height:"));
    QSpinBox* heightSpinBox = new QSpinBox();
    heightSpinBox->setRange(1, 9999);
    heightSpinBox->setValue(600); // Default value
    heightSpinBox->setSuffix(" px");
    heightLayout->addWidget(heightLabel);
    heightLayout->addWidget(heightSpinBox);
    
    // 3. 缩放比例（不可编辑）
    QHBoxLayout* zoomLayout = new QHBoxLayout();
    QLabel* zoomLabel = new QLabel(tr("Zoom:"));
    QLineEdit* zoomEdit = new QLineEdit("100%");
    zoomEdit->setReadOnly(true);
    zoomLayout->addWidget(zoomLabel);
    zoomLayout->addWidget(zoomEdit);
    
    // 4. 画布颜色（QColorDialog）
    QHBoxLayout* colorLayout = new QHBoxLayout();
    QLabel* colorLabel = new QLabel(tr("Canvas Color:"));
    QPushButton* colorButton = new QPushButton();
    colorButton->setFixedSize(30, 30);
    QColor initialColor(Qt::white);
    QString qss = QString("background-color: %1").arg(initialColor.name());
    colorButton->setStyleSheet(qss);
    connect(colorButton, &QPushButton::clicked, this, [=]() {
        QColor color = QColorDialog::getColor(initialColor, this, tr("Select Canvas Color"));
        if (color.isValid()) {
            QString qss = QString("background-color: %1").arg(color.name());
            colorButton->setStyleSheet(qss);
            // Emit signal or call function to change canvas color
        }
    });
    colorLayout->addWidget(colorLabel);
    colorLayout->addWidget(colorButton);
    
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
    
    // 1. 边框宽度
    QHBoxLayout* borderWidthLayout = new QHBoxLayout();
    QLabel* borderWidthLabel = new QLabel(tr("Border Width:"));
    QSpinBox* borderWidthSpinBox = new QSpinBox();
    borderWidthSpinBox->setRange(0, 9);
    borderWidthSpinBox->setValue(1); // Default value
    borderWidthSpinBox->setSuffix(" px");
    borderWidthLayout->addWidget(borderWidthLabel);
    borderWidthLayout->addWidget(borderWidthSpinBox);
    
    // 2. 边框样式
    QHBoxLayout* borderStyleLayout = new QHBoxLayout();
    QLabel* borderStyleLabel = new QLabel(tr("Border Style:"));
    QComboBox* borderStyleComboBox = new QComboBox();
    borderStyleComboBox->addItem(tr("Solid Line"), static_cast<int>(Qt::SolidLine));
    borderStyleComboBox->addItem(tr("Dash Line"), static_cast<int>(Qt::DashLine));
    borderStyleComboBox->addItem(tr("Dot Line"), static_cast<int>(Qt::DotLine));
    borderStyleComboBox->addItem(tr("Dash Dot Line"), static_cast<int>(Qt::DashDotLine));
    borderStyleComboBox->addItem(tr("Dash Dot Dot Line"), static_cast<int>(Qt::DashDotDotLine));
    borderStyleLayout->addWidget(borderStyleLabel);
    borderStyleLayout->addWidget(borderStyleComboBox);
    
    // 3. 边框颜色
    QHBoxLayout* borderColorLayout = new QHBoxLayout();
    QLabel* borderColorLabel = new QLabel(tr("Border Color:"));
    QPushButton* borderColorButton = new QPushButton();
    borderColorButton->setFixedSize(30, 30);
    QColor initialBorderColor(Qt::black);
    QString borderQss = QString("background-color: %1").arg(initialBorderColor.name());
    borderColorButton->setStyleSheet(borderQss);
    connect(borderColorButton, &QPushButton::clicked, this, [=]() {
        QColor color = QColorDialog::getColor(initialBorderColor, this, tr("Select Border Color"));
        if (color.isValid()) {
            QString qss = QString("background-color: %1").arg(color.name());
            borderColorButton->setStyleSheet(qss);
            // Emit signal or call function to change border color
        }
    });
    borderColorLayout->addWidget(borderColorLabel);
    borderColorLayout->addWidget(borderColorButton);
    
    // 4. 填充颜色
    QHBoxLayout* fillColorLayout = new QHBoxLayout();
    QLabel* fillColorLabel = new QLabel(tr("Fill Color:"));
    QPushButton* fillColorButton = new QPushButton();
    fillColorButton->setFixedSize(30, 30);
    QColor initialFillColor(Qt::white);
    QString fillQss = QString("background-color: %1").arg(initialFillColor.name());
    fillColorButton->setStyleSheet(fillQss);
    connect(fillColorButton, &QPushButton::clicked, this, [=]() {
        QColor color = QColorDialog::getColor(initialFillColor, this, tr("Select Fill Color"));
        if (color.isValid()) {
            QString qss = QString("background-color: %1").arg(color.name());
            fillColorButton->setStyleSheet(qss);
            // Emit signal or call function to change fill color
        }
    });
    fillColorLayout->addWidget(fillColorLabel);
    fillColorLayout->addWidget(fillColorButton);
    
    layout->addLayout(borderWidthLayout);
    layout->addLayout(borderStyleLayout);
    layout->addLayout(borderColorLayout);
    layout->addLayout(fillColorLayout);
    layout->addStretch();
    
    return widget;
}
