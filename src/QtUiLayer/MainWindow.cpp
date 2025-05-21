#include "MainWindow.h"
#include "SvgShapes.h"
#include <QApplication>
#include <QHBoxLayout>
#include <QSplitter>
#include <QIcon>
#include <QFileInfo>
#include <QDir>
#include <QEvent>
#include <QActionGroup>
#include <QMessageBox>
#include <QTranslator>
#include <QLocale>
#include "LoggingService.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), m_currentFilePath(""), m_currentLanguage("en_US")
{
    LoggingService::getInstance().debug("MainWindow constructor called.");
    
    /*
    m_translationsPath = QApplication::applicationDirPath() + "/translations/";
    m_translationsDir = new QDir(m_translationsPath);
    if (!m_translationsDir->exists()) {
        m_translationsPath = QDir::currentPath() + "/translations/";
    }
    
    QString locale = QLocale::system().name();


    if (!m_translator.load(locale, translationsPath)) {
        m_translator.load("en_US", translationsPath);
        m_currentLanguage = "en_US";
    } else {
        m_currentLanguage = locale;
    }
    qApp->installTranslator(&m_translator);
    */

    // Create UI components
    m_drawingLayer = new DrawingLayer(this);
    m_leftSideBar = new LeftSideBar(this);
    m_rightAttrBar = new RightAttrBar(this);
    
    // Create horizontal splitter
    QSplitter* splitter = new QSplitter(Qt::Horizontal, this);
    splitter->addWidget(m_leftSideBar);
    splitter->addWidget(m_drawingLayer);
    splitter->addWidget(m_rightAttrBar);
    
    // Set splitter proportions
    splitter->setStretchFactor(0, 2);  // Left sidebar
    splitter->setStretchFactor(1, 8); // Drawing area
    splitter->setStretchFactor(2, 3);  // Right sidebar
    
    // 设置分割器初始大小
    QList<int> sizes;
    sizes << 150 << 800 << 300;
    splitter->setSizes(sizes);
      // Set as central widget
    setCentralWidget(splitter);
    
    // Setup menus and toolbars
    setupMenus();
    setupToolBar();
    setupStatusBar();
    
    // Window size
    resize(1280, 800);
    setWindowTitle(tr("SVG Editor"));
    
    // Connect signals and slots
    connect(m_leftSideBar, &LeftSideBar::toolSelected, this, &MainWindow::handleToolSelected);
    
    // Create initial empty document
    newFile();
    
    LoggingService::getInstance().debug("MainWindow setup complete.");
}

MainWindow::~MainWindow()
{
    LoggingService::getInstance().debug("MainWindow destructor called.");
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
    setupLanguageMenu();
    
    // TODO: Add Edit menu and other menus
    
    LoggingService::getInstance().debug("Menus setup complete.");
}

void MainWindow::setupToolBar()
{
    QToolBar* fileToolBar = addToolBar(tr("File Operations"));
    fileToolBar->setMovable(false);
    
    // Add common file operations
    fileToolBar->addAction(tr("New"), this, &MainWindow::newFile);
    fileToolBar->addAction(tr("Open"), this, &MainWindow::openFile);
    fileToolBar->addAction(tr("Save"), this, &MainWindow::saveFile);
    
    // TODO: Add more toolbar items
    
    LoggingService::getInstance().debug("Toolbar setup complete.");
}

void MainWindow::setupStatusBar()
{
    statusBar()->showMessage(tr("Ready"));
    LoggingService::getInstance().debug("Status bar setup complete.");
}

// File operations
void MainWindow::newFile()
{
    // Create new SVG document
    m_svgEngine.createNewDocument(800, 600, {255, 255, 255, 255});
    m_currentFilePath = "";
    LoggingService::getInstance().info("New SVG document created.");
    
    // Update display
    QString svgContent = QString::fromStdString(m_svgEngine.getCurrentDocument()->generateSvgContent());
    m_drawingLayer->setSvgContent(svgContent);
    
    // Update window title
    updateTitle();
}

void MainWindow::openFile()
{
    QString filePath = QFileDialog::getOpenFileName(this, tr("Open SVG File"), QDir::homePath(), tr("SVG Files (*.svg)"));
    if (!filePath.isEmpty()) {
        if (m_svgEngine.loadSvgFile(filePath.toStdString())) {
            m_currentFilePath = filePath;
            LoggingService::getInstance().info("Loaded SVG file: " + filePath.toStdString());
            
            QString svgContent = QString::fromStdString(m_svgEngine.getCurrentDocument()->generateSvgContent());
            m_drawingLayer->setSvgContent(svgContent);
            
            // Update window title
            updateTitle();
        } else {
            QMessageBox::critical(this, tr("Error"), tr("Cannot open file: ") + filePath);
            LoggingService::getInstance().error("Failed to load SVG file: " + filePath.toStdString());
        }
    }
}

void MainWindow::saveFile()
{
    if (m_currentFilePath.isEmpty()) {
        saveFileAs();
    } else {
        if (m_svgEngine.saveSvgFile(m_currentFilePath.toStdString())) {
            statusBar()->showMessage(tr("File saved: ") + m_currentFilePath, 3000);
            LoggingService::getInstance().info("Saved SVG file: " + m_currentFilePath.toStdString());
        } else {
            QMessageBox::critical(this, tr("Error"), tr("Cannot save file: ") + m_currentFilePath);
            LoggingService::getInstance().error("Failed to save SVG file: " + m_currentFilePath.toStdString());
        }
    }
}

void MainWindow::saveFileAs()
{
    QString filePath = QFileDialog::getSaveFileName(this, tr("Save SVG File"), QDir::homePath(), tr("SVG Files (*.svg)"));
    if (!filePath.isEmpty()) {
        // Ensure file has correct extension
        if (!filePath.toLower().endsWith(".svg")) {
            filePath += ".svg";
        }
        
        if (m_svgEngine.saveSvgFile(filePath.toStdString())) {
            m_currentFilePath = filePath;
            updateTitle();
            statusBar()->showMessage(tr("File saved: ") + m_currentFilePath, 3000);
            LoggingService::getInstance().info("Saved SVG file as: " + m_currentFilePath.toStdString());
        } else {
            QMessageBox::critical(this, tr("Error"), tr("Cannot save file: ") + filePath);
            LoggingService::getInstance().error("Failed to save SVG file: " + filePath.toStdString());
        }
    }
}

void MainWindow::exportToPNG()
{
    // Will be implemented after completing the DrawingLayer
    QMessageBox::information(this, tr("Information"), tr("This feature will be available after drawing layer implementation is completed."));
    LoggingService::getInstance().info("PNG export feature not yet implemented.");
}

void MainWindow::updateTitle()
{
    QString title = tr("SVG Editor");
    if (!m_currentFilePath.isEmpty()) {
        QFileInfo fileInfo(m_currentFilePath);
        title += " - " + fileInfo.fileName();
    } else {
        title += " - " + tr("Untitled");
    }
    setWindowTitle(title);
}

void MainWindow::setupLanguageMenu()
{
    // 获取可用的语言列表
    QStringList languages = getAvailableLanguages();
    
    // 创建语言选择的动作组（仅允许选择一种语言）
    QActionGroup* langGroup = new QActionGroup(this);
    langGroup->setExclusive(true);
    
    // 为每种语言创建一个选择项
    QMap<QString, QString> displayNames;
    displayNames["en_US"] = tr("English");
    displayNames["zh_CN"] = tr("简体中文");
    
    for (const QString& langCode : languages) {
        QString displayName = displayNames.contains(langCode) ? displayNames[langCode] : langCode;
        QAction* langAction = new QAction(displayName, this);
        langAction->setCheckable(true);
        langAction->setData(langCode);
        
        // 设置当前语言为选中状态
        if (langCode == m_currentLanguage) {
            langAction->setChecked(true);
        }
        
        // 连接信号槽，点击时切换语言
        connect(langAction, &QAction::triggered, this, [this, langCode]() {
            switchLanguage(langCode);
            LoggingService::getInstance().info("Language menu action triggered: " + langCode.toStdString());
        });
        
        langGroup->addAction(langAction);
        m_languageMenu->addAction(langAction);
    }
    
    LoggingService::getInstance().debug("Language menu setup complete.");
}

void MainWindow::changeEvent(QEvent* event)
{
    if (event->type() == QEvent::LanguageChange) {
        LoggingService::getInstance().debug("Language change event received in MainWindow");
        updateTitle();
        // 更新所有使用tr()的UI元素
    }
    QMainWindow::changeEvent(event);
}

void MainWindow::switchLanguage(const QString& locale)
{
    if (m_currentLanguage == locale) {
        LoggingService::getInstance().debug("Language is already set to " + locale.toStdString());
        return;
    }
    
    // 移除旧的翻译器
    qApp->removeTranslator(&m_translator);
    
    // 加载新的翻译文件
    QString translationsPath = QApplication::applicationDirPath() + "/translations/";
    QDir translationDir(translationsPath);
    if (!translationDir.exists()) {
        // 如果不存在，尝试查找项目目录中的translations
        translationsPath = QDir::currentPath() + "/translations/";
    }
    
    bool loaded = m_translator.load(locale, translationsPath);
    if (loaded) {
        // 安装新的翻译器
        qApp->installTranslator(&m_translator);
        m_currentLanguage = locale;
        LoggingService::getInstance().info("Switched language to: " + locale.toStdString());
    } else {
        LoggingService::getInstance().error("Failed to load translation for locale: " + locale.toStdString());
    }
}

QStringList MainWindow::getAvailableLanguages() const
{
    QStringList languages;
    
    QString translationsPath = QApplication::applicationDirPath() + "/translations/";
    QDir dir(translationsPath);
    if (!dir.exists()) {
        translationsPath = QDir::currentPath() + "/translations/";
        dir.setPath(translationsPath);
    }
    
    QStringList filters;
    filters << "*.qm";
    QStringList files = dir.entryList(filters, QDir::Files);
    
    QMap<QString, QString> supportedLanguages;
    supportedLanguages["en_US"] = tr("English");
    supportedLanguages["zh_CN"] = tr("简体中文");
    
    for (const QString& file : files) {
        QString langCode = QFileInfo(file).baseName();
        if (supportedLanguages.contains(langCode)) {
            languages << langCode;
        }
    }
    
    if (languages.isEmpty()) {
        languages << "en_US";
    }
    
    return languages;
}

void MainWindow::handleToolSelected(int toolId)
{
    // Update right attribute bar based on selected tool
    m_rightAttrBar->showAttributesForTool(toolId);
    
    // TODO: Set current drawing tool
    LoggingService::getInstance().debug("Selected tool with ID: " + std::to_string(toolId));
    
    // Temporarily keep this feature for circle example creation
    if (toolId == 4) { // TOOL_CIRCLE 
        createCircle();
    }
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
        m_drawingLayer->setSvgContent(updatedSvgContent);
        LoggingService::getInstance().debug("SVG content updated and reloaded in drawing layer.");
    } else {
        LoggingService::getInstance().warn("Cannot create circle: SvgDocument is null.");
    }
}
