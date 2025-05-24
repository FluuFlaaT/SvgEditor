#include "shapetoolbar.h"

Q_LOGGING_CATEGORY(shapeToolBarLog, "ShapeToolBar")

ShapeToolBar::ShapeToolBar(QWidget *parent)
    : QWidget(parent),
    m_selectedShapeType(ShapeType::None)
{
    m_mainLayout = new QVBoxLayout(this);
    m_mainLayout->setContentsMargins(5, 10, 5, 10);
    m_mainLayout->setSpacing(5);

    // Create shape tool buttons
    lineBtn = new QPushButton("Line", this);
    freehandBtn = new QPushButton("Freehand", this);
    rectangleBtn = new QPushButton("Rectangle", this);
    ellipseBtn = new QPushButton("Ellipse", this);
    pentagonBtn = new QPushButton("Pentagon", this);
    starBtn = new QPushButton("Star", this);
    hexagonBtn = new QPushButton("Hexagon", this);

    // Set tooltips for buttons
    // lineBtn->setToolTip(tr("Create straight lines"));
    // freehandBtn->setToolTip(tr("Draw freehand paths"));
    // rectangleBtn->setToolTip(tr("Create rectangles"));
    // ellipseBtn->setToolTip(tr("Create circles and ellipses"));
    // pentagonBtn->setToolTip(tr("Create regular pentagons"));
    // starBtn->setToolTip(tr("Create five-pointed stars"));
    // hexagonBtn->setToolTip(tr("Create regular hexagons"));

    // Add title label
    QLabel* shapesLabel = new QLabel("Shape Tools", this);
    shapesLabel->setAlignment(Qt::AlignCenter);
    shapesLabel->setStyleSheet("font-weight: bold;");
    m_mainLayout->addWidget(shapesLabel);

    // Add buttons to layout
    m_mainLayout->addWidget(lineBtn);
    m_mainLayout->addWidget(freehandBtn);
    m_mainLayout->addWidget(rectangleBtn);
    m_mainLayout->addWidget(ellipseBtn);
    m_mainLayout->addWidget(pentagonBtn);
    m_mainLayout->addWidget(starBtn);
    m_mainLayout->addWidget(hexagonBtn);

    // Connect button signals
    connect(lineBtn, &QPushButton::clicked, this, [this]() {
        setSelectedShapeType(ShapeType::Line);
        emit shapeToolSelected(ShapeType::Line);
    });
    
    connect(freehandBtn, &QPushButton::clicked, this, [this]() {
        setSelectedShapeType(ShapeType::Freehand);
        emit shapeToolSelected(ShapeType::Freehand);
    });
    
    connect(rectangleBtn, &QPushButton::clicked, this, [this]() {
        setSelectedShapeType(ShapeType::Rectangle);
        emit shapeToolSelected(ShapeType::Rectangle);
    });
    
    connect(ellipseBtn, &QPushButton::clicked, this, [this]() {
        setSelectedShapeType(ShapeType::Ellipse);
        emit shapeToolSelected(ShapeType::Ellipse);
    });
    
    connect(pentagonBtn, &QPushButton::clicked, this, [this]() {
        setSelectedShapeType(ShapeType::Pentagon);
        emit shapeToolSelected(ShapeType::Pentagon);
    });
    
    connect(starBtn, &QPushButton::clicked, this, [this]() {
        setSelectedShapeType(ShapeType::Star);
        emit shapeToolSelected(ShapeType::Star);
    });
    
    connect(hexagonBtn, &QPushButton::clicked, this, [this]() {
        setSelectedShapeType(ShapeType::Hexagon);
        emit shapeToolSelected(ShapeType::Hexagon);
    });

    m_mainLayout->addStretch();
    setFixedWidth(200);
    setMinimumHeight(300);
    
    qCDebug(shapeToolBarLog) << "ShapeToolBar constructed";
}

ShapeToolBar::~ShapeToolBar()
{
    qCDebug(shapeToolBarLog) << "ShapeToolBar destroyed";
}

void ShapeToolBar::setSelectedShapeType(ShapeType type)
{
    m_selectedShapeType = type;
    highlightButton(type);
    qCDebug(shapeToolBarLog) << "Selected shape type:" << static_cast<int>(type);
}

void ShapeToolBar::highlightButton(ShapeType type)
{
    // Reset all buttons
    for (auto btn : shapeButtons) {
        (*btn)->setStyleSheet("");
    }
    
    // Highlight the selected button
    switch (type) {
        case ShapeType::Line:
            lineBtn->setStyleSheet("background-color: #AED6F1; font-weight: bold;");
            break;
        case ShapeType::Freehand:
            freehandBtn->setStyleSheet("background-color: #AED6F1; font-weight: bold;");
            break;
        case ShapeType::Rectangle:
            rectangleBtn->setStyleSheet("background-color: #AED6F1; font-weight: bold;");
            break;
        case ShapeType::Ellipse:
            ellipseBtn->setStyleSheet("background-color: #AED6F1; font-weight: bold;");
            break;
        case ShapeType::Pentagon:
            pentagonBtn->setStyleSheet("background-color: #AED6F1; font-weight: bold;");
            break;
        case ShapeType::Star:
            starBtn->setStyleSheet("background-color: #AED6F1; font-weight: bold;");
            break;
        case ShapeType::Hexagon:
            hexagonBtn->setStyleSheet("background-color: #AED6F1; font-weight: bold;");
            break;
        default:
            break;
    }
}
