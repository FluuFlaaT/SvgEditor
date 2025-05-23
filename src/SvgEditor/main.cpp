#include <QApplication>
#include <QTranslator>
#include <QLibraryInfo>
#include <QLocale>
#include <QDir>
#include <QString>
#include <QLoggingCategory>
#include "mainwindow.h"
#include "../Settings/settingsmanager.h"

Q_DECLARE_LOGGING_CATEGORY(svgEditorLog)
Q_LOGGING_CATEGORY(svgEditorLog, "SvgEditor")

int main(int argc, char *argv[])
{
    qCInfo(svgEditorLog) << "Application starting...";

    QApplication a(argc, argv);

    // Set application information for QSettings
    QCoreApplication::setOrganizationName("svgeditor");
    QCoreApplication::setApplicationName("SVG Editor");

    // Initialize settings manager
    SettingsManager::instance();
    qCInfo(svgEditorLog) << "Settings manager initialized";

    QTranslator translator;
    if(translator.load("", ":/i18n")) {
        a.installTranslator(&translator);
    }

    MainWindow w;
    w.show();

    int result = a.exec();

    qCInfo(svgEditorLog) << "Application exiting...";
    return result;
}