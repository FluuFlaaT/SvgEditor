#include "RightAttrBar.h"
#include <QFormLayout>
#include <QLineEdit>
#include <QColorDialog>
#include <QPushButton>
#include <QDoubleSpinBox>
#include <QComboBox>
#include <QGroupBox>
#include <QEvent>
#include "LoggingService.h"

RightAttrBar::RightAttrBar(QWidget *parent)
    : QWidget(parent)
{
    setMinimumWidth(220);
    setMaximumWidth(300); 
    
    m_mainLayout = new QVBoxLayout(this);
    m_mainLayout->setContentsMargins(5, 10, 5, 10);
    m_mainLayout->setSpacing(10);
    
    m_stackedWidget = new QStackedWidget(this);

    m_stackedWidget->addWidget(createCommonAttributesWidget());
    m_stackedWidget->addWidget(createCircleAttributesWidget());
    m_stackedWidget->addWidget(createRectangleAttributesWidget());
    m_stackedWidget->addWidget(createLineAttributesWidget());
    
    m_mainLayout->addWidget(m_stackedWidget);
    
    m_mainLayout->addStretch();
    
    LoggingService::getInstance().debug("RightAttrBar constructed");
}

RightAttrBar::~RightAttrBar()
{
    LoggingService::getInstance().debug("RightAttrBar destroyed");
}

void RightAttrBar::showAttributesForTool(int toolId)
{
    switch (toolId) {
        case 2: // TOOL_LINE
            // m_titleLabel->setText(tr("Line Properties"));
            m_stackedWidget->setCurrentIndex(3); // 线条属性面板
            break;
        case 3: // TOOL_RECTANGLE
            // m_titleLabel->setText(tr("Rectangle Properties"));
            m_stackedWidget->setCurrentIndex(2); // 矩形属性面板
            break;
        case 4: // TOOL_CIRCLE
            // m_titleLabel->setText(tr("Circle Properties"));
            m_stackedWidget->setCurrentIndex(1); // 圆形属性面板
            break;
        default:
            // m_titleLabel->setText(tr("Common Properties"));
            m_stackedWidget->setCurrentIndex(0); // 通用属性面板
            break;
    }
    
    LoggingService::getInstance().debug("Showing attributes for tool ID: " + std::to_string(toolId));
}

void RightAttrBar::showAttributesForElement(int elementType)
{
    // 类似于showAttributesForTool，但用于显示选中元素的属性
    showAttributesForTool(elementType);
}

QWidget* RightAttrBar::createCommonAttributesWidget()
{
    QWidget* widget = new QWidget(this);
    QVBoxLayout* layout = new QVBoxLayout(widget);
    
    QGroupBox* commonGroup = new QGroupBox(tr("Common Properties"), widget);
    QFormLayout* commonForm = new QFormLayout(commonGroup);
    
    // 画布填充颜色
    // TODO: SVG颜色默认用白色，但是不能一直是白色，to be fixed
    QPushButton* fillColorBtn = new QPushButton(tr("Select"), widget);
    fillColorBtn->setStyleSheet("background-color: white;");
    connect(fillColorBtn, &QPushButton::clicked, this, [fillColorBtn]() {
        // QColorDialog colorSelect(nullptr);
        QColor colorFill = QColorDialog::getColor(Qt::white, nullptr, tr("Select Fill Color"));
        if (colorFill.isValid()) {
            fillColorBtn->setStyleSheet("background-color: " + colorFill.name() + ";");
            LoggingService::getInstance().debug("Set Fill Color to: " + colorFill.name().toStdString());
        }
    });
    QLabel* fillColorLabel = new QLabel(tr("Fill Color:"), widget);
    fillColorLabel->setToolTip(tr("Fill Color"));
    commonForm->addRow(fillColorLabel, fillColorBtn);
    
    // 描边颜色 - 添加工具提示
    QPushButton* strokeColorBtn = new QPushButton(tr("Select"), widget);
    strokeColorBtn->setStyleSheet("background-color: black;");
    connect(strokeColorBtn, &QPushButton::clicked, this, [strokeColorBtn]() {
        QColor colorStroke = QColorDialog::getColor(Qt::black, nullptr, tr("Select Stroke Color"));
        if (colorStroke.isValid()) {
            strokeColorBtn->setStyleSheet("background-color: " + colorStroke.name() + ";");
            LoggingService::getInstance().debug("Set Fill Color to: " + colorStroke.name().toStdString());
        }
    });
    QLabel* strokeColorLabel = new QLabel(tr("Stroke Color:"), widget);
    strokeColorLabel->setToolTip(tr("Stroke Color"));
    commonForm->addRow(strokeColorLabel, strokeColorBtn);
    
    // 描边宽度 - 添加工具提示
    QLabel* strokeWidthLabel = new QLabel(tr("Stroke Width:"), widget);
    strokeWidthLabel->setToolTip(tr("Stroke Width"));
    
    QDoubleSpinBox* strokeWidthSpin = new QDoubleSpinBox(widget);
    strokeWidthSpin->setRange(0.1, 10.0);
    strokeWidthSpin->setSingleStep(0.1);
    strokeWidthSpin->setValue(1.0);
    strokeWidthSpin->setToolTip(tr("Stroke Width"));
    commonForm->addRow(strokeWidthLabel, strokeWidthSpin);
    
    // 透明度 - 添加工具提示
    QLabel* opacityLabel = new QLabel(tr("Opacity:"), widget);
    opacityLabel->setToolTip(tr("Opacity"));
    
    QDoubleSpinBox* opacitySpin = new QDoubleSpinBox(widget);
    opacitySpin->setRange(0.0, 1.0);
    opacitySpin->setSingleStep(0.1);
    opacitySpin->setValue(1.0);
    opacitySpin->setToolTip(tr("Opacity"));
    commonForm->addRow(opacityLabel, opacitySpin);
    
    // 这部分代码已被上面的代码替代
    
    layout->addWidget(commonGroup);
    layout->addStretch();
    
    return widget;
}

QWidget* RightAttrBar::createCircleAttributesWidget()
{
    QWidget* widget = new QWidget(this);
    QVBoxLayout* layout = new QVBoxLayout(widget);
    
    // 通用属性组（复用） 
    layout->addWidget(createCommonAttributesWidget());
    
    // 圆形特有属性组 
    QGroupBox* circleGroup = new QGroupBox(tr("Circle Properties"), widget);
    QFormLayout* circleForm = new QFormLayout(circleGroup);
    
    // 圆心X 
    QDoubleSpinBox* centerXSpin = new QDoubleSpinBox(widget);
    centerXSpin->setRange(0, 1000);
    centerXSpin->setValue(200);
    circleForm->addRow(tr("Center X:"), centerXSpin);
    
    // 圆心Y 
    QDoubleSpinBox* centerYSpin = new QDoubleSpinBox(widget);
    centerYSpin->setRange(0, 1000);
    centerYSpin->setValue(150);
    circleForm->addRow(tr("Center Y:"), centerYSpin);
    
    // 半径 
    QDoubleSpinBox* radiusSpin = new QDoubleSpinBox(widget);
    radiusSpin->setRange(0, 500);
    radiusSpin->setValue(50);
    circleForm->addRow(tr("Radius:"), radiusSpin);
    
    layout->addWidget(circleGroup);
    layout->addStretch();
    
    return widget;
}

QWidget* RightAttrBar::createRectangleAttributesWidget()
{
    QWidget* widget = new QWidget(this);
    QVBoxLayout* layout = new QVBoxLayout(widget);
    
    // 通用属性组（复用） 
    layout->addWidget(createCommonAttributesWidget());
    
    // 矩形特有属性组 
    QGroupBox* rectGroup = new QGroupBox(tr("Rectangle Properties"), widget);
    QFormLayout* rectForm = new QFormLayout(rectGroup);
    
    // 左上角X 
    QDoubleSpinBox* topLeftXSpin = new QDoubleSpinBox(widget);
    topLeftXSpin->setRange(0, 1000);
    topLeftXSpin->setValue(100);
    rectForm->addRow(tr("Top Left X:"), topLeftXSpin);
    
    // 左上角Y 
    QDoubleSpinBox* topLeftYSpin = new QDoubleSpinBox(widget);
    topLeftYSpin->setRange(0, 1000);
    topLeftYSpin->setValue(100);
    rectForm->addRow(tr("Top Left Y:"), topLeftYSpin);
    
    // 宽度 
    QDoubleSpinBox* widthSpin = new QDoubleSpinBox(widget);
    widthSpin->setRange(0, 1000);
    widthSpin->setValue(200);
    rectForm->addRow(tr("Width:"), widthSpin);
    
    // 高度 
    QDoubleSpinBox* heightSpin = new QDoubleSpinBox(widget);
    heightSpin->setRange(0, 1000);
    heightSpin->setValue(150);
    rectForm->addRow(tr("Height:"), heightSpin);
    
    // 圆角 
    QDoubleSpinBox* cornerRadiusSpin = new QDoubleSpinBox(widget);
    cornerRadiusSpin->setRange(0, 100);
    cornerRadiusSpin->setValue(0);
    rectForm->addRow(tr("Corner Radius:"), cornerRadiusSpin);
    
    layout->addWidget(rectGroup);
    layout->addStretch();
    
    return widget;
}

QWidget* RightAttrBar::createLineAttributesWidget()
{
    QWidget* widget = new QWidget(this);
    QVBoxLayout* layout = new QVBoxLayout(widget);
    
    // 通用属性组（复用） 
    layout->addWidget(createCommonAttributesWidget());
    
    // 线条特有属性组 
    QGroupBox* lineGroup = new QGroupBox(tr("Line Properties"), widget);
    QFormLayout* lineForm = new QFormLayout(lineGroup);
    
    // 起点X 
    QDoubleSpinBox* startXSpin = new QDoubleSpinBox(widget);
    startXSpin->setRange(0, 1000);
    startXSpin->setValue(100);
    lineForm->addRow(tr("Start X:"), startXSpin);
    
    // 起点Y 
    QDoubleSpinBox* startYSpin = new QDoubleSpinBox(widget);
    startYSpin->setRange(0, 1000);
    startYSpin->setValue(100);
    lineForm->addRow(tr("Start Y:"), startYSpin);
    
    // 终点X 
    QDoubleSpinBox* endXSpin = new QDoubleSpinBox(widget);
    endXSpin->setRange(0, 1000);
    endXSpin->setValue(300);
    lineForm->addRow(tr("End X:"), endXSpin);
    
    // 终点Y 
    QDoubleSpinBox* endYSpin = new QDoubleSpinBox(widget);
    endYSpin->setRange(0, 1000);
    endYSpin->setValue(200);
    lineForm->addRow(tr("End Y:"), endYSpin);
    
    layout->addWidget(lineGroup);
    layout->addStretch();
    
    return widget;
}

void RightAttrBar::changeEvent(QEvent* event)
{
    if (event->type() == QEvent::LanguageChange) {
        // 更新属性面板中的所有文本
        // 查找并更新所有组框标题
        QList<QGroupBox*> groupBoxes = findChildren<QGroupBox*>();
        for (QGroupBox* groupBox : groupBoxes) {
            if (groupBox->title() == "Common Properties") {
                groupBox->setTitle(tr("Common Properties"));
            }
        }
        
        // 更新所有标签文本
        QList<QLabel*> labels = findChildren<QLabel*>();
        for (QLabel* label : labels) {
            if (label->text().endsWith(':')) {
                if (label->text() == "Fill Color:") {
                    label->setText(tr("Fill Color:"));
                    label->setToolTip(tr("Fill Color"));
                } else if (label->text() == "Stroke Color:") {
                    label->setText(tr("Stroke Color:"));
                    label->setToolTip(tr("Stroke Color"));
                } else if (label->text() == "Stroke Width:") {
                    label->setText(tr("Stroke Width:"));
                } else if (label->text() == "Opacity:") {
                    label->setText(tr("Opacity:"));
                }
            }
        }
        
        // 更新所有按钮文本
        QList<QPushButton*> buttons = findChildren<QPushButton*>();
        for (QPushButton* button : buttons) {
            if (button->text() == "Select") {
                button->setText(tr("Select"));
            }
        }
        
        LoggingService::getInstance().debug("RightAttrBar language changed");
    }
    
    QWidget::changeEvent(event);
}
