#include "MainWindow.h"
#include <QMenu>
#include <QMenuBar>
#include <QToolBar>
#include <QStatusBar>
#include <QFileDialog>
#include <QDir>
#include <QAction>

Q_LOGGING_CATEGORY(mainWindowLog, "MainWindow")

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent),
    m_leftSideBar(new LeftSideBar(this)),
    m_rightAttrBar(new RightAttrBar(this))
{
    qCInfo(mainWindowLog) << "MainWindow constructed.";
    
    setupMenus();
    setupToolBar();
    setupStatusBar();
    setupLanguageMenu();
    
    m_currentFilePath = "";
    updateTitle();
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