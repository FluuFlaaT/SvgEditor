#pragma once
#include <vector>
#include <string>
#include <memory>
#include "SvgElement.h"

namespace tinyxml2 {
    class XMLElement;
}

class SvgDocument {
private:
    std::vector<std::unique_ptr<SvgElement>> m_elements;
    double m_width;
    double m_height;
    Color m_backgroundColor;

    // SVG解析帮助方法
    void parseChildElements(tinyxml2::XMLElement* parentElement);
    void parseSvgLine(tinyxml2::XMLElement* element);
    void parseSvgRectangle(tinyxml2::XMLElement* element);
    void parseSvgCircle(tinyxml2::XMLElement* element);
    void parseSvgEllipse(tinyxml2::XMLElement* element);
    void parseSvgPolygon(tinyxml2::XMLElement* element);
    void parseSvgPolyline(tinyxml2::XMLElement* element);
    void parseSvgText(tinyxml2::XMLElement* element);
    void parseCommonAttributes(tinyxml2::XMLElement* element, SvgElement* svgElement);

public:
    SvgDocument(double w = 600, double h = 400, Color bg = {255,255,255,255}) // 默认白色背景
        : m_width(w), m_height(h), m_backgroundColor(bg) {}
    
    ~SvgDocument();

    SvgDocument(const SvgDocument&) = delete;
    SvgDocument& operator=(const SvgDocument&) = delete;
    
    SvgDocument(SvgDocument&&) = default;
    SvgDocument& operator=(SvgDocument&&) = default;

    void addElement(std::unique_ptr<SvgElement> element);
    bool removeElementById(const std::string& id);
    bool removeElement(const SvgElement* element_ptr);
    void clearElements();
    std::string generateSvgContent() const;
    bool parseSvgContent(const std::string& content);
    
    // ---------- Getter & Setter ----------
    const std::vector<std::unique_ptr<SvgElement>>& getElements() const { return m_elements; }
    std::vector<std::unique_ptr<SvgElement>>& getElements() { return m_elements; } // 非const版本，用于修改
    
    double getWidth() const { return m_width; }
    void setWidth(double w); 
    
    double getHeight() const { return m_height; }
    void setHeight(double h);
    
    Color getBackgroundColor() const { return m_backgroundColor; }
    void setBackgroundColor(const Color& color);
};
