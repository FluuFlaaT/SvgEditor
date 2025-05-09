#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
<<<<<<< Updated upstream
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    {
    ui->setupUi(this);

    setWindowTitle(tr("SVG Editor")); // tr() 用于国际化
    resize(800, 600);

    // TODO: 在这里添加菜单栏、工具栏、状态栏等
    }

MainWindow::~MainWindow() {
    delete ui;
}
=======
    : QMainWindow(parent), ui(new Ui::MainWindow) {
    ui->setupUi(this);

    setWindowTitle(tr("SVG Editor")); 
    resize(800, 600);

    createActions();
    createMenus();
    createStatusBar();
    createToolBar();

    ui->statusbar->showMessage(tr("Ready"));
}

MainWindow::~MainWindow() {
    delete ui;
}

void MainWindow::createActions() {
    // openActioncursor
    openAction = new QAction(tr("Open..."), this);
    openAction->setShortcut(QKeySequence::New);
    connect(openAction, &QAction::triggered, this, &MainWindow::openFile);


}

void MainWindow::createMenus() {

}

void MainWindow::createStatusBar() {

}

void MainWindow::createToolBar() {

}
>>>>>>> Stashed changes
