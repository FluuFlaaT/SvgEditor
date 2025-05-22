#include "LeftSideBar.h"
#include <QVBoxLayout>
#include <QPushButton>
#include <QLabel>
#include <QToolButton>
#include <QMenu>
#include <QAction>
#include <QButtonGroup>

Q_LOGGING_CATEGORY(leftSideBarLog, "LeftSideBar")

LeftSideBar::LeftSideBar(QWidget *parent)
    : QWidget(parent)
{
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    setLayout(mainLayout);

    // Create tool buttons
    selectBtn = new QPushButton("Select", this);
    drawBtn = new QPushButton("Draw", this);
    shapeBtn = new QPushButton("Shapes", this);
    textBtn = new QPushButton("Text", this);
    dragBtn = new QPushButton("Drag", this);
    zoomBtn = new QPushButton("Zoom", this);

    // Make tool buttons checkable for radio button-like behavior
    selectBtn->setCheckable(true);
    drawBtn->setCheckable(true);
    shapeBtn->setCheckable(true);
    textBtn->setCheckable(true);
    dragBtn->setCheckable(true);
    zoomBtn->setCheckable(true);

    // Set tooltips for buttons
    selectBtn->setToolTip(tr("Select and manipulate objects"));
    drawBtn->setToolTip(tr("Draw freehand lines"));
    shapeBtn->setToolTip(tr("Create shapes"));
    textBtn->setToolTip(tr("Add text"));
    dragBtn->setToolTip(tr("Pan/scroll the canvas view"));
    zoomBtn->setToolTip(tr("Zoom controls"));

    // Set cursor for drag button to indicate its function
    dragBtn->setCursor(Qt::OpenHandCursor);

    // Create button group for mutually exclusive selection
    m_toolButtonGroup = new QButtonGroup(this);
    m_toolButtonGroup->addButton(selectBtn, 0);
    m_toolButtonGroup->addButton(drawBtn, 1);
    m_toolButtonGroup->addButton(shapeBtn, 2);
    m_toolButtonGroup->addButton(textBtn, 3);
    m_toolButtonGroup->addButton(dragBtn, 4);
    m_toolButtonGroup->addButton(zoomBtn, 5);
    m_toolButtonGroup->setExclusive(true);

    // Connect button signals
    connect(dragBtn, &QPushButton::clicked, this, &LeftSideBar::dragToolRequested);
    connect(zoomBtn, &QPushButton::clicked, this, &LeftSideBar::zoomToolRequested);

    // Add tool buttons to layout
    QLabel* toolsLabel = new QLabel("Tools", this);
    toolsLabel->setAlignment(Qt::AlignCenter);
    toolsLabel->setStyleSheet("font-weight: bold;");
    mainLayout->addWidget(toolsLabel);

    for(auto btn : btnGroup) {
        mainLayout->addWidget(*btn);
    }

    // Create zoom tools widget (initially hidden)
    zoomToolsWidget = new QWidget(this);
    QVBoxLayout* zoomLayout = new QVBoxLayout(zoomToolsWidget);
    zoomLayout->setContentsMargins(5, 5, 5, 5);
    zoomLayout->setSpacing(5);

    // Add a header label for Zoom Tools similar to Shape Tools
    QLabel* zoomToolsLabel = new QLabel("Zoom Tools", zoomToolsWidget);
    zoomToolsLabel->setAlignment(Qt::AlignCenter);
    zoomToolsLabel->setStyleSheet("font-weight: bold;");
    zoomLayout->addWidget(zoomToolsLabel);

    // Create zoom sub-buttons
    zoomInBtn = new QPushButton(tr("Zoom In"), zoomToolsWidget);
    zoomOutBtn = new QPushButton(tr("Zoom Out"), zoomToolsWidget);
    resetZoomBtn = new QPushButton(tr("Reset Zoom"), zoomToolsWidget);
    fitToWindowBtn = new QPushButton(tr("Fit to Window"), zoomToolsWidget);

    // Initialize the zoom button group
    zoomBtnGroup = {zoomInBtn, zoomOutBtn, resetZoomBtn, fitToWindowBtn};

    // Add zoom buttons to layout
    zoomLayout->addWidget(zoomInBtn);
    zoomLayout->addWidget(zoomOutBtn);
    zoomLayout->addWidget(resetZoomBtn);
    zoomLayout->addWidget(fitToWindowBtn);

    // Add zoom tools widget to main layout
    mainLayout->addWidget(zoomToolsWidget);
    zoomToolsWidget->hide(); // Initially hidden

    // Connect zoom buttons to signals with highlighting
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

    qCDebug(leftSideBarLog) << "LeftSideBar constructed with mutually exclusive tool buttons";
}

LeftSideBar::~LeftSideBar()
{
    // Cleanup resources if needed
}

void LeftSideBar::highlightButton(int toolId)
{
    // Ensure the correct button is checked in the button group
    if (toolId >= 0 && toolId < btnGroup.size()) {
        m_toolButtonGroup->button(toolId)->setChecked(true);

        // Show/hide zoom tools widget based on selection
        zoomToolsWidget->setVisible(toolId == 5); // 5 is the index for zoom button
        zoomExpanded = (toolId == 5);

        qCDebug(leftSideBarLog) << "Tool button highlighted:" << toolId;
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