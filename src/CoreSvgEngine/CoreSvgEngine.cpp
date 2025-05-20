#include "CoreSvgEngine.h"
#include <iostream>
#include <fstream>
#include <sstream>

CoreSvgEngine::~CoreSvgEngine() {
    delete m_document;
    m_document = nullptr;
}

void CoreSvgEngine::createNewDocument(double width, double height, Color bgColor) {
    delete m_document;
    m_document = new SvgDocument(width, height, bgColor);
}

bool CoreSvgEngine::loadSvgFile(const std::string& filePath) {
    if (!m_document) {
        m_document = new SvgDocument(); // 如果没有文档则创建一个
    } else {
        m_document->clearElements(); // 清空现有元素
    }
    std::ifstream file(filePath);
    if (!file.is_open()) {
        std::cerr << "Error: Could not open file " << filePath << std::endl;
        return false;
    }
    std::stringstream buffer;
    buffer << file.rdbuf();
    file.close();
    return m_document->parseSvgContent(buffer.str());
}

bool CoreSvgEngine::saveSvgFile(const std::string& filePath) const {
    if (!m_document) {
        return false;
    }
    std::ofstream file(filePath);
    if (!file.is_open()) {
        std::cerr << "Error: Could not open file for writing " << filePath << std::endl;
        return false;
    }
    file << m_document->generateSvgContent();
    file.close();
    return true;
}