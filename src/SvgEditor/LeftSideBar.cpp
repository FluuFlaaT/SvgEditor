#include "LeftSideBar.h"
#include <QVBoxLayout>
#include <QPushButton>
#include <QLabel>
#include <QToolButton>
#include <QMenu>
#include <QAction>

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

    // Set tooltips for buttons
    selectBtn->setToolTip(tr("Select and manipulate objects"));
    drawBtn->setToolTip(tr("Draw freehand lines"));
    shapeBtn->setToolTip(tr("Create shapes"));
    textBtn->setToolTip(tr("Add text"));
    dragBtn->setToolTip(tr("Pan/scroll the canvas view"));

    // Set cursor for drag button to indicate its function
    dragBtn->setCursor(Qt::OpenHandCursor);

    // Connect drag button signal
    connect(dragBtn, &QPushButton::clicked, this, &LeftSideBar::dragToolRequested);

    // Add tool buttons to layout
    QLabel* toolsLabel = new QLabel("Tools", this);
    toolsLabel->setAlignment(Qt::AlignCenter);
    toolsLabel->setStyleSheet("font-weight: bold;");
    mainLayout->addWidget(toolsLabel);

    for(auto btn : btnGroup) {
        mainLayout->addWidget(*btn);
    }

    mainLayout->addSpacing(20);

    // Create zoom section with toggle button
    QLabel* zoomLabel = new QLabel("Zoom", this);
    zoomLabel->setAlignment(Qt::AlignCenter);
    zoomLabel->setStyleSheet("font-weight: bold;");
    mainLayout->addWidget(zoomLabel);

    zoomToggleBtn = new QPushButton(tr("Zoom"), this);
    mainLayout->addWidget(zoomToggleBtn);

    zoomInBtn = new QPushButton(tr("Zoom In"), this);
    zoomOutBtn = new QPushButton(tr("Zoom Out"), this);
    resetZoomBtn = new QPushButton(tr("Reset Zoom"), this);
    fitToWindowBtn = new QPushButton(tr("Fit to Window"), this);

    // 初始隐藏四个按钮
    zoomInBtn->setVisible(false);
    zoomOutBtn->setVisible(false);
    resetZoomBtn->setVisible(false);
    fitToWindowBtn->setVisible(false);

    mainLayout->addWidget(zoomInBtn);
    mainLayout->addWidget(zoomOutBtn);
    mainLayout->addWidget(resetZoomBtn);
    mainLayout->addWidget(fitToWindowBtn);

    // Zoom主按钮点击展开/收起
    connect(zoomToggleBtn, &QPushButton::clicked, this, [this]() {
        zoomExpanded = !zoomExpanded;
        zoomInBtn->setVisible(zoomExpanded);
        zoomOutBtn->setVisible(zoomExpanded);
        resetZoomBtn->setVisible(zoomExpanded);
        fitToWindowBtn->setVisible(zoomExpanded);
    });

    // Connect zoom buttons to signals
    connect(zoomInBtn, &QPushButton::clicked, this, &LeftSideBar::zoomInRequested);
    connect(zoomOutBtn, &QPushButton::clicked, this, &LeftSideBar::zoomOutRequested);
    connect(resetZoomBtn, &QPushButton::clicked, this, &LeftSideBar::resetZoomRequested);
    connect(fitToWindowBtn, &QPushButton::clicked, this, &LeftSideBar::fitToWindowRequested);

    mainLayout->addStretch();

    setFixedWidth(200);
    setMinimumHeight(400);
}

LeftSideBar::~LeftSideBar()
{
    // Cleanup resources if needed
}