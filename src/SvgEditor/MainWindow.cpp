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
    QMenu* fileMenu = menuBar()->addMenu(tr("File"));


    QAction* newAction = new QAction(tr("New"), this);
    newAction->setShortcut(QKeySequence::New);
    connect(newAction, &QAction::triggered, this, &MainWindow::newFile);
    fileMenu->addAction(newAction);
    
    QAction* openAction = new QAction(tr("Open"), this);
    openAction->setShortcut(QKeySequence::Open);
    connect(openAction, &QAction::triggered, this, &MainWindow::openFile);
    fileMenu->addAction(openAction);
    
    fileMenu->addSeparator();
    
    QAction* saveAction = new QAction(tr("Save"), this);
    saveAction->setShortcut(QKeySequence::Save);
    connect(saveAction, &QAction::triggered, this, &MainWindow::saveFile);
    fileMenu->addAction(saveAction);
    
    QAction* saveAsAction = new QAction(tr("Save As..."), this);
    saveAsAction->setShortcut(QKeySequence::SaveAs);
    connect(saveAsAction, &QAction::triggered, this, &MainWindow::saveFileAs);
    fileMenu->addAction(saveAsAction);
    
    fileMenu->addSeparator();
    
    QAction* exportAction = new QAction(tr("Export PNG..."), this);
    connect(exportAction, &QAction::triggered, this, &MainWindow::exportToPNG);
    fileMenu->addAction(exportAction);
    
    fileMenu->addSeparator();
    
    QAction* exitAction = new QAction(tr("Exit"), this); 
    exitAction->setShortcut(QKeySequence::Quit);
    connect(exitAction, &QAction::triggered, QApplication::instance(), &QApplication::quit);
    fileMenu->addAction(exitAction);
    
    QMenu* settingsMenu = menuBar()->addMenu(tr("Settings"));
    
    m_languageMenu = settingsMenu->addMenu(tr("Language"));
    
    // TODO: Add Edit menu and other menus
    
    qCDebug(mainWindowLog) << "Menu Setup success.";
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