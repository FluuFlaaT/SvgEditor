#include <QApplication> 
#include "mainwindow.h" 

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);

<<<<<<< Updated upstream
    // HiDPI support
=======
    // 添加HiDPI适配
>>>>>>> Stashed changes
    QApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    QApplication::setAttribute(Qt::AA_UseHighDpiPixmaps);

    MainWindow mainWindow;
    mainWindow.show();

    return app.exec();
}