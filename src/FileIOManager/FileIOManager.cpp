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

    // 读取文件内容
    QByteArray fileData = file.readAll();
    file.close();

    // 创建新的SvgDocument
    SvgDocument* newDoc = new SvgDocument();
    
    // 解析SVG内容
    bool parseSuccess = newDoc->parseSvgContent(fileData.toStdString());
    if (!parseSuccess) {
        delete newDoc;
        m_lastError = "Cannot parse SVG file content";
        LoggingService::getInstance().error(m_lastError.toStdString());
        return false;
    }

    // 设置输出文档
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

    // 获取SVG内容并写入文件
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
    // 注意：这是一个简单的实现，具体实现可能需要根据DrawingView的实际接口调整
    LoggingService::getInstance().info("Exporting to PNG: " + filePath.toStdString());
    m_lastError.clear();

    if (!view) {
        m_lastError = "Invalid view";
        LoggingService::getInstance().error(m_lastError.toStdString());
        return false;
    }

    // 此处需要根据DrawingView的具体实现来完成
    // 以下为占位实现，需要在Qt层完成后更新
    m_lastError = "Export to PNG not implemented";
    LoggingService::getInstance().log(LogLevel::Warning, m_lastError.toStdString());
    return false;

    /* 完整实现可能类似以下代码：
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
