#include "mainwindow.h"
#include <QApplication>
#include <QMessageBox>
#include <QSplitter>
#include <QFileDialog>
#include <QStandardPaths>
#include <QPixmap>
#include <QLoggingCategory>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QStatusBar>
#include <QMenuBar>
#include <QToolBar>
#include <QLabel>
#include <QPushButton>
#include <QGraphicsSimpleTextItem>
#include "../ConfigDialog/configdialog.h"
#include "../Commands/ModifyTextCommand.h"
#include "../ConfigManager/configmanager.h"
#include <QTimer>

// Include the undo/redo implementation
#include "MainWindow_UndoRedo.cpp"

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
    m_documentModified(false),
    m_undoAction(nullptr),
    m_redoAction(nullptr)
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
    connect(m_leftSideBar->zoomBtn, &QPushButton::clicked, this, [this]() { handleToolSelected(5); });

    resize(1280, 800);
    setWindowTitle(tr("SVG Editor"));
    setWindowIcon(QIcon(":/icon/images/icon.svg"));

    m_currentFilePath = "";
    m_documentModified = false;
    updateTitle();

    // Initialize with an empty canvas ready for drawing using default settings
    ConfigManager* configManager = ConfigManager::instance();
    QSize defaultCanvasSize = configManager->getDefaultCanvasSize();
    m_svgEngine->createNewDocument(defaultCanvasSize.width(), defaultCanvasSize.height());
    
    // Set default background color
    QColor defaultBgColor = configManager->getDefaultCanvasBackgroundColor();
    Color bgColor;
    bgColor.r = defaultBgColor.red();
    bgColor.g = defaultBgColor.green();
    bgColor.b = defaultBgColor.blue();
    bgColor.alpha = defaultBgColor.alpha();
    m_svgEngine->getCurrentDocument()->setBackgroundColor(bgColor);
    
    m_canvasArea->openFileWithEngine(m_svgEngine);

    // Connect RightAttrBar signals to update canvas settings
    connect(m_rightAttrBar, &RightAttrBar::canvasSizeChanged, this, [this](int width, int height) {
        try {
            // Update canvas size in the engine
            SvgDocument* doc = m_svgEngine->getCurrentDocument();
            if (doc) {
                doc->setWidth(width);
                doc->setHeight(height);

                // Create new background and outline items instead of reloading everything
                QGraphicsScene* scene = m_canvasArea->scene();
                if (scene) {
                    // Update background item
                    QRectF newRect(0, 0, width, height);
                    
                    // Find and update the background item
                    QList<QGraphicsItem*> items = scene->items();
                    for (QGraphicsItem* item : items) {
                        if (QGraphicsRectItem* rectItem = dynamic_cast<QGraphicsRectItem*>(item)) {
                            // Check if this is the background item (z-value -1)
                            if (rectItem->zValue() == -1) {
                                rectItem->setRect(newRect);
                                break;
                            }
                        }
                    }
                    
                    // Find and update the outline item
                    for (QGraphicsItem* item : items) {
                        if (QGraphicsRectItem* rectItem = dynamic_cast<QGraphicsRectItem*>(item)) {
                            // Check if this is the outline item (z-value MAX_N)
                            if (rectItem->zValue() == 25565) { // MAX_N value
                                rectItem->setRect(newRect);
                                break;
                            }
                        }
                    }
                    
                    // Update scene rect with padding
                    scene->setSceneRect(newRect.adjusted(-10, -10, 10, 10));
                }

                m_documentModified = true;
                updateTitle();
                showStatusMessage(tr("Canvas size changed to %1x%2").arg(width).arg(height), 2000);
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

        // Update the background item directly instead of reloading everything
        QGraphicsScene* scene = m_canvasArea->scene();
        if (scene) {
            QList<QGraphicsItem*> items = scene->items();
            for (QGraphicsItem* item : items) {
                if (QGraphicsRectItem* rectItem = dynamic_cast<QGraphicsRectItem*>(item)) {
                    // Check if this is the background item (z-value -1)
                    if (rectItem->zValue() == -1) {
                        rectItem->setBrush(QBrush(color));
                        break;
                    }
                }
            }
        }

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
    connect(m_leftSideBar, &LeftSideBar::zoomOutRequested, m_canvasArea, &CanvasArea::zoomOut);
    connect(m_leftSideBar, &LeftSideBar::resetZoomRequested, m_canvasArea, &CanvasArea::resetZoom);
    connect(m_leftSideBar, &LeftSideBar::fitToWindowRequested, m_canvasArea, &CanvasArea::fitToView);

    // Connect tool signals
    connect(m_leftSideBar, &LeftSideBar::dragToolRequested, this, [this]() { handleToolSelected(4); });
    connect(m_leftSideBar, &LeftSideBar::zoomToolRequested, this, [this]() { handleToolSelected(5); });

    // Connect shape toolbar signals
    connect(m_shapeToolBar, &ShapeToolBar::shapeToolSelected, this, &MainWindow::handleShapeToolSelected);

    // Connect shape created signal to mark document as modified
    connect(m_canvasArea, &CanvasArea::shapeCreated, this, [this](QGraphicsItem*) {
        m_documentModified = true;
        updateTitle();
        qCDebug(mainWindowLog) << "Document marked as modified due to shape creation";
    });

    // Connect item selection signal to update the right attribute bar
    connect(m_canvasArea, &CanvasArea::itemSelected, m_rightAttrBar, &RightAttrBar::updateForSelectedItem);

    // Connect property change signals to update the selected item
    connect(m_rightAttrBar, &RightAttrBar::borderColorChanged, this, &MainWindow::updateSelectedItemBorderColor);
    connect(m_rightAttrBar, &RightAttrBar::fillColorChanged, this, &MainWindow::updateSelectedItemFillColor);
    connect(m_rightAttrBar, &RightAttrBar::borderWidthChanged, this, &MainWindow::updateSelectedItemBorderWidth);
    connect(m_rightAttrBar, &RightAttrBar::borderStyleChanged, this, &MainWindow::updateSelectedItemBorderStyle);

    // Connect text property change signals
    connect(m_rightAttrBar, &RightAttrBar::textContentChanged, this, &MainWindow::updateSelectedItemTextContent);
    connect(m_rightAttrBar, &RightAttrBar::fontFamilyChanged, this, &MainWindow::updateSelectedItemFontFamily);
    connect(m_rightAttrBar, &RightAttrBar::fontSizeChanged, this, &MainWindow::updateSelectedItemFontSize);
    connect(m_rightAttrBar, &RightAttrBar::fontBoldChanged, this, &MainWindow::updateSelectedItemFontBold);
    connect(m_rightAttrBar, &RightAttrBar::fontItalicChanged, this, &MainWindow::updateSelectedItemFontItalic);
    connect(m_rightAttrBar, &RightAttrBar::textAlignmentChanged, this, &MainWindow::updateSelectedItemTextAlignment);
    connect(m_rightAttrBar, &RightAttrBar::textColorChanged, this, &MainWindow::updateSelectedItemTextColor);

    // Set the current engine for the canvas area
    m_canvasArea->setCurrentEngine(m_svgEngine);

    // Initialize undo/redo actions state
    updateUndoRedoActions();

    // Connect to CommandManager's undoRedoChanged signal
    connect(CommandManager::instance(), &CommandManager::undoRedoChanged,
            this, &MainWindow::updateUndoRedoActions);

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
    if (m_documentModified && !maybeSave()) {
        return;
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

            // Use default settings from ConfigManager
            ConfigManager* configManager = ConfigManager::instance();
            QSize defaultCanvasSize = configManager->getDefaultCanvasSize();
            m_svgEngine->createNewDocument(defaultCanvasSize.width(), defaultCanvasSize.height());
            
            // Set default background color
            QColor defaultBgColor = configManager->getDefaultCanvasBackgroundColor();
            Color bgColor;
            bgColor.r = defaultBgColor.red();
            bgColor.g = defaultBgColor.green();
            bgColor.b = defaultBgColor.blue();
            bgColor.alpha = defaultBgColor.alpha();
            m_svgEngine->getCurrentDocument()->setBackgroundColor(bgColor);

            if (m_svgEngine->saveSvgFile(fileName.toStdString())) {
                m_currentFilePath = fileName;
                m_documentModified = false;

                // Clear the command history for the new file
                CommandManager::instance()->clear();
                updateUndoRedoActions();

                loadFileWithEngine(fileName);
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
                // Clear the command history for the opened file
                CommandManager::instance()->clear();
                updateUndoRedoActions();

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

bool MainWindow::loadFileWithEngine(const QString& fileName) {
    if (!QFileInfo::exists(fileName) || !m_canvasArea->openFileWithEngine(m_svgEngine)) {
        QMessageBox::critical(this, tr("Open SVG File"),
                              tr("Could not open file '%1'.").arg(QDir::toNativeSeparators(fileName)));
        return false;
    }

    m_currentFilePath = fileName;
    m_documentModified = false;
    updateTitle();
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

    // Use the LeftSideBar's highlightButton method to handle button highlighting
    m_leftSideBar->highlightButton(toolId);

    // Update status bar
    QString toolName;
    int attrWidgetType = RightAttrBar::CommonAttributes; // Default to common attributes

    // Reset drag mode by default
    m_canvasArea->setDragMode(QGraphicsView::RubberBandDrag);

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
            attrWidgetType = RightAttrBar::TextAttributes;
            m_canvasArea->setShapeCreationMode(ShapeType::Text);
            break;
        case 4:
            toolName = tr("Drag Tool");
            attrWidgetType = RightAttrBar::CommonAttributes;
            // Enable drag mode for the canvas
            m_canvasArea->setDragMode(QGraphicsView::ScrollHandDrag);
            break;
        case 5:
            toolName = tr("Zoom Tool");
            attrWidgetType = RightAttrBar::CommonAttributes;
            // No specific canvas mode for zoom, just show the zoom tools
            break;
        default:
            toolName = tr("Unknown Tool");
            attrWidgetType = RightAttrBar::CommonAttributes;
    }

    // Switch the right attribute bar to show the appropriate widget
    m_rightAttrBar->setCurrentWidget(attrWidgetType);

    // Clear any current selection when switching tools
    m_rightAttrBar->clearSelection();
    m_canvasArea->scene()->clearSelection();

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

    // Add Edit menu with undo/redo
    QMenu* editMenu = menuBar()->addMenu(tr("Edit"));

    // Create undo action
    m_undoAction = new QAction(tr("Undo"), this);
    m_undoAction->setShortcut(QKeySequence::Undo);
    m_undoAction->setIcon(QIcon::fromTheme("edit-undo", QIcon(":/icons/undo.png")));
    m_undoAction->setEnabled(false);
    connect(m_undoAction, &QAction::triggered, this, &MainWindow::undo);
    editMenu->addAction(m_undoAction);

    // Create redo action
    m_redoAction = new QAction(tr("Redo"), this);
    m_redoAction->setShortcut(QKeySequence::Redo);
    m_redoAction->setIcon(QIcon::fromTheme("edit-redo", QIcon(":/icons/redo.png")));
    m_redoAction->setEnabled(false);
    connect(m_redoAction, &QAction::triggered, this, &MainWindow::redo);
    editMenu->addAction(m_redoAction);

    // Connect to CommandManager's undoRedoChanged signal
    connect(CommandManager::instance(), &CommandManager::undoRedoChanged,
            this, &MainWindow::updateUndoRedoActions);

    QMenu* settingsMenu = menuBar()->addMenu(tr("Settings"));

    // Add Preferences action
    QAction* preferencesAction = new QAction(tr("Preferences..."), this);
    preferencesAction->setShortcut(QKeySequence::Preferences);
    connect(preferencesAction, &QAction::triggered, this, &MainWindow::showPreferences);
    settingsMenu->addAction(preferencesAction);
    
    settingsMenu->addSeparator();

    m_languageMenu = settingsMenu->addMenu(tr("Language"));

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

    // Add undo/redo actions to toolbar
    m_mainToolBar->addAction(m_undoAction);
    m_mainToolBar->addAction(m_redoAction);

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

void MainWindow::updateSelectedItemBorderColor(const QColor& color)
{
    QGraphicsItem* selectedItem = m_canvasArea->getSelectedItem();
    if (!selectedItem) {
        qCWarning(mainWindowLog) << "No item selected for border color update";
        return;
    }

    QPen pen;

    // Get the current pen from the item
    if (auto lineItem = dynamic_cast<QGraphicsLineItem*>(selectedItem)) {
        pen = lineItem->pen();
        pen.setColor(color);
        lineItem->setPen(pen);
    } else if (auto rectItem = dynamic_cast<QGraphicsRectItem*>(selectedItem)) {
        pen = rectItem->pen();
        pen.setColor(color);
        rectItem->setPen(pen);
    } else if (auto ellipseItem = dynamic_cast<QGraphicsEllipseItem*>(selectedItem)) {
        pen = ellipseItem->pen();
        pen.setColor(color);
        ellipseItem->setPen(pen);
    } else if (auto polygonItem = dynamic_cast<QGraphicsPolygonItem*>(selectedItem)) {
        pen = polygonItem->pen();
        pen.setColor(color);
        polygonItem->setPen(pen);
    } else if (auto pathItem = dynamic_cast<QGraphicsPathItem*>(selectedItem)) {
        pen = pathItem->pen();
        pen.setColor(color);
        pathItem->setPen(pen);
    }

    // Synchronize changes to SVG document
    syncItemToSvgDocument(selectedItem);

    // Mark document as modified
    m_documentModified = true;
    updateTitle();

    qCDebug(mainWindowLog) << "Updated selected item border color to:" << color.name();
}

void MainWindow::updateSelectedItemFillColor(const QColor& color)
{
    QGraphicsItem* selectedItem = m_canvasArea->getSelectedItem();
    if (!selectedItem) {
        qCWarning(mainWindowLog) << "No item selected for fill color update";
        return;
    }

    QBrush brush(color);

    // Apply the brush to the item
    if (auto rectItem = dynamic_cast<QGraphicsRectItem*>(selectedItem)) {
        rectItem->setBrush(brush);
    } else if (auto ellipseItem = dynamic_cast<QGraphicsEllipseItem*>(selectedItem)) {
        ellipseItem->setBrush(brush);
    } else if (auto polygonItem = dynamic_cast<QGraphicsPolygonItem*>(selectedItem)) {
        polygonItem->setBrush(brush);
    } else if (auto pathItem = dynamic_cast<QGraphicsPathItem*>(selectedItem)) {
        pathItem->setBrush(brush);
    }

    // Synchronize changes to SVG document
    syncItemToSvgDocument(selectedItem);

    // Mark document as modified
    m_documentModified = true;
    updateTitle();

    qCDebug(mainWindowLog) << "Updated selected item fill color to:" << color.name();
}

void MainWindow::updateSelectedItemBorderWidth(int width)
{
    QGraphicsItem* selectedItem = m_canvasArea->getSelectedItem();
    if (!selectedItem) {
        qCWarning(mainWindowLog) << "No item selected for border width update";
        return;
    }

    QPen pen;

    // Get the current pen from the item and update its width
    if (auto lineItem = dynamic_cast<QGraphicsLineItem*>(selectedItem)) {
        pen = lineItem->pen();
        pen.setWidth(width);
        lineItem->setPen(pen);
    } else if (auto rectItem = dynamic_cast<QGraphicsRectItem*>(selectedItem)) {
        pen = rectItem->pen();
        pen.setWidth(width);
        rectItem->setPen(pen);
    } else if (auto ellipseItem = dynamic_cast<QGraphicsEllipseItem*>(selectedItem)) {
        pen = ellipseItem->pen();
        pen.setWidth(width);
        ellipseItem->setPen(pen);
    } else if (auto polygonItem = dynamic_cast<QGraphicsPolygonItem*>(selectedItem)) {
        pen = polygonItem->pen();
        pen.setWidth(width);
        polygonItem->setPen(pen);
    } else if (auto pathItem = dynamic_cast<QGraphicsPathItem*>(selectedItem)) {
        pen = pathItem->pen();
        pen.setWidth(width);
        pathItem->setPen(pen);
    }

    // Synchronize changes to SVG document
    syncItemToSvgDocument(selectedItem);

    // Mark document as modified
    m_documentModified = true;
    updateTitle();

    qCDebug(mainWindowLog) << "Updated selected item border width to:" << width;
}

void MainWindow::updateSelectedItemBorderStyle(Qt::PenStyle style)
{
    QGraphicsItem* selectedItem = m_canvasArea->getSelectedItem();
    if (!selectedItem) {
        qCWarning(mainWindowLog) << "No item selected for border style update";
        return;
    }

    QPen pen;

    // Get the current pen from the item and update its style
    if (auto lineItem = dynamic_cast<QGraphicsLineItem*>(selectedItem)) {
        pen = lineItem->pen();
        pen.setStyle(style);
        lineItem->setPen(pen);
    } else if (auto rectItem = dynamic_cast<QGraphicsRectItem*>(selectedItem)) {
        pen = rectItem->pen();
        pen.setStyle(style);
        rectItem->setPen(pen);
    } else if (auto ellipseItem = dynamic_cast<QGraphicsEllipseItem*>(selectedItem)) {
        pen = ellipseItem->pen();
        pen.setStyle(style);
        ellipseItem->setPen(pen);
    } else if (auto polygonItem = dynamic_cast<QGraphicsPolygonItem*>(selectedItem)) {
        pen = polygonItem->pen();
        pen.setStyle(style);
        polygonItem->setPen(pen);
    } else if (auto pathItem = dynamic_cast<QGraphicsPathItem*>(selectedItem)) {
        pen = pathItem->pen();
        pen.setStyle(style);
        pathItem->setPen(pen);
    }

    // Synchronize changes to SVG document
    syncItemToSvgDocument(selectedItem);

    // Mark document as modified
    m_documentModified = true;
    updateTitle();

    qCDebug(mainWindowLog) << "Updated selected item border style to:" << static_cast<int>(style);
}

void MainWindow::updateSelectedItemTextContent(const QString& text)
{
    QGraphicsItem* selectedItem = m_canvasArea->getSelectedItem();
    if (!selectedItem) {
        qCWarning(mainWindowLog) << "No item selected for text content update";
        return;
    }

    if (auto textItem = dynamic_cast<EditableTextItem*>(selectedItem)) {
        // For our new EditableTextItem
        QString oldText = textItem->toPlainString();
        if (oldText != text) {
            // Create and execute the command
            auto command = std::make_unique<ModifyTextCommand>(textItem, oldText, text);
            CommandManager::instance()->executeCommand(std::move(command));
            
            // Synchronize changes to SVG document
            syncItemToSvgDocument(selectedItem);
            m_documentModified = true;
            updateTitle();
            qCDebug(mainWindowLog) << "Updated editable text content to:" << text;
        }
    }
    else if (auto textItem = dynamic_cast<QGraphicsSimpleTextItem*>(selectedItem)) {
        // For backward compatibility
        QString oldText = textItem->text();
        if (oldText != text) {
            textItem->setText(text);
            // Synchronize changes to SVG document
            syncItemToSvgDocument(selectedItem);
            m_documentModified = true;
            updateTitle();
            qCDebug(mainWindowLog) << "Updated simple text content to:" << text;
        }
    }
}

void MainWindow::updateSelectedItemFontFamily(const QString& family)
{
    QGraphicsItem* selectedItem = m_canvasArea->getSelectedItem();
    if (!selectedItem) {
        qCWarning(mainWindowLog) << "No item selected for font family update";
        return;
    }

    if (auto textItem = dynamic_cast<EditableTextItem*>(selectedItem)) {
        // For our new EditableTextItem
        QString oldFamily = textItem->font().family();
        if (oldFamily != family) {
            // Create and execute the command
            auto command = std::make_unique<ModifyTextCommand>(textItem, oldFamily, family, TextModificationType::FontFamily);
            CommandManager::instance()->executeCommand(std::move(command));
            
            // Synchronize changes to SVG document
            syncItemToSvgDocument(selectedItem);
            m_documentModified = true;
            updateTitle();
            qCDebug(mainWindowLog) << "Updated editable text font family to:" << family;
        }
    }
    else if (auto textItem = dynamic_cast<QGraphicsSimpleTextItem*>(selectedItem)) {
        // For backward compatibility
        QFont font = textItem->font();
        QString oldFamily = font.family();
        if (oldFamily != family) {
            font.setFamily(family);
            textItem->setFont(font);
            // Synchronize changes to SVG document
            syncItemToSvgDocument(selectedItem);
            m_documentModified = true;
            updateTitle();
            qCDebug(mainWindowLog) << "Updated simple text font family to:" << family;
        }
    }
}

void MainWindow::updateSelectedItemFontSize(int size)
{
    QGraphicsItem* selectedItem = m_canvasArea->getSelectedItem();
    if (!selectedItem) {
        qCWarning(mainWindowLog) << "No item selected for font size update";
        return;
    }

    if (auto textItem = dynamic_cast<EditableTextItem*>(selectedItem)) {
        // For our new EditableTextItem
        int oldSize = textItem->font().pointSize();
        if (oldSize != size) {
            // Create and execute the command
            auto command = std::make_unique<ModifyTextCommand>(textItem, oldSize, size);
            CommandManager::instance()->executeCommand(std::move(command));
            
            // Synchronize changes to SVG document
            syncItemToSvgDocument(selectedItem);
            m_documentModified = true;
            updateTitle();
            qCDebug(mainWindowLog) << "Updated editable text font size to:" << size;
        }
    }
    else if (auto textItem = dynamic_cast<QGraphicsSimpleTextItem*>(selectedItem)) {
        // For backward compatibility
        QFont font = textItem->font();
        int oldSize = font.pointSize();
        if (oldSize != size) {
            font.setPointSize(size);
            textItem->setFont(font);
            // Synchronize changes to SVG document
            syncItemToSvgDocument(selectedItem);
            m_documentModified = true;
            updateTitle();
            qCDebug(mainWindowLog) << "Updated simple text font size to:" << size;
        }
    }
}

void MainWindow::updateSelectedItemFontBold(bool bold)
{
    QGraphicsItem* selectedItem = m_canvasArea->getSelectedItem();
    if (!selectedItem) {
        qCWarning(mainWindowLog) << "No item selected for font bold update";
        return;
    }

    if (auto textItem = dynamic_cast<EditableTextItem*>(selectedItem)) {
        // For our new EditableTextItem
        bool oldBold = textItem->isBold();
        if (oldBold != bold) {
            // Create and execute the command
            auto command = std::make_unique<ModifyTextCommand>(textItem, oldBold, bold, TextModificationType::FontBold);
            CommandManager::instance()->executeCommand(std::move(command));
            
            // Synchronize changes to SVG document
            syncItemToSvgDocument(selectedItem);
            m_documentModified = true;
            updateTitle();
            qCDebug(mainWindowLog) << "Updated editable text font bold to:" << (bold ? "true" : "false");
        }
    }
    else if (auto textItem = dynamic_cast<QGraphicsSimpleTextItem*>(selectedItem)) {
        // For backward compatibility
        QFont font = textItem->font();
        bool oldBold = font.bold();
        if (oldBold != bold) {
            font.setBold(bold);
            textItem->setFont(font);
            // Synchronize changes to SVG document
            syncItemToSvgDocument(selectedItem);
            m_documentModified = true;
            updateTitle();
            qCDebug(mainWindowLog) << "Updated simple text font bold to:" << (bold ? "true" : "false");
        }
    }
}

void MainWindow::updateSelectedItemFontItalic(bool italic)
{
    QGraphicsItem* selectedItem = m_canvasArea->getSelectedItem();
    if (!selectedItem) {
        qCWarning(mainWindowLog) << "No item selected for font italic update";
        return;
    }

    if (auto textItem = dynamic_cast<EditableTextItem*>(selectedItem)) {
        // For our new EditableTextItem
        bool oldItalic = textItem->isItalic();
        if (oldItalic != italic) {
            // Create and execute the command
            auto command = std::make_unique<ModifyTextCommand>(textItem, oldItalic, italic, TextModificationType::FontItalic);
            CommandManager::instance()->executeCommand(std::move(command));
            
            // Synchronize changes to SVG document
            syncItemToSvgDocument(selectedItem);
            m_documentModified = true;
            updateTitle();
            qCDebug(mainWindowLog) << "Updated editable text font italic to:" << (italic ? "true" : "false");
        }
    }
    else if (auto textItem = dynamic_cast<QGraphicsSimpleTextItem*>(selectedItem)) {
        // For backward compatibility
        QFont font = textItem->font();
        bool oldItalic = font.italic();
        if (oldItalic != italic) {
            font.setItalic(italic);
            textItem->setFont(font);
            // Synchronize changes to SVG document
            syncItemToSvgDocument(selectedItem);
            m_documentModified = true;
            updateTitle();
            qCDebug(mainWindowLog) << "Updated simple text font italic to:" << (italic ? "true" : "false");
        }
    }
}

void MainWindow::updateSelectedItemTextAlignment(int alignment)
{
    QGraphicsItem* selectedItem = m_canvasArea->getSelectedItem();
    if (!selectedItem) {
        qCWarning(mainWindowLog) << "No item selected for text alignment update";
        return;
    }

    if (auto textItem = dynamic_cast<EditableTextItem*>(selectedItem)) {
        // For our new EditableTextItem
        Qt::Alignment oldAlignment = textItem->textAlignment();
        Qt::Alignment textAlignment = Qt::AlignLeft;
        switch (alignment) {
            case 0: textAlignment = Qt::AlignLeft; break;
            case 1: textAlignment = Qt::AlignCenter; break;
            case 2: textAlignment = Qt::AlignRight; break;
            default: textAlignment = Qt::AlignLeft; break;
        }
        
        if (oldAlignment != textAlignment) {
            // Create and execute the command
            auto command = std::make_unique<ModifyTextCommand>(textItem, oldAlignment, textAlignment);
            CommandManager::instance()->executeCommand(std::move(command));
            
            // Synchronize changes to SVG document
            syncItemToSvgDocument(selectedItem);
            m_documentModified = true;
            updateTitle();
            qCDebug(mainWindowLog) << "Updated editable text alignment to:" << alignment;
        }
    }
    else if (auto textItem = dynamic_cast<QGraphicsSimpleTextItem*>(selectedItem)) {
        // QGraphicsSimpleTextItem doesn't support alignment directly
        // In a real implementation, we would need to handle this by adjusting the text position
        qCDebug(mainWindowLog) << "Simple text item doesn't support alignment directly. Alignment index:" << alignment;
    }
}

void MainWindow::updateSelectedItemTextColor(const QColor& color)
{
    QGraphicsItem* selectedItem = m_canvasArea->getSelectedItem();
    if (!selectedItem) {
        qCWarning(mainWindowLog) << "No item selected for text color update";
        return;
    }

    if (auto textItem = dynamic_cast<EditableTextItem*>(selectedItem)) {
        // For our new EditableTextItem
        QColor oldColor = textItem->defaultTextColor();
        if (oldColor != color) {
            // Create and execute the command
            auto command = std::make_unique<ModifyTextCommand>(textItem, oldColor, color);
            CommandManager::instance()->executeCommand(std::move(command));
            
            // Synchronize changes to SVG document
            syncItemToSvgDocument(selectedItem);
            m_documentModified = true;
            updateTitle();
            qCDebug(mainWindowLog) << "Updated editable text color to:" << color.name();
        }
    }
    else if (auto textItem = dynamic_cast<QGraphicsSimpleTextItem*>(selectedItem)) {
        // For backward compatibility
        QColor oldColor = textItem->brush().color();
        if (oldColor != color) {
            textItem->setBrush(QBrush(color));
            // Synchronize changes to SVG document
            syncItemToSvgDocument(selectedItem);
            m_documentModified = true;
            updateTitle();
            qCDebug(mainWindowLog) << "Updated simple text color to:" << color.name();
        }
    }
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

void MainWindow::syncItemToSvgDocument(QGraphicsItem* item)
{
    if (!item || !m_svgEngine || !m_svgEngine->getCurrentDocument()) {
        return;
    }

    SvgDocument* doc = m_svgEngine->getCurrentDocument();
    const auto& elements = doc->getElements();
    
    // Find the corresponding graphics item index in the document
    int itemIndex = -1;
    for (int i = 0; i < doc->m_graphicsItems.size(); ++i) {
        if (doc->m_graphicsItems[i] == item) {
            itemIndex = i;
            break;
        }
    }

    if (itemIndex == -1 || itemIndex >= elements.size()) {
        qCWarning(mainWindowLog) << "Could not find corresponding SVG element for graphics item";
        return;
    }

    // Get the corresponding SVG element
    SvgElement* svgElement = elements[itemIndex].get();
    if (!svgElement) {
        return;
    }

    // Update the SVG element properties based on the graphics item
    if (auto lineItem = dynamic_cast<QGraphicsLineItem*>(item)) {
        QPen pen = lineItem->pen();
        Color strokeColor(pen.color().red(), pen.color().green(), pen.color().blue(), pen.color().alpha());
        svgElement->setStrokeColor(strokeColor);
        svgElement->setStrokeWidth(pen.width());
        svgElement->setOpacity(item->opacity());
    }
    else if (auto rectItem = dynamic_cast<QGraphicsRectItem*>(item)) {
        QPen pen = rectItem->pen();
        QBrush brush = rectItem->brush();
        Color strokeColor(pen.color().red(), pen.color().green(), pen.color().blue(), pen.color().alpha());
        Color fillColor(brush.color().red(), brush.color().green(), brush.color().blue(), brush.color().alpha());
        svgElement->setStrokeColor(strokeColor);
        svgElement->setFillColor(fillColor);
        svgElement->setStrokeWidth(pen.width());
        svgElement->setOpacity(item->opacity());
    }
    else if (auto ellipseItem = dynamic_cast<QGraphicsEllipseItem*>(item)) {
        QPen pen = ellipseItem->pen();
        QBrush brush = ellipseItem->brush();
        Color strokeColor(pen.color().red(), pen.color().green(), pen.color().blue(), pen.color().alpha());
        Color fillColor(brush.color().red(), brush.color().green(), brush.color().blue(), brush.color().alpha());
        svgElement->setStrokeColor(strokeColor);
        svgElement->setFillColor(fillColor);
        svgElement->setStrokeWidth(pen.width());
        svgElement->setOpacity(item->opacity());
    }
    else if (auto polygonItem = dynamic_cast<QGraphicsPolygonItem*>(item)) {
        QPen pen = polygonItem->pen();
        QBrush brush = polygonItem->brush();
        Color strokeColor(pen.color().red(), pen.color().green(), pen.color().blue(), pen.color().alpha());
        Color fillColor(brush.color().red(), brush.color().green(), brush.color().blue(), brush.color().alpha());
        svgElement->setStrokeColor(strokeColor);
        svgElement->setFillColor(fillColor);
        svgElement->setStrokeWidth(pen.width());
        svgElement->setOpacity(item->opacity());
    }
    else if (auto pathItem = dynamic_cast<QGraphicsPathItem*>(item)) {
        QPen pen = pathItem->pen();
        QBrush brush = pathItem->brush();
        Color strokeColor(pen.color().red(), pen.color().green(), pen.color().blue(), pen.color().alpha());
        svgElement->setStrokeColor(strokeColor);
        svgElement->setStrokeWidth(pen.width());
        svgElement->setOpacity(item->opacity());
    }
    else if (auto textItem = dynamic_cast<EditableTextItem*>(item)) {
        // Handle editable text item properties
        QColor color = textItem->defaultTextColor();
        Color fillColor(color.red(), color.green(), color.blue(), color.alpha());
        svgElement->setFillColor(fillColor);
        svgElement->setOpacity(item->opacity());
        
        // Update text-specific properties if this is a text element
        if (auto svgTextElement = dynamic_cast<SvgText*>(svgElement)) {
            svgTextElement->setTextContent(textItem->toPlainString().toStdString());
            QFont font = textItem->font();
            svgTextElement->setFontFamily(font.family().toStdString());
            svgTextElement->setFontSize(font.pointSizeF());
            svgTextElement->setBold(textItem->isBold());
            svgTextElement->setItalic(textItem->isItalic());
        }
    }
    else if (auto textItem = dynamic_cast<QGraphicsSimpleTextItem*>(item)) {
        // Handle simple text item properties
        QColor color = textItem->brush().color();
        Color fillColor(color.red(), color.green(), color.blue(), color.alpha());
        svgElement->setFillColor(fillColor);
        svgElement->setOpacity(item->opacity());
        
        // Update text-specific properties if this is a text element
        if (auto svgTextElement = dynamic_cast<SvgText*>(svgElement)) {
            svgTextElement->setTextContent(textItem->text().toStdString());
            QFont font = textItem->font();
            svgTextElement->setFontFamily(font.family().toStdString());
            svgTextElement->setFontSize(font.pointSizeF());
            svgTextElement->setBold(font.bold());
            svgTextElement->setItalic(font.italic());
        }
    }

    qCDebug(mainWindowLog) << "Synchronized graphics item properties to SVG element at index:" << itemIndex;
}

void MainWindow::showPreferences()
{
    qCDebug(mainWindowLog) << "Opening preferences dialog";
    
    ConfigDialog dialog(this);
    int result = dialog.exec();
    
    if (result == QDialog::Accepted) {
        qCDebug(mainWindowLog) << "Preferences saved";
        showStatusMessage(tr("Preferences updated"), 2000);
    } else {
        qCDebug(mainWindowLog) << "Preferences cancelled";
    }
}