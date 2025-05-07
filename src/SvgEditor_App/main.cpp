#include <QApplication> 
#include "mainwindow.h" 

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);

    // HiDPI support
    QApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    QApplication::setAttribute(Qt::AA_UseHighDpiPixmaps);

    MainWindow mainWindow;
    mainWindow.show();

    return app.exec();
}