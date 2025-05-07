#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
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