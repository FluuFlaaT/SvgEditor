#include <QApplication>
#include <QTranslator>
#include <QLibraryInfo>
#include <QLocale>
#include <QDir>
#include <QString>
#include <QLoggingCategory>
#include "mainwindow.h"

Q_DECLARE_LOGGING_CATEGORY(svgEditorLog)
Q_LOGGING_CATEGORY(svgEditorLog, "SvgEditor")

int main(int argc, char *argv[])
{
    qCInfo(svgEditorLog) << "Application starting...";
    
    QApplication a(argc, argv);
    
    // Load Qt's own translations for standard dialogs
    QTranslator qtTranslator;
    QString qtLocale = QLocale::system().name();
    if (qtTranslator.load("qt_" + qtLocale, QLibraryInfo::location(QLibraryInfo::TranslationsPath))) {
        a.installTranslator(&qtTranslator);
        qCInfo(svgEditorLog) << "Qt base translations loaded for locale:" << qtLocale;
    }
    
    // Load application translations
    QTranslator appTranslator;
    QString locale = QLocale::system().name();
    // Try to load translation file from resources
    if (appTranslator.load("svgeditor_" + locale, ":/i18n")) {
        a.installTranslator(&appTranslator);
        qCInfo(svgEditorLog) << "Application translations loaded for locale:" << locale;
    } else {
        // Fallback to English if system locale not available
        if (appTranslator.load("svgeditor_en", ":/i18n")) {
            a.installTranslator(&appTranslator);
            qCInfo(svgEditorLog) << "Fallback to English translations";
        } else {
            qCWarning(svgEditorLog) << "No translations loaded, using source language";
        }
    }

    MainWindow w;
    w.show();
    
    int result = a.exec();
    
    qCInfo(svgEditorLog) << "Application exiting...";
    return result;
}