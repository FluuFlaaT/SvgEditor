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
#include "LeftSideBar.h"
#include "RightAttrBar.h"
#include "CanvasArea.h"
#include "../CoreSvgEngine/CoreSvgEngine.h"

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
    bool loadFile(const QString&);
    bool loadFileWithEngine(const QString& fileName);
    void saveFile();
    void saveFileAs();
    void exportToPNG();

    void handleToolSelected(int toolId);
    void updateZoomStatus(qreal zoomFactor);

private:
    LeftSideBar* m_leftSideBar;
    RightAttrBar* m_rightAttrBar;
    CanvasArea* m_canvasArea;
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
};