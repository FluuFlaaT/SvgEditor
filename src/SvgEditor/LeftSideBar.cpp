#include "LeftSideBar.h"
#include <QVBoxLayout>
#include <QPushButton>
#include <QLabel>

LeftSideBar::LeftSideBar(QWidget *parent)
    : QWidget(parent)
{
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    setLayout(mainLayout);

    QPushButton *selectBtn = new QPushButton("Select", this);
    QPushButton *drawBtn = new QPushButton("Draw", this);
    QPushButton *shapeBtn = new QPushButton("Shapes", this);
    QPushButton *textBtn = new QPushButton("Text", this);

    mainLayout->addWidget(selectBtn);
    mainLayout->addWidget(drawBtn);
    mainLayout->addWidget(shapeBtn);
    mainLayout->addWidget(textBtn);

    mainLayout->addStretch();

    setFixedWidth(200);
    setMinimumHeight(400);
}

LeftSideBar::~LeftSideBar()
{
    // Cleanup resources if needed
}