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
#include "LeftSideBar.h"
#include "RightAttrBar.h"
#include "CanvasArea.h"

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
    void saveFile();
    void saveFileAs();
    void exportToPNG();
    
    void handleToolSelected(int toolId);

private:
    LeftSideBar* m_leftSideBar;
    RightAttrBar* m_rightAttrBar;
    CanvasArea* m_canvasArea;

    QMenu* m_languageMenu;
    // QDir* m_translationsDir;
    QTranslator m_translator;  
    QString m_currentLanguage; 
    QString m_translationsPath;
    QStringList m_languages;
    
    void setupMenus();
    void setupToolBar();
    void setupStatusBar();
    void setupLanguageMenu();
    void switchLanguage(const QString& locale); 
    QStringList getAvailableLanguages() const;  
    
    // CoreSvgEngine m_svgEngine;
    QString m_currentFilePath;
    
    void updateTitle();
};