#include "leftsidebar.h"
#include "CustomTooltip.h"
#include "../ConfigManager/ButtonTipManager.h"
#include <QVBoxLayout>
#include <QPushButton>
#include <QLabel>
#include <QToolButton>
#include <QMenu>
#include <QAction>
#include <QButtonGroup>
#include <QEvent>
#include <QMouseEvent>

Q_LOGGING_CATEGORY(leftSideBarLog, "LeftSideBar")

LeftSideBar::LeftSideBar(QWidget *parent)
    : QWidget(parent), m_selectedShapeType(ShapeType::None)
{
    // Initialize tooltip components
    m_tooltip = new CustomTooltip(this);
    m_tipManager = ButtonTipManager::instance();
    
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    setLayout(mainLayout);

    // Logical workflow order: select, draw, annotate, view
    dragBtn = new QPushButton("Drag", this);
    shapeBtn = new QPushButton("Shapes", this);
    textBtn = new QPushButton("Text", this);
    zoomBtn = new QPushButton("Zoom", this);

    dragBtn->setCheckable(true);
    shapeBtn->setCheckable(true);
    textBtn->setCheckable(true);
    zoomBtn->setCheckable(true);

    // dragBtn->setToolTip(tr("Pan/scroll the canvas view"));
    // shapeBtn->setToolTip(tr("Create shapes"));
    // textBtn->setToolTip(tr("Add text"));
    // zoomBtn->setToolTip(tr("Zoom controls"));

    dragBtn->setCursor(Qt::OpenHandCursor);

    // Exclusive selection prevents conflicting tool states
    m_toolButtonGroup = new QButtonGroup(this);
    m_toolButtonGroup->addButton(dragBtn, 0);
    m_toolButtonGroup->addButton(shapeBtn, 1);
    m_toolButtonGroup->addButton(textBtn, 2);
    m_toolButtonGroup->addButton(zoomBtn, 3);
    m_toolButtonGroup->setExclusive(true);

    connect(dragBtn, &QPushButton::clicked, this, &LeftSideBar::dragToolRequested);
    connect(shapeBtn, &QPushButton::clicked, this, &LeftSideBar::shapeGroupRequested);
    connect(textBtn, &QPushButton::clicked, this, &LeftSideBar::textToolRequested);
    connect(zoomBtn, &QPushButton::clicked, this, &LeftSideBar::zoomToolRequested);

    QLabel* toolsLabel = new QLabel("Tools", this);
    toolsLabel->setAlignment(Qt::AlignCenter);
    toolsLabel->setStyleSheet("font-weight: bold;");
    mainLayout->addWidget(toolsLabel);

    mainLayout->addWidget(dragBtn);
    mainLayout->addWidget(shapeBtn);

    // Collapsible panels save screen real estate for primary tools
    shapeToolsWidget = new QWidget(this);
    QVBoxLayout* shapeLayout = new QVBoxLayout(shapeToolsWidget);
    shapeLayout->setContentsMargins(5, 5, 5, 5);
    shapeLayout->setSpacing(5);

    QLabel* shapeToolsLabel = new QLabel("Shape Tools", shapeToolsWidget);
    shapeToolsLabel->setAlignment(Qt::AlignCenter);
    shapeToolsLabel->setStyleSheet("font-weight: bold;");
    shapeLayout->addWidget(shapeToolsLabel);

    lineBtn = new QPushButton(tr("Line"), shapeToolsWidget);
    freehandBtn = new QPushButton(tr("Freehand"), shapeToolsWidget);
    ellipseBtn = new QPushButton(tr("Ellipse"), shapeToolsWidget);
    pentagonBtn = new QPushButton(tr("Pentagon"), shapeToolsWidget);
    starBtn = new QPushButton(tr("Star"), shapeToolsWidget);
    hexagonBtn = new QPushButton(tr("Hexagon"), shapeToolsWidget);

    // lineBtn->setToolTip(tr("Create straight lines"));
    // freehandBtn->setToolTip(tr("Draw freehand paths"));
    // ellipseBtn->setToolTip(tr("Create circles and ellipses"));
    // pentagonBtn->setToolTip(tr("Create regular pentagons"));
    // starBtn->setToolTip(tr("Create five-pointed stars"));
    // hexagonBtn->setToolTip(tr("Create regular hexagons"));

    shapeBtnGroup = {lineBtn, freehandBtn, ellipseBtn, pentagonBtn, starBtn, hexagonBtn};

    shapeLayout->addWidget(lineBtn);
    shapeLayout->addWidget(freehandBtn);
    shapeLayout->addWidget(ellipseBtn);
    shapeLayout->addWidget(pentagonBtn);
    shapeLayout->addWidget(starBtn);
    shapeLayout->addWidget(hexagonBtn);

    mainLayout->addWidget(shapeToolsWidget);
    shapeToolsWidget->hide();

    // Lambdas combine state management with signal emission efficiently
    connect(lineBtn, &QPushButton::clicked, this, [this]() {
        setSelectedShapeType(ShapeType::Line);
        highlightShapeButton(0);
        emit shapeToolSelected(ShapeType::Line);
    });
    
    connect(freehandBtn, &QPushButton::clicked, this, [this]() {
        setSelectedShapeType(ShapeType::Freehand);
        highlightShapeButton(1);
        emit shapeToolSelected(ShapeType::Freehand);
    });
    
    connect(ellipseBtn, &QPushButton::clicked, this, [this]() {
        setSelectedShapeType(ShapeType::Ellipse);
        highlightShapeButton(2);
        emit shapeToolSelected(ShapeType::Ellipse);
    });
    
    connect(pentagonBtn, &QPushButton::clicked, this, [this]() {
        setSelectedShapeType(ShapeType::Pentagon);
        highlightShapeButton(3);
        emit shapeToolSelected(ShapeType::Pentagon);
    });
    
    connect(starBtn, &QPushButton::clicked, this, [this]() {
        setSelectedShapeType(ShapeType::Star);
        highlightShapeButton(4);
        emit shapeToolSelected(ShapeType::Star);
    });
    
    connect(hexagonBtn, &QPushButton::clicked, this, [this]() {
        setSelectedShapeType(ShapeType::Hexagon);
        highlightShapeButton(5);
        emit shapeToolSelected(ShapeType::Hexagon);
    });

    mainLayout->addWidget(textBtn);
    mainLayout->addWidget(zoomBtn);

    // Separate zoom widget reduces cognitive load for shape-focused users
    zoomToolsWidget = new QWidget(this);
    QVBoxLayout* zoomLayout = new QVBoxLayout(zoomToolsWidget);
    zoomLayout->setContentsMargins(5, 5, 5, 5);
    zoomLayout->setSpacing(5);

    QLabel* zoomToolsLabel = new QLabel("Zoom Tools", zoomToolsWidget);
    zoomToolsLabel->setAlignment(Qt::AlignCenter);
    zoomToolsLabel->setStyleSheet("font-weight: bold;");
    zoomLayout->addWidget(zoomToolsLabel);

    zoomInBtn = new QPushButton(tr("Zoom In"), zoomToolsWidget);
    zoomOutBtn = new QPushButton(tr("Zoom Out"), zoomToolsWidget);
    resetZoomBtn = new QPushButton(tr("Reset Zoom"), zoomToolsWidget);
    fitToWindowBtn = new QPushButton(tr("Fit to Window"), zoomToolsWidget);

    zoomBtnGroup = {zoomInBtn, zoomOutBtn, resetZoomBtn, fitToWindowBtn};

    zoomLayout->addWidget(zoomInBtn);
    zoomLayout->addWidget(zoomOutBtn);
    zoomLayout->addWidget(resetZoomBtn);
    zoomLayout->addWidget(fitToWindowBtn);

    mainLayout->addWidget(zoomToolsWidget);
    zoomToolsWidget->hide();

    connect(zoomInBtn, &QPushButton::clicked, this, [this]() {
        highlightZoomButton(0);
        emit zoomInRequested();
    });

    connect(zoomOutBtn, &QPushButton::clicked, this, [this]() {
        highlightZoomButton(1);
        emit zoomOutRequested();
    });

    connect(resetZoomBtn, &QPushButton::clicked, this, [this]() {
        highlightZoomButton(2);
        emit resetZoomRequested();
    });

    connect(fitToWindowBtn, &QPushButton::clicked, this, [this]() {
        highlightZoomButton(3);
        emit fitToWindowRequested();
    });

    mainLayout->addStretch();

    setFixedWidth(200);
    setMinimumHeight(400);

    // Setup button tooltips
    setupButtonTooltips();
    setupButtonEventFilters();
    
    // Connect to tip manager signals
    connect(m_tipManager, &ButtonTipManager::tipsLoaded, this, &LeftSideBar::onButtonTipsLoaded);

    qCDebug(leftSideBarLog) << "LeftSideBar constructed with new layout: Drag, Shapes, Text, Zoom";
}

LeftSideBar::~LeftSideBar()
{
    // Qt automatically cleans up child widgets and layout objects
}

void LeftSideBar::highlightButton(int toolId)
{
    // Ensure the correct button is checked in the button group
    if (toolId >= 0 && toolId < btnGroup.size()) {
        m_toolButtonGroup->button(toolId)->setChecked(true);

        // Show/hide sub-tools widgets based on selection
        shapeToolsWidget->setVisible(toolId == 1); // 1 is the index for shape button
        zoomToolsWidget->setVisible(toolId == 3);  // 3 is the index for zoom button
        shapeExpanded = (toolId == 1);
        zoomExpanded = (toolId == 3);

        qCDebug(leftSideBarLog) << "Tool button highlighted:" << toolId;
    }
}

void LeftSideBar::highlightShapeButton(int btnIndex)
{
    // Reset all shape buttons
    for (int i = 0; i < shapeBtnGroup.size(); ++i) {
        if (shapeBtnGroup[i]) {
            shapeBtnGroup[i]->setStyleSheet("");
        }
    }

    // Highlight the selected shape button
    if (btnIndex >= 0 && btnIndex < shapeBtnGroup.size() && shapeBtnGroup[btnIndex]) {
        shapeBtnGroup[btnIndex]->setStyleSheet("background-color: #AED6F1; font-weight: bold;");
        lastSelectedShapeBtn = btnIndex;

        qCDebug(leftSideBarLog) << "Shape button highlighted:" << btnIndex;
    }
}

void LeftSideBar::highlightZoomButton(int btnIndex)
{
    // Reset all zoom buttons
    for (int i = 0; i < zoomBtnGroup.size(); ++i) {
        if (zoomBtnGroup[i]) {
            zoomBtnGroup[i]->setStyleSheet("");
        }
    }

    // Highlight the selected zoom button
    if (btnIndex >= 0 && btnIndex < zoomBtnGroup.size() && zoomBtnGroup[btnIndex]) {
        zoomBtnGroup[btnIndex]->setStyleSheet("background-color: #AED6F1; font-weight: bold;");
        lastSelectedZoomBtn = btnIndex;

        qCDebug(leftSideBarLog) << "Zoom button highlighted:" << btnIndex;
    }
}

void LeftSideBar::setSelectedShapeType(ShapeType type)
{
    m_selectedShapeType = type;
    qCDebug(leftSideBarLog) << "Selected shape type:" << static_cast<int>(type);
}

void LeftSideBar::setupButtonTooltips()
{
    // Map buttons to their corresponding API IDs
    m_buttonIdMap[dragBtn] = "selectionbutton";
    m_buttonIdMap[lineBtn] = "linedrawbutton";
    m_buttonIdMap[freehandBtn] = "freehandlinedrawbutton";
    m_buttonIdMap[ellipseBtn] = "rectdrawbutton";  // Using rect as fallback for ellipse
    m_buttonIdMap[pentagonBtn] = "pentagondrawbutton";
    m_buttonIdMap[starBtn] = "stardrawbutton";
    m_buttonIdMap[zoomInBtn] = "zoominbutton";
    m_buttonIdMap[zoomOutBtn] = "zoomoutbutton";
    
    qCDebug(leftSideBarLog) << "Button tooltip mapping setup completed";
}

void LeftSideBar::setupButtonEventFilters()
{
    // Install event filters for all buttons that have tooltip mappings
    for (auto it = m_buttonIdMap.begin(); it != m_buttonIdMap.end(); ++it) {
        it.key()->installEventFilter(this);
    }
    
    qCDebug(leftSideBarLog) << "Event filters installed for" << m_buttonIdMap.size() << "buttons";
}

QString LeftSideBar::getButtonId(QPushButton* button) const
{
    return m_buttonIdMap.value(button, QString());
}

void LeftSideBar::showButtonTooltip(QPushButton* button, const QPoint& position)
{
    QString buttonId = getButtonId(button);
    if (buttonId.isEmpty()) {
        return;
    }
    
    ButtonTipInfo tipInfo = m_tipManager->getButtonTip(buttonId);
    m_tooltip->scheduleShow(tipInfo.title, tipInfo.text, position);
    
    qCDebug(leftSideBarLog) << "Showing tooltip for button:" << buttonId;
}

void LeftSideBar::hideButtonTooltip()
{
    m_tooltip->scheduleHide();
}

bool LeftSideBar::eventFilter(QObject* obj, QEvent* event)
{
    QPushButton* button = qobject_cast<QPushButton*>(obj);
    if (!button || !m_buttonIdMap.contains(button)) {
        return QWidget::eventFilter(obj, event);
    }
    
    switch (event->type()) {
    case QEvent::Enter:
        {
            // Position tooltip near the right edge of the button
            QPoint buttonTopRight = button->rect().topRight();
            QPoint globalPos = button->mapToGlobal(buttonTopRight);
            showButtonTooltip(button, globalPos);
        }
        break;
    case QEvent::Leave:
        hideButtonTooltip();
        break;
    default:
        break;
    }
    
    return QWidget::eventFilter(obj, event);
}

void LeftSideBar::onButtonTipsLoaded()
{
    qCDebug(leftSideBarLog) << "Button tips loaded from server";
    // Tips are now available, no additional action needed
    // The tooltip will automatically use the loaded tips
}