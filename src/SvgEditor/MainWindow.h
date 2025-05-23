#pragma once
#include <QMainWindow>
#include <QString>
#include <QTranslator>
#include <QStringList>
#include <QLoggingCategory>
#include <QMenu>
#include <QMenuBar>
#include <QToolBar>
#include <QStatusBar>
#include <QFileDialog>
#include <QDir>
#include <QAction>
#include <QSplitter>
#include <QApplication>
#include <QStandardPaths>
#include <QMessageBox>
#include <QPainter>
#include <QLabel>
#include "leftsidebar.h"
#include "rightattrbar.h"
#include "canvasarea.h"
#include "shapetoolbar.h"
#include "../CoreSvgEngine/coresvgengine.h"
#include "../CoreSvgEngine/svgtext.h"
#include "../Commands/CommandManager.h"

Q_DECLARE_LOGGING_CATEGORY(mainWindowLog)

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void newFile();
    void openFile();
    bool loadFileWithEngine(const QString& fileName);
    void saveFile();
    void saveFileAs();
    void exportToPNG();

    void handleToolSelected(int toolId);
    void handleShapeToolSelected(ShapeType type);
    void updateZoomStatus(qreal zoomFactor);
    void showStatusMessage(const QString& message, int timeout = 2000);
    void clearStatusMessage();

    // Methods to update selected item properties
    void updateSelectedItemBorderColor(const QColor& color);
    void updateSelectedItemFillColor(const QColor& color);
    void updateSelectedItemBorderWidth(int width);
    void updateSelectedItemBorderStyle(Qt::PenStyle style);

    // Text property update slots
    void updateSelectedItemTextContent(const QString& text);
    void updateSelectedItemFontFamily(const QString& family);
    void updateSelectedItemFontSize(int size);
    void updateSelectedItemFontBold(bool bold);
    void updateSelectedItemFontItalic(bool italic);
    void updateSelectedItemTextAlignment(int alignment);
    void updateSelectedItemTextColor(const QColor& color);

    // Undo/Redo slots
    void undo();
    void redo();

private:
    LeftSideBar* m_leftSideBar;
    RightAttrBar* m_rightAttrBar;
    CanvasArea* m_canvasArea;
    ShapeToolBar* m_shapeToolBar;
    CoreSvgEngine* m_svgEngine;

    QMenu* m_languageMenu;
    // QDir* m_translationsDir;
    QTranslator m_translator;
    QString m_currentLanguage;
    QString m_translationsPath;
    QStringList m_languages;

    QToolBar* m_mainToolBar;
    QLabel* m_statusLabel;
    QLabel* m_zoomLabel;

    QTimer* m_statusTimer;
    bool m_statusMessageActive;

    void setupMenus();
    void setupToolBar();
    void setupStatusBar();
    void setupLanguageMenu();
    void switchLanguage(const QString& locale);
    QStringList getAvailableLanguages() const;

    // CoreSvgEngine m_svgEngine;
    QString m_currentFilePath;
    bool m_documentModified;

    void updateTitle();
    bool maybeSave();
    void updateRightAttrBarFromDocument();
    void updateUndoRedoActions();
    void syncItemToSvgDocument(QGraphicsItem* item);

    // Undo/Redo actions
    QAction* m_undoAction;
    QAction* m_redoAction;
};