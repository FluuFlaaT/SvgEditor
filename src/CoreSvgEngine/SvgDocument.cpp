#include "SvgDocument.h"
#include <iostream>
#include <sstream>
#include <algorithm>

// 析构函数
SvgDocument::~SvgDocument() {
    for (SvgElement* elem : m_elements) {
        delete elem;
    }
    m_elements.clear();
}

// 添加元素
void SvgDocument::addElement(SvgElement* element) {
    if (element) {
        m_elements.push_back(element);
    }
}

// 根据ID移除元素
bool SvgDocument::removeElementById(const std::string& id) {
    auto it = std::remove_if(m_elements.begin(), m_elements.end(),
                             [&id](SvgElement* elem){ return elem && elem->getID() == id; });
    if (it != m_elements.end()) {
        for (auto del_it = it; del_it != m_elements.end(); ++del_it) {
            delete *del_it; // 删除找到的元素
        }
        m_elements.erase(it, m_elements.end());
        return true;
    }
    return false;
}

// 根据指针移除元素
bool SvgDocument::removeElement(SvgElement* element) {
    auto it = std::find(m_elements.begin(), m_elements.end(), element);
    if (it != m_elements.end()) {
        m_elements.erase(it);
        delete element; // 删除元素
        return true;
    }
    return false;
}

// 清除所有元素
void SvgDocument::clearElements() {
    for (SvgElement* elem : m_elements) {
        delete elem;
    }
    m_elements.clear();
}

// 生成SVG内容
std::string SvgDocument::generateSvgContent() const {
    std::stringstream ss;
    ss << "<svg width=\"" << m_width << "\" height=\"" << m_height << "\" xmlns=\"http://www.w3.org/2000/svg\">\n";
    // 如果背景色不是完全透明的白色，则添加背景矩形
    if (m_backgroundColor.alpha > 0 && 
        !(m_backgroundColor.r == 255 && m_backgroundColor.g == 255 && 
        m_backgroundColor.b == 255 && m_backgroundColor.alpha == 255)) {
         ss << "  <rect width=\"100%\" height=\"100%\" fill=\"" << m_backgroundColor.toString() << "\" />\n";
    }
    for (const auto* elem : m_elements) {
        if (elem) {
            ss << "  " << elem->toSvgString() << "\n";
        }
    }
    ss << "</svg>";
    return ss.str();
}

// 解析SVG内容
bool SvgDocument::parseSvgContent(const std::string& content) {
    // 实际的SVG解析器会在这里解析 `content` 字符串,
    // 识别不同的SVG标签 (line, rect, circle, etc.) 和它们的属性,
    // 然后创建相应的 SvgElement 对象并添加到 m_elements 中。
    // 例如，它会查找 <svg> 标签获取 width 和 height。
    // 这里仅为占位符。
    std::cout << "SvgDocument::parseSvgContent - Placeholder for actual SVG parsing of: " 
              << content.substr(0, 100) << "...\n";
    // 假设解析总是成功的
    return true;
}