#include "MainWindow.h"

Q_LOGGING_CATEGORY(mainWindowLog, "MainWindow")

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent),
    m_leftSideBar(new LeftSideBar(this)),
    m_rightAttrBar(new RightAttrBar(this)),
    m_canvasArea(new CanvasArea(this))
{
    qCDebug(mainWindowLog) << "MainWindow constructed.";
    
    QSplitter* splitter = new QSplitter(Qt::Horizontal, this);
    splitter->addWidget(m_leftSideBar);
    splitter->addWidget(m_canvasArea);
    splitter->addWidget(m_rightAttrBar);

    splitter->setStretchFactor(0, 2);  // Left sidebar
    splitter->setStretchFactor(1, 8);  // Canvas area
    splitter->setStretchFactor(2, 3);  // Right sidebar

    QList<int> sizes;
    sizes << 150 << 800 << 300;
    splitter->setSizes(sizes);
    setCentralWidget(splitter);
    
    setupMenus();
    setupToolBar();
    setupStatusBar();
    setupLanguageMenu();

    resize(1280, 800);
    setWindowTitle(tr("SVG Editor"));
    
    newFile();

    m_currentFilePath = "";
    updateTitle();

    qCDebug(mainWindowLog) << "MainWindow construct success.";
}

MainWindow::~MainWindow()
{
}

void MainWindow::newFile()
{
}

void MainWindow::openFile()
{
}

void MainWindow::saveFile()
{
}

void MainWindow::saveFileAs()
{
}

void MainWindow::exportToPNG()
{
}

void MainWindow::handleToolSelected(int toolId)
{
}

void MainWindow::setupMenus()
{
}

void MainWindow::setupToolBar()
{
}

void MainWindow::setupStatusBar()
{
}

void MainWindow::setupLanguageMenu()
{
}

void MainWindow::switchLanguage(const QString& locale)
{
}

QStringList MainWindow::getAvailableLanguages() const
{
    return QStringList();
}

void MainWindow::updateTitle()
{
}