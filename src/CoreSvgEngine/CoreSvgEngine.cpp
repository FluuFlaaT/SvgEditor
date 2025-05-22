#include "CoreSvgEngine.h"
#include <iostream>
#include <fstream>
#include <sstream>
// #include "../LoggingService/LoggingService.h"
#include <QLoggingCategory>
#include <QString>
Q_DECLARE_LOGGING_CATEGORY(coreSvgEngineLog)
Q_LOGGING_CATEGORY(coreSvgEngineLog, "CoreSvgEngine")

CoreSvgEngine::CoreSvgEngine() : m_document(new SvgDocument()) {
    qCDebug(coreSvgEngineLog) << "Creating CoreSvgEngine instance with default document";
    createNewDocument(600, 800);
}

CoreSvgEngine::~CoreSvgEngine() {
    qCDebug(coreSvgEngineLog) << "Destroying CoreSvgEngine instance";
    // 换成智能指针后不需要 delete 
}

void CoreSvgEngine::createNewDocument(double width, double height, Color bgColor) {
    qCDebug(coreSvgEngineLog) << "Creating new document: width=" << width << ", height=" << height;
    m_document = std::make_unique<SvgDocument>(width, height, bgColor);
}

bool CoreSvgEngine::loadSvgFile(const std::string& filePath) {
    qCDebug(coreSvgEngineLog) << "Loading SVG file:" << QString::fromStdString(filePath);
    m_document.get()->clearElements();
    if (!m_document) {
        qCDebug(coreSvgEngineLog) << "No document instance, creating new document";
        m_document = std::make_unique<SvgDocument>(); // 如果没有文档则创建一个
    } else {
        qCDebug(coreSvgEngineLog) << "Clearing elements from existing document";
        m_document->clearElements(); // 清空现有元素
    }
    std::ifstream file(filePath);
    if (!file.is_open()) {
        qCWarning(coreSvgEngineLog) << "Failed to open file:" << QString::fromStdString(filePath);
        std::cerr << "Error: Could not open file " << filePath << std::endl;
        return false;
    }    
    std::stringstream buffer;
    buffer << file.rdbuf();
    file.close();
    bool result = m_document->parseSvgContent(buffer.str());
    if (result) {
        qCDebug(coreSvgEngineLog) << "Successfully parsed SVG file content";
    } else {
        qCWarning(coreSvgEngineLog) << "Failed to parse SVG file content";
    }
    return result;
}

bool CoreSvgEngine::saveSvgFile(const std::string& filePath) const {
    qCDebug(coreSvgEngineLog) << "Saving SVG file:" << QString::fromStdString(filePath) ;
    if (!m_document) {
        qCWarning(coreSvgEngineLog) << "No document instance, cannot save";
        return false;
    }
    std::ofstream file(filePath);
    if (!file.is_open()) {
        qCWarning(coreSvgEngineLog) << "Failed to open file for writing:" << QString::fromStdString(filePath);
        std::cerr << "Error: Could not open file for writing " << filePath << std::endl;
        return false;
    }
    file << m_document->generateSvgContent();
    file.close();
    qCDebug(coreSvgEngineLog) << "Successfully saved SVG file:" << QString::fromStdString(filePath);
    return true;
}