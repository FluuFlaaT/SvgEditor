#include <QApplication>
#include <QTranslator>
#include <QLibraryInfo>
#include <QLocale>
#include <QDir>
#include <QString>
#include <QLoggingCategory>
#include "MainWindow.h"

Q_DECLARE_LOGGING_CATEGORY(svgEditorLog)
Q_LOGGING_CATEGORY(svgEditorLog, "SvgEditor")

int main(int argc, char *argv[])
{
    qCInfo(svgEditorLog) << "Application starting...";
    
    QApplication a(argc, argv);
    
    MainWindow w;
    w.show();
    
    int result = a.exec();
    
    qCInfo(svgEditorLog) << "Application exiting...";
    return result;
}