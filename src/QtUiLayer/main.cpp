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
    
    QTranslator translator;
    QString locale = QLocale::system().name();
    LoggingService::getInstance().info("System locale: " + locale.toStdString());
    
    QString translationsPath = QApplication::applicationDirPath() + "/translations/";
    bool translationLoaded = false;
    
    QDir translationDir(translationsPath);
    LoggingService::getInstance().info(std::string("translationsPath = ") + translationsPath.toStdString());
    if (!translationDir.exists()) {
        translationsPath = QDir::currentPath() + "/translations/";
        translationDir.setPath(translationsPath);
        if (!translationDir.exists()) {
            LoggingService::getInstance().warn("Translations directory not found.");
        }
    }
    
    if (translator.load(locale, translationsPath)) {
        a.installTranslator(&translator);
        LoggingService::getInstance().info("Loaded translation: " + locale.toStdString());
        translationLoaded = true;
    } else {
        if (translator.load("en_US", translationsPath)) {
            a.installTranslator(&translator);
            LoggingService::getInstance().info("Loaded fallback translation: en_US");
            translationLoaded = true;
        } else {
            LoggingService::getInstance().warn("No translations could be loaded.");
        }
    }
    
    MainWindow w;
    w.show();
    
    int result = a.exec();
    
    LoggingService::getInstance().info("Application exiting...");
    return result;
}
