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

    // Create zoom section with menu
    QLabel* zoomLabel = new QLabel("Zoom", this);
    zoomLabel->setAlignment(Qt::AlignCenter);
    zoomLabel->setStyleSheet("font-weight: bold;");
    mainLayout->addWidget(zoomLabel);

    zoomMenuBtn = new QToolButton(this);
    zoomMenuBtn->setText(tr("Zoom Menu"));
    zoomMenuBtn->setPopupMode(QToolButton::InstantPopup);
    zoomMenu = new QMenu(this);
    zoomInAction = zoomMenu->addAction(tr("Zoom In"));
    zoomOutAction = zoomMenu->addAction(tr("Zoom Out"));
    resetZoomAction = zoomMenu->addAction(tr("Reset Zoom"));
    fitToWindowAction = zoomMenu->addAction(tr("Fit to Window"));
    zoomMenuBtn->setMenu(zoomMenu);
    mainLayout->addWidget(zoomMenuBtn);

    // Connect zoom actions to signals
    connect(zoomInAction, &QAction::triggered, this, &LeftSideBar::zoomInRequested);
    connect(zoomOutAction, &QAction::triggered, this, &LeftSideBar::zoomOutRequested);
    connect(resetZoomAction, &QAction::triggered, this, &LeftSideBar::resetZoomRequested);
    connect(fitToWindowAction, &QAction::triggered, this, &LeftSideBar::fitToWindowRequested);

    mainLayout->addStretch();

    setFixedWidth(200);
    setMinimumHeight(400);
}

LeftSideBar::~LeftSideBar()
{
    // Cleanup resources if needed
}