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

    // Add tool buttons to layout
    QLabel* toolsLabel = new QLabel("Tools", this);
    toolsLabel->setAlignment(Qt::AlignCenter);
    toolsLabel->setStyleSheet("font-weight: bold;");
    mainLayout->addWidget(toolsLabel);

    for(auto btn : btnGroup) {
        mainLayout->addWidget(*btn);
    }

    mainLayout->addSpacing(20);

    // Create zoom buttons
    QLabel* zoomLabel = new QLabel("Zoom", this);
    zoomLabel->setAlignment(Qt::AlignCenter);
    zoomLabel->setStyleSheet("font-weight: bold;");
    mainLayout->addWidget(zoomLabel);

    zoomInBtn = new QPushButton("Zoom In", this);
    zoomOutBtn = new QPushButton("Zoom Out", this);
    zoomResetBtn = new QPushButton("Reset Zoom", this);
    zoomFitBtn = new QPushButton("Fit to Window", this);

    // Add zoom buttons to layout
    mainLayout->addWidget(zoomInBtn);
    mainLayout->addWidget(zoomOutBtn);
    mainLayout->addWidget(zoomResetBtn);
    mainLayout->addWidget(zoomFitBtn);

    // Connect zoom button signals
    connect(zoomInBtn, &QPushButton::clicked, this, &LeftSideBar::zoomInRequested);
    connect(zoomOutBtn, &QPushButton::clicked, this, &LeftSideBar::zoomOutRequested);
    connect(zoomResetBtn, &QPushButton::clicked, this, &LeftSideBar::zoomResetRequested);
    connect(zoomFitBtn, &QPushButton::clicked, this, &LeftSideBar::zoomFitRequested);

    mainLayout->addStretch();

    setFixedWidth(200);
    setMinimumHeight(400);
}

LeftSideBar::~LeftSideBar()
{
    // Cleanup resources if needed
}