#include "FileIOManager.h"
#include <fstream>
#include <sstream>
#include <QFile>
#include <QImage>
#include <QPainter>
#include "../LoggingService/LoggingService.h"

FileIOManager::FileIOManager(CoreSvgEngine* engine) : m_engine(engine), m_lastError("") {
    LoggingService::getInstance().info("Creating FileIOManager instance");
}

bool FileIOManager::openSvgFile(const QString& filePath, SvgDocument*& outDocument) {
    LoggingService::getInstance().info("Opening SVG file: " + filePath.toStdString());
    m_lastError.clear();

    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        m_lastError = "Cannot open file: " + filePath;
        LoggingService::getInstance().error(m_lastError.toStdString());
        return false;
    }

    QByteArray fileData = file.readAll();
    file.close();

    SvgDocument* newDoc = new SvgDocument();
    
    bool parseSuccess = newDoc->parseSvgContent(fileData.toStdString());
    if (!parseSuccess) {
        delete newDoc;
        m_lastError = "Cannot parse SVG file content";
        LoggingService::getInstance().error(m_lastError.toStdString());
        return false;
    }

    outDocument = newDoc;
    LoggingService::getInstance().info("Successfully opened and parsed SVG file");
    return true;
}

bool FileIOManager::saveSvgFile(const QString& filePath, SvgDocument* doc) {
    LoggingService::getInstance().info("Saving SVG file: " + filePath.toStdString());
    m_lastError.clear();

    if (!doc) {
        m_lastError = "Invalid SVG document";
        LoggingService::getInstance().error(m_lastError.toStdString());
        return false;
    }

    QFile file(filePath);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        m_lastError = "Cannot open file for writing: " + filePath;
        LoggingService::getInstance().error(m_lastError.toStdString());
        return false;
    }

    std::string svgContent = doc->generateSvgContent();
    QByteArray data(svgContent.c_str(), svgContent.length());
    
    qint64 bytesWritten = file.write(data);
    file.close();

    if (bytesWritten == -1 || bytesWritten < svgContent.length()) {
        m_lastError = "Error writing file";
        LoggingService::getInstance().error(m_lastError.toStdString());
        return false;
    }

    LoggingService::getInstance().info("Successfully saved SVG file");
    return true;
}

bool FileIOManager::exportToPng(const QString& filePath, DrawingView* view) {
    // TODO: 初步实现，占位中。
    LoggingService::getInstance().info("Exporting to PNG: " + filePath.toStdString());
    m_lastError.clear();

    if (!view) {
        m_lastError = "Invalid view";
        LoggingService::getInstance().error(m_lastError.toStdString());
        return false;
    }

    // TODO: 需要根据DrawingView的具体实现来完成
    m_lastError = "Export to PNG not implemented";
    LoggingService::getInstance().log(LogLevel::Warning, m_lastError.toStdString());
    return false;

    /* 
    QImage image(view->width(), view->height(), QImage::Format_ARGB32);
    image.fill(Qt::transparent);

    QPainter painter(&image);
    view->render(&painter);
    
    bool success = image.save(filePath, "PNG");
    if (!success) {
        m_lastError = "Failed to save PNG image";
        LoggingService::getInstance().error(m_lastError.toStdString());
    }
    return success;
    */
}

QString FileIOManager::lastError() const {
    return m_lastError;
}
