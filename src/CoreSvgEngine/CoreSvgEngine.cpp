#include "CoreSvgEngine.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include "../LoggingService/LoggingService.h"

CoreSvgEngine::CoreSvgEngine() : m_document(new SvgDocument()) {
    LoggingService::getInstance().info("Creating CoreSvgEngine instance with default document");
    createNewDocument(600, 800);
}

CoreSvgEngine::~CoreSvgEngine() {
    LoggingService::getInstance().info("Destroying CoreSvgEngine instance");
    // 换成智能指针后不需要 delete 
}

void CoreSvgEngine::createNewDocument(double width, double height, Color bgColor) {
    LoggingService::getInstance().info("Creating new document: width=" + std::to_string(width) + ", height=" + std::to_string(height));
    m_document = std::make_unique<SvgDocument>(width, height, bgColor);
}

bool CoreSvgEngine::loadSvgFile(const std::string& filePath) {
    LoggingService::getInstance().info("Loading SVG file: " + filePath);
    if (!m_document) {
        LoggingService::getInstance().info("No document instance, creating new document");
        m_document = std::make_unique<SvgDocument>(); // 如果没有文档则创建一个
    } else {
        LoggingService::getInstance().info("Clearing elements from existing document");
        m_document->clearElements(); // 清空现有元素
    }
    std::ifstream file(filePath);
    if (!file.is_open()) {
        LoggingService::getInstance().error("Failed to open file: " + filePath);
        std::cerr << "Error: Could not open file " << filePath << std::endl;
        return false;
    }    std::stringstream buffer;
    buffer << file.rdbuf();
    file.close();
    bool result = m_document->parseSvgContent(buffer.str());
    if (result) {
        LoggingService::getInstance().info("Successfully parsed SVG file content");
    } else {
        LoggingService::getInstance().error("Failed to parse SVG file content");
    }
    return result;
}

bool CoreSvgEngine::saveSvgFile(const std::string& filePath) const {
    LoggingService::getInstance().info("Saving SVG file: " + filePath);
    if (!m_document) {
        LoggingService::getInstance().error("No document instance, cannot save");
        return false;
    }    std::ofstream file(filePath);
    if (!file.is_open()) {
        LoggingService::getInstance().error("Failed to open file for writing: " + filePath);
        std::cerr << "Error: Could not open file for writing " << filePath << std::endl;
        return false;
    }
    file << m_document->generateSvgContent();
    file.close();
    LoggingService::getInstance().info("Successfully saved SVG file");
    return true;
}