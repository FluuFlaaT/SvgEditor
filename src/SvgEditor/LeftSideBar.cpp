#include "LeftSideBar.h"
#include <QVBoxLayout>
#include <QPushButton>
#include <QLabel>

LeftSideBar::LeftSideBar(QWidget *parent)
    : QWidget(parent)
{
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    setLayout(mainLayout);

    selectBtn = new QPushButton("Select", this);
    drawBtn = new QPushButton("Draw", this);
    shapeBtn = new QPushButton("Shapes", this);
    textBtn = new QPushButton("Text", this);

    for(auto btn : btnGroup) {
        mainLayout->addWidget(*btn);
    }

    mainLayout->addStretch();

    setFixedWidth(200);
    setMinimumHeight(400);
}

LeftSideBar::~LeftSideBar()
{
    // Cleanup resources if needed
}