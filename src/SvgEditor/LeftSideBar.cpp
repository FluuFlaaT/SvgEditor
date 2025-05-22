#include "LeftSideBar.h"
#include <QVBoxLayout>
#include <QPushButton>
#include <QLabel>

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

    // Create zoom section with only Zoom In button
    QLabel* zoomLabel = new QLabel("Zoom", this);
    zoomLabel->setAlignment(Qt::AlignCenter);
    zoomLabel->setStyleSheet("font-weight: bold;");
    mainLayout->addWidget(zoomLabel);

    zoomInBtn = new QPushButton("Zoom In", this);
    zoomInBtn->setToolTip(tr("Increase zoom level (Ctrl++)"));

    // Add zoom button to layout
    mainLayout->addWidget(zoomInBtn);

    // Connect zoom button signal
    connect(zoomInBtn, &QPushButton::clicked, this, &LeftSideBar::zoomInRequested);

    mainLayout->addStretch();

    setFixedWidth(200);
    setMinimumHeight(400);
}

LeftSideBar::~LeftSideBar()
{
    // Cleanup resources if needed
}