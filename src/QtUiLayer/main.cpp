#include "MainWindow.h"
#include <QApplication>
#include <QTranslator>
#include <QLibraryInfo>
#include <QLocale>
#include <QDir>
#include <QString>
#include "LoggingService.h"

int main(int argc, char *argv[])
{
    LoggingService::getInstance().info("Application starting...");
    
    QApplication a(argc, argv);
    
    MainWindow w;
    w.show();
    
    int result = a.exec();
    
    LoggingService::getInstance().info("Application exiting...");
    return result;
}
