#include "MainWindow.h"
#include "SvgShapes.h"     
#include <QVBoxLayout>
#include <QPushButton>
#include "LoggingService.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent) // 如果你使用 .ui 文件，取消这行的注释并确保 ui_MainWindow.h存在
{
    LoggingService::getInstance().debug("MainWindow constructor called.");
    // ui->setupUi(this); // 如果你使用 .ui 文件

    QPushButton *createCircleButton = new QPushButton("Create Circle", this);
    
    m_svgWidget = new SvgDisplayWidget(this);

    QVBoxLayout *layout = new QVBoxLayout;
    layout->addWidget(createCircleButton);
    layout->addWidget(m_svgWidget);

    QWidget *centralWidget = new QWidget(this);
    centralWidget->setLayout(layout);
    setCentralWidget(centralWidget);

    resize(800, 600);

    connect(createCircleButton, &QPushButton::clicked, this, &MainWindow::createCircle);

    m_svgEngine.createNewDocument(400, 300, {240, 240, 240, 255});
    LoggingService::getInstance().info("Initial SVG document created in MainWindow.");
    
    QString initialSvgContent = QString::fromStdString(m_svgEngine.getCurrentDocument()->generateSvgContent());
    m_svgWidget->loadSvgContent(initialSvgContent);
    LoggingService::getInstance().debug("MainWindow setup complete.");
}

MainWindow::~MainWindow()
{
    LoggingService::getInstance().debug("MainWindow destructor called.");
}

void MainWindow::createCircle()
{
    LoggingService::getInstance().debug("createCircle slot called.");
    SvgDocument* doc = m_svgEngine.getCurrentDocument();
    if (doc) {
        LoggingService::getInstance().debug("Creating a new SvgCircle element.");
        
        SvgCircle* circle = new SvgCircle({200, 150}, 50);
        circle->setFillColor({255, 0, 0, 255});
        circle->setStrokeColor({0, 0, 0, 255});
        circle->setStrokeWidth(2.0);
        doc->addElement(circle);

        QString updatedSvgContent = QString::fromStdString(doc->generateSvgContent());
        m_svgWidget->loadSvgContent(updatedSvgContent);
        LoggingService::getInstance().debug("SVG content updated and reloaded in display widget.");
    } else {
        LoggingService::getInstance().warn("Cannot create circle: SvgDocument is null.");
    }
}
