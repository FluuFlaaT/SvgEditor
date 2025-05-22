#include "MainWindow.h"
#include <QTimer>

Q_LOGGING_CATEGORY(mainWindowLog, "MainWindow")

static inline QString picturesLocation()
{
    return QStandardPaths::standardLocations(QStandardPaths::PicturesLocation).value(0, QDir::currentPath());
}

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent),
    m_leftSideBar(new LeftSideBar(this)),
    m_rightAttrBar(new RightAttrBar(this)),
    m_canvasArea(new CanvasArea(this)),
    m_shapeToolBar(new ShapeToolBar(this)),
    m_svgEngine(new CoreSvgEngine),
    m_documentModified(false)
{
    qCDebug(mainWindowLog) << "MainWindow constructed.";

    // Create a horizontal splitter for the main layout
    QSplitter* splitter = new QSplitter(Qt::Horizontal, this);

    // Create a vertical splitter for the left sidebar and shape toolbar
    QSplitter* leftSplitter = new QSplitter(Qt::Vertical, this);
    leftSplitter->addWidget(m_leftSideBar);
    leftSplitter->addWidget(m_shapeToolBar);

    // Initially hide the shape toolbar
    m_shapeToolBar->hide();

    // Add the left splitter, canvas area, and right sidebar to the main splitter
    splitter->addWidget(leftSplitter);
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

    // Connect LeftSideBar buttons to handleToolSelected
    connect(m_leftSideBar->selectBtn, &QPushButton::clicked, this, [this]() { handleToolSelected(0); });
    connect(m_leftSideBar->drawBtn, &QPushButton::clicked, this, [this]() { handleToolSelected(1); });
    connect(m_leftSideBar->shapeBtn, &QPushButton::clicked, this, [this]() { handleToolSelected(2); });
    connect(m_leftSideBar->textBtn, &QPushButton::clicked, this, [this]() { handleToolSelected(3); });
    connect(m_leftSideBar->dragBtn, &QPushButton::clicked, this, [this]() { handleToolSelected(4); });

    resize(1280, 800);
    setWindowTitle(tr("SVG Editor"));

    m_currentFilePath = "";
    m_documentModified = false;
    updateTitle();

    // Initialize with an empty canvas ready for drawing
    m_svgEngine->createNewDocument(800, 600);
    m_canvasArea->openFileWithEngine(m_svgEngine);

    // Connect RightAttrBar signals to update canvas settings
    connect(m_rightAttrBar, &RightAttrBar::canvasSizeChanged, this, [this](int width, int height) {
        try {
            // Update canvas size in the engine
            SvgDocument* doc = m_svgEngine->getCurrentDocument();
            if (doc) {
                doc->setWidth(width);
                doc->setHeight(height);

                // Update the canvas view to reflect the new size
                if (m_canvasArea->openFileWithEngine(m_svgEngine)) {
                    m_documentModified = true;
                    updateTitle();
                    showStatusMessage(tr("Canvas size changed to %1x%2").arg(width).arg(height), 2000);
                } else {
                    showStatusMessage(tr("Failed to update canvas size"), 2000);
                }
            }
        } catch (const std::exception& e) {
            qCWarning(mainWindowLog) << "Exception during canvas size change:" << e.what();
            showStatusMessage(tr("Error changing canvas size"), 2000);
        }
    });

    connect(m_rightAttrBar, &RightAttrBar::canvasColorChanged, this, [this](const QColor& color) {
        // Update canvas background color in the engine
        Color bgColor;
        bgColor.r = color.red();
        bgColor.g = color.green();
        bgColor.b = color.blue();
        bgColor.alpha = color.alpha();
        m_svgEngine->getCurrentDocument()->setBackgroundColor(bgColor);

        // Update the canvas view to reflect the new background color
        m_canvasArea->openFileWithEngine(m_svgEngine);

        m_documentModified = true;
        updateTitle();
        showStatusMessage(tr("Canvas color changed"), 2000);
    });

    // Initialize the RightAttrBar with current document properties
    updateRightAttrBarFromDocument();

    // Connect zoom signals
    connect(m_canvasArea, &CanvasArea::zoomChanged, this, &MainWindow::updateZoomStatus);
    connect(m_canvasArea, &CanvasArea::zoomChanged, m_rightAttrBar, &RightAttrBar::updateZoomLevel);

    // Connect left sidebar zoom button (only zoomIn remains)
    connect(m_leftSideBar, &LeftSideBar::zoomInRequested, m_canvasArea, &CanvasArea::zoomIn);

    // Connect drag tool signal
    connect(m_leftSideBar, &LeftSideBar::dragToolRequested, this, [this]() { handleToolSelected(4); });

    // Connect shape toolbar signals
    connect(m_shapeToolBar, &ShapeToolBar::shapeToolSelected, this, &MainWindow::handleShapeToolSelected);

    // Set the current engine for the canvas area
    m_canvasArea->setCurrentEngine(m_svgEngine);

    showStatusMessage(tr("Ready"), 2000);

    qCDebug(mainWindowLog) << "MainWindow construct success.";
}

MainWindow::~MainWindow()
{
    // Clean up resources
    delete m_svgEngine;
    qCDebug(mainWindowLog) << "MainWindow destroyed.";
}

void MainWindow::newFile()
{
    // Check if we need to save current document
    if (m_documentModified && !maybeSave()) {
        return; // User canceled the operation
    }

    QFileDialog fileDialog(this);
    fileDialog.setAcceptMode(QFileDialog::AcceptMode::AcceptSave);
    fileDialog.setMimeTypeFilters(QStringList() << "image/svg+xml" << "image/svg+xml-compressed");
    fileDialog.setDefaultSuffix("svg");
    fileDialog.setWindowTitle(tr("New SVG File"));

    if(m_currentFilePath.isEmpty()) {
        fileDialog.setDirectory(picturesLocation());
    }

    fileDialog.setOption(QFileDialog::DontUseNativeDialog, false);

    if (fileDialog.exec() == QDialog::Accepted) {
        if (!fileDialog.selectedFiles().isEmpty()) {
            QString fileName = fileDialog.selectedFiles().constFirst();

            // Create a new SVG document with default size
            m_svgEngine->createNewDocument(800, 600);

            // Save the new document
            if (m_svgEngine->saveSvgFile(fileName.toStdString())) {
                m_currentFilePath = fileName;
                m_documentModified = false;

                // Load the new file
                loadFileWithEngine(fileName);

                // Ensure the RightAttrBar is in sync with the new document
                updateRightAttrBarFromDocument();

                qCDebug(mainWindowLog) << "New file created:" << fileName;
                showStatusMessage(tr("New file created"), 2000);
            } else {
                QMessageBox::critical(this, tr("New SVG File"),
                                     tr("Could not create file '%1'.").arg(QDir::toNativeSeparators(fileName)));
                qCWarning(mainWindowLog) << "Failed to create new file:" << fileName;
            }
        }
    }
}

void MainWindow::openFile()
{
    // Check if we need to save current document
    if (m_documentModified && !maybeSave()) {
        return; // User canceled the operation
    }

    QFileDialog fileDialog(this);
    fileDialog.setAcceptMode(QFileDialog::AcceptMode::AcceptOpen);
    fileDialog.setMimeTypeFilters(QStringList() << "image/svg+xml" << "image/svg+xml-compressed");
    fileDialog.setWindowTitle(tr("Open SVG File"));

    if(m_currentFilePath.isEmpty()) {
        fileDialog.setDirectory(picturesLocation());
    } else {
        fileDialog.setDirectory(QFileInfo(m_currentFilePath).path());
    }

    fileDialog.setOption(QFileDialog::DontUseNativeDialog, false);

    if (fileDialog.exec() == QDialog::Accepted) {
        if (!fileDialog.selectedFiles().isEmpty()) {
            QString fileName = fileDialog.selectedFiles().constFirst();

            qCDebug(mainWindowLog) << "Opening file:" << fileName;

            if (m_svgEngine->loadSvgFile(fileName.toStdString())) {
                if (loadFileWithEngine(fileName)) {
                    showStatusMessage(tr("File opened"), 2000);
                }
            } else {
                QMessageBox::critical(this, tr("Open SVG File"),
                                     tr("Could not open file '%1'.").arg(QDir::toNativeSeparators(fileName)));
                qCWarning(mainWindowLog) << "Failed to open file:" << fileName;
            }
        }
    }
}

bool MainWindow::loadFile(const QString& fileName) {
    if (!QFileInfo::exists(fileName) || !m_canvasArea->openFile(fileName)) {
        QMessageBox::critical(this, tr("Open SVG File"),
                              tr("Could not open file '%1'.").arg(QDir::toNativeSeparators(fileName)));
        return false;
    }

    m_currentFilePath = fileName;
    m_documentModified = false;
    updateTitle();
    return true;
}

bool MainWindow::loadFileWithEngine(const QString& fileName) {
    if (!QFileInfo::exists(fileName) || !m_canvasArea->openFileWithEngine(m_svgEngine)) {
        QMessageBox::critical(this, tr("Open SVG File"),
                              tr("Could not open file '%1'.").arg(QDir::toNativeSeparators(fileName)));
        return false;
    }

    m_currentFilePath = fileName;
    m_documentModified = false;
    updateTitle();

    // Update the RightAttrBar with the document properties
    updateRightAttrBarFromDocument();

    m_canvasArea->update();

    qCDebug(mainWindowLog) << "File loaded successfully:" << fileName;
    return true;
}

void MainWindow::saveFile()
{
    if (m_currentFilePath.isEmpty()) {
        saveFileAs();
        return;
    }

    qCDebug(mainWindowLog) << "Saving file to:" << m_currentFilePath;

    if (m_svgEngine->saveSvgFile(m_currentFilePath.toStdString())) {
        m_documentModified = false;
        updateTitle();

        // Ensure the RightAttrBar is in sync with the saved document
        updateRightAttrBarFromDocument();

        showStatusMessage(tr("File saved"), 2000);
        qCDebug(mainWindowLog) << "File saved successfully";
    } else {
        QMessageBox::critical(this, tr("Save SVG File"),
                             tr("Could not save file '%1'.").arg(QDir::toNativeSeparators(m_currentFilePath)));
        qCWarning(mainWindowLog) << "Failed to save file:" << m_currentFilePath;
    }
}

void MainWindow::saveFileAs()
{
    QFileDialog fileDialog(this);
    fileDialog.setAcceptMode(QFileDialog::AcceptMode::AcceptSave);
    fileDialog.setMimeTypeFilters(QStringList() << "image/svg+xml" << "image/svg+xml-compressed");
    fileDialog.setDefaultSuffix("svg");
    fileDialog.setWindowTitle(tr("Save SVG File As"));

    if (m_currentFilePath.isEmpty()) {
        fileDialog.setDirectory(picturesLocation());
    } else {
        fileDialog.setDirectory(QFileInfo(m_currentFilePath).path());
        fileDialog.selectFile(QFileInfo(m_currentFilePath).fileName());
    }

    fileDialog.setOption(QFileDialog::DontUseNativeDialog, false);

    if (fileDialog.exec() == QDialog::Accepted) {
        QString fileName = fileDialog.selectedFiles().constFirst();
        m_currentFilePath = fileName;
        saveFile();
    }
}

void MainWindow::exportToPNG()
{
    QFileDialog fileDialog(this);
    fileDialog.setAcceptMode(QFileDialog::AcceptMode::AcceptSave);
    fileDialog.setMimeTypeFilters(QStringList() << "image/png");
    fileDialog.setDefaultSuffix("png");
    fileDialog.setWindowTitle(tr("Export to PNG"));

    if (m_currentFilePath.isEmpty()) {
        fileDialog.setDirectory(picturesLocation());
    } else {
        QString suggestedName = QFileInfo(m_currentFilePath).completeBaseName() + ".png";
        fileDialog.setDirectory(QFileInfo(m_currentFilePath).path());
        fileDialog.selectFile(suggestedName);
    }

    fileDialog.setOption(QFileDialog::DontUseNativeDialog, false);

    if (fileDialog.exec() == QDialog::Accepted) {
        QString fileName = fileDialog.selectedFiles().constFirst();

        // Get the scene from the canvas
        QGraphicsScene* scene = m_canvasArea->scene();
        if (!scene) {
            QMessageBox::critical(this, tr("Export to PNG"), tr("No scene to export"));
            return;
        }

        // Create a pixmap to render the scene
        QRectF sceneRect = scene->sceneRect();
        QPixmap pixmap(sceneRect.size().toSize());
        pixmap.fill(Qt::transparent);

        // Render the scene to the pixmap
        QPainter painter(&pixmap);
        painter.setRenderHint(QPainter::Antialiasing);
        scene->render(&painter);
        painter.end();

        // Save the pixmap as PNG
        if (pixmap.save(fileName, "PNG")) {
            showStatusMessage(tr("Exported to PNG"), 2000);
            qCDebug(mainWindowLog) << "Successfully exported to PNG:" << fileName;
        } else {
            QMessageBox::critical(this, tr("Export to PNG"),
                                 tr("Could not export to '%1'.").arg(QDir::toNativeSeparators(fileName)));
            qCWarning(mainWindowLog) << "Failed to export to PNG:" << fileName;
        }
    }
}

void MainWindow::handleToolSelected(int toolId)
{
    qCDebug(mainWindowLog) << "Tool selected:" << toolId;

    // Highlight the selected button
    for (int i = 0; i < m_leftSideBar->btnGroup.size(); ++i) {
        QPushButton* btn = *(m_leftSideBar->btnGroup[i]);
        if (i == toolId) {
            btn->setStyleSheet("background-color: #AED6F1; font-weight: bold;");
        } else {
            btn->setStyleSheet("");
        }
    }

    // Update status bar
    QString toolName;
    int attrWidgetType = RightAttrBar::CommonAttributes; // Default to common attributes

    // Reset drag mode by default
    m_canvasArea->setDragMode(false);

    // Hide shape toolbar by default
    m_shapeToolBar->hide();

    switch (toolId) {
        case 0:
            toolName = tr("Select Tool");
            attrWidgetType = RightAttrBar::CommonAttributes;
            m_canvasArea->setSelectMode();
            break;
        case 1:
            toolName = tr("Draw Tool");
            attrWidgetType = RightAttrBar::CommonAttributes;
            // Set freehand drawing mode
            m_canvasArea->setShapeCreationMode(ShapeType::Freehand);
            break;
        case 2:
            toolName = tr("Shape Tool");
            attrWidgetType = RightAttrBar::CircleAttributes;
            // Show the shape toolbar
            m_shapeToolBar->show();
            m_shapeToolBar->setSelectedShapeType(ShapeType::Rectangle); // Default to rectangle
            m_canvasArea->setShapeCreationMode(ShapeType::Rectangle);
            break;
        case 3:
            toolName = tr("Text Tool");
            attrWidgetType = RightAttrBar::CommonAttributes;
            break;
        case 4:
            toolName = tr("Drag Tool");
            attrWidgetType = RightAttrBar::CommonAttributes;
            // Enable drag mode for the canvas
            m_canvasArea->setDragMode(true);
            break;
        default:
            toolName = tr("Unknown Tool");
            attrWidgetType = RightAttrBar::CommonAttributes;
    }

    // Switch the right attribute bar to show the appropriate widget
    m_rightAttrBar->setCurrentWidget(attrWidgetType);

    showStatusMessage(tr("Current tool: %1").arg(toolName), 2000);
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

    // Add View menu
    QMenu* viewMenu = menuBar()->addMenu(tr("View"));

    // Zoom actions
    QAction* zoomInAction = new QAction(tr("Zoom In"), this);
    zoomInAction->setShortcut(QKeySequence::ZoomIn); // Usually Ctrl++
    zoomInAction->setIcon(QIcon::fromTheme("zoom-in", QIcon(":/icons/zoom-in.png")));
    connect(zoomInAction, &QAction::triggered, m_canvasArea, &CanvasArea::zoomIn);
    viewMenu->addAction(zoomInAction);

    QAction* zoomOutAction = new QAction(tr("Zoom Out"), this);
    zoomOutAction->setShortcut(QKeySequence::ZoomOut); // Usually Ctrl+-
    zoomOutAction->setIcon(QIcon::fromTheme("zoom-out", QIcon(":/icons/zoom-out.png")));
    connect(zoomOutAction, &QAction::triggered, m_canvasArea, &CanvasArea::zoomOut);
    viewMenu->addAction(zoomOutAction);

    QAction* resetZoomAction = new QAction(tr("Reset Zoom"), this);
    resetZoomAction->setShortcut(QKeySequence(tr("Ctrl+0")));
    resetZoomAction->setIcon(QIcon::fromTheme("zoom-original", QIcon(":/icons/zoom-reset.png")));
    connect(resetZoomAction, &QAction::triggered, m_canvasArea, &CanvasArea::resetZoom);
    viewMenu->addAction(resetZoomAction);

    QAction* fitToWindowAction = new QAction(tr("Fit to Window"), this);
    fitToWindowAction->setShortcut(QKeySequence(tr("Ctrl+F")));
    fitToWindowAction->setIcon(QIcon::fromTheme("zoom-fit-best", QIcon(":/icons/zoom-fit.png")));
    connect(fitToWindowAction, &QAction::triggered, m_canvasArea, &CanvasArea::fitToView);
    viewMenu->addAction(fitToWindowAction);

    QMenu* settingsMenu = menuBar()->addMenu(tr("Settings"));

    m_languageMenu = settingsMenu->addMenu(tr("Language"));

    // TODO: Add Edit menu and other menus

    qCDebug(mainWindowLog) << "Menu Setup success.";
}

void MainWindow::setupToolBar()
{
    qCDebug(mainWindowLog) << "Setting up toolbar";

    m_mainToolBar = addToolBar(tr("Main Toolbar"));
    m_mainToolBar->setMovable(false);
    m_mainToolBar->setIconSize(QSize(24, 24));

    // Add file operations to toolbar
    QAction* newAction = new QAction(tr("New"), this);
    newAction->setIcon(QIcon::fromTheme("document-new", QIcon(":/icons/new.png")));
    connect(newAction, &QAction::triggered, this, &MainWindow::newFile);
    m_mainToolBar->addAction(newAction);

    QAction* openAction = new QAction(tr("Open"), this);
    openAction->setIcon(QIcon::fromTheme("document-open", QIcon(":/icons/open.png")));
    connect(openAction, &QAction::triggered, this, &MainWindow::openFile);
    m_mainToolBar->addAction(openAction);

    QAction* saveAction = new QAction(tr("Save"), this);
    saveAction->setIcon(QIcon::fromTheme("document-save", QIcon(":/icons/save.png")));
    connect(saveAction, &QAction::triggered, this, &MainWindow::saveFile);
    m_mainToolBar->addAction(saveAction);

    m_mainToolBar->addSeparator();

    // Add zoom controls
    QAction* zoomInAction = new QAction(tr("Zoom In"), this);
    zoomInAction->setIcon(QIcon::fromTheme("zoom-in", QIcon(":/icons/zoom-in.png")));
    connect(zoomInAction, &QAction::triggered, m_canvasArea, &CanvasArea::zoomIn);
    m_mainToolBar->addAction(zoomInAction);

    QAction* zoomOutAction = new QAction(tr("Zoom Out"), this);
    zoomOutAction->setIcon(QIcon::fromTheme("zoom-out", QIcon(":/icons/zoom-out.png")));
    connect(zoomOutAction, &QAction::triggered, m_canvasArea, &CanvasArea::zoomOut);
    m_mainToolBar->addAction(zoomOutAction);

    QAction* zoomResetAction = new QAction(tr("Reset Zoom"), this);
    zoomResetAction->setIcon(QIcon::fromTheme("zoom-original", QIcon(":/icons/zoom-reset.png")));
    connect(zoomResetAction, &QAction::triggered, m_canvasArea, &CanvasArea::resetZoom);
    m_mainToolBar->addAction(zoomResetAction);

    QAction* zoomFitAction = new QAction(tr("Fit to View"), this);
    zoomFitAction->setIcon(QIcon::fromTheme("zoom-fit-best", QIcon(":/icons/zoom-fit.png")));
    connect(zoomFitAction, &QAction::triggered, m_canvasArea, &CanvasArea::fitToView);
    m_mainToolBar->addAction(zoomFitAction);

    qCDebug(mainWindowLog) << "Toolbar setup complete";
}

void MainWindow::setupStatusBar()
{
    qCDebug(mainWindowLog) << "Setting up status bar";

    m_statusLabel = new QLabel(tr("Ready"));
    statusBar()->addWidget(m_statusLabel, 1);

    m_zoomLabel = new QLabel(tr("Zoom: 100%"));
    statusBar()->addPermanentWidget(m_zoomLabel);

    // Initialize status message handling
    m_statusTimer = new QTimer(this);
    m_statusTimer->setSingleShot(true);
    connect(m_statusTimer, &QTimer::timeout, this, &MainWindow::clearStatusMessage);
    m_statusMessageActive = false;

    qCDebug(mainWindowLog) << "Status bar setup complete";
}

void MainWindow::showStatusMessage(const QString& message, int timeout)
{
    // If a message is already active, stop its timer
    if (m_statusMessageActive) {
        m_statusTimer->stop();
    }

    // Show the new message
    m_statusLabel->setText(message);
    m_statusMessageActive = true;

    // Start the timer for auto-clearing
    if (timeout > 0) {
        m_statusTimer->start(timeout);
    }

    qCDebug(mainWindowLog) << "Status message:" << message;
}

void MainWindow::clearStatusMessage()
{
    if (m_statusMessageActive) {
        m_statusLabel->setText(tr("Ready"));
        m_statusMessageActive = false;
        qCDebug(mainWindowLog) << "Status message cleared";
    }
}

void MainWindow::updateZoomStatus(qreal zoomFactor)
{
    if (m_zoomLabel) {
        m_zoomLabel->setText(tr("Zoom: %1%").arg(int(zoomFactor * 100)));
    }
}

void MainWindow::handleShapeToolSelected(ShapeType type)
{
    qCDebug(mainWindowLog) << "Shape tool selected:" << static_cast<int>(type);

    // Set the shape creation mode in the canvas area
    m_canvasArea->setShapeCreationMode(type);

    // Update the status bar
    QString shapeName;
    switch (type) {
        case ShapeType::Line:
            shapeName = tr("Line");
            break;
        case ShapeType::Freehand:
            shapeName = tr("Freehand");
            break;
        case ShapeType::Rectangle:
            shapeName = tr("Rectangle");
            break;
        case ShapeType::Ellipse:
            shapeName = tr("Ellipse");
            break;
        case ShapeType::Pentagon:
            shapeName = tr("Pentagon");
            break;
        case ShapeType::Star:
            shapeName = tr("Star");
            break;
        case ShapeType::Hexagon:
            shapeName = tr("Hexagon");
            break;
        default:
            shapeName = tr("Unknown");
            break;
    }

    showStatusMessage(tr("Shape tool: %1").arg(shapeName), 2000);
}

void MainWindow::setupLanguageMenu()
{
    qCDebug(mainWindowLog) << "Setting up language menu";

    m_languages = getAvailableLanguages();

    if (m_languages.isEmpty()) {
        qCWarning(mainWindowLog) << "No language files found";
        return;
    }

    for (const QString& locale : m_languages) {
        QString language = QLocale(locale).nativeLanguageName();
        QAction* action = m_languageMenu->addAction(language);
        action->setCheckable(true);
        action->setData(locale);

        connect(action, &QAction::triggered, this, [this, locale]() {
            switchLanguage(locale);
        });

        if (locale == QLocale::system().name().left(2)) {
            action->setChecked(true);
            m_currentLanguage = locale;
        }
    }

    qCDebug(mainWindowLog) << "Language menu setup complete with" << m_languages.size() << "languages";
}

void MainWindow::switchLanguage(const QString& locale)
{
    qCDebug(mainWindowLog) << "Switching language to:" << locale;

    if (m_currentLanguage == locale) {
        return;
    }

    // Load and install the new translator
    if (m_translator.load("svgeditor_" + locale, m_translationsPath)) {
        qApp->installTranslator(&m_translator);
        m_currentLanguage = locale;

        // Update checked state in menu
        for (QAction* action : m_languageMenu->actions()) {
            action->setChecked(action->data().toString() == locale);
        }

        qCDebug(mainWindowLog) << "Language switched successfully";
    } else {
        qCWarning(mainWindowLog) << "Failed to load translation for:" << locale;
    }
}

QStringList MainWindow::getAvailableLanguages() const
{
    QStringList result;

    // Add English as default
    result << "en";

    // TODO: Scan translations directory for available language files
    // This would typically look for files like svgeditor_fr.qm, svgeditor_de.qm, etc.

    return result;
}

void MainWindow::updateTitle()
{
    QString title = tr("SVG Editor");

    if (!m_currentFilePath.isEmpty()) {
        QString fileName = QFileInfo(m_currentFilePath).fileName();
        title = fileName + " - " + title;

        if (m_documentModified) {
            title = "* " + title;
        }
    }

    setWindowTitle(title);
}

bool MainWindow::maybeSave()
{
    if (!m_documentModified) {
        return true;
    }

    QMessageBox::StandardButton ret = QMessageBox::warning(
        this,
        tr("SVG Editor"),
        tr("The document has been modified.\nDo you want to save your changes?"),
        QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel
    );

    if (ret == QMessageBox::Save) {
        saveFile();
        return !m_documentModified; // Return true if save was successful
    } else if (ret == QMessageBox::Cancel) {
        return false;
    }

    return true; // User chose Discard
}

void MainWindow::updateRightAttrBarFromDocument()
{
    if (!m_svgEngine || !m_svgEngine->getCurrentDocument()) {
        qCWarning(mainWindowLog) << "No document to update RightAttrBar from";
        return;
    }

    SvgDocument* doc = m_svgEngine->getCurrentDocument();

    // Update canvas size
    double width = doc->getWidth();
    double height = doc->getHeight();
    m_rightAttrBar->updateCanvasSize(static_cast<int>(width), static_cast<int>(height));

    // Update canvas color
    Color bgColor = doc->getBackgroundColor();
    QColor qBgColor(bgColor.r, bgColor.g, bgColor.b, bgColor.alpha);
    m_rightAttrBar->updateCanvasColor(qBgColor);

    qCDebug(mainWindowLog) << "Updated RightAttrBar from document - Size:" << width << "x" << height
                          << "Color:" << QString::fromStdString(bgColor.toString());
}