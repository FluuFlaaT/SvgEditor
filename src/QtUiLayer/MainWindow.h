#pragma once
#include <QMainWindow>
#include <QMenu>
#include <QMenuBar>
#include <QAction>
#include <QToolBar>
#include <QStatusBar>
#include <QFileDialog>
#include <QMessageBox>
#include <QHBoxLayout>
#include <QTranslator>
#include "CoreSvgEngine.h"
#include "LoggingService.h"
#include "DrawingLayer.h"
#include "LeftSideBar.h"
#include "RightAttrBar.h"

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    
    void changeEvent(QEvent* event) override;

private slots:
    void newFile();
    void openFile();
    void saveFile();
    void saveFileAs();
    void exportToPNG();
    
    void handleToolSelected(int toolId);
    void createCircle();

private:
    DrawingLayer* m_drawingLayer;
    LeftSideBar* m_leftSideBar;
    RightAttrBar* m_rightAttrBar;

    QMenu* m_languageMenu;
    QTranslator m_translator;  
    QString m_currentLanguage; 
    
    void setupMenus();
    void setupToolBar();
    void setupStatusBar();
    void setupLanguageMenu();
    void switchLanguage(const QString& locale); 
    QStringList getAvailableLanguages() const;  
    
    CoreSvgEngine m_svgEngine;
    QString m_currentFilePath;
    
    void updateTitle();
};
