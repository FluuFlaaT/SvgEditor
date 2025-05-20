#pragma once
#include <vector>
#include <string>
#include "SvgElement.h"

class SvgDocument {
private:
    std::vector<SvgElement*> m_elements;
    double m_width;
    double m_height;
    Color m_backgroundColor;

public:
    SvgDocument(double w = 600, double h = 400, Color bg = {255,255,255,255}) // 默认白色背景
        : m_width(w), m_height(h), m_backgroundColor(bg) {}
    
    ~SvgDocument();

    SvgDocument(const SvgDocument&) = delete;
    SvgDocument& operator=(const SvgDocument&) = delete;
    
    SvgDocument(SvgDocument&&) = default;
    SvgDocument& operator=(SvgDocument&&) = default;

    void addElement(SvgElement* element);
    bool removeElementById(const std::string& id);
    bool removeElement(SvgElement* element);
    void clearElements();
    std::string generateSvgContent() const;
    bool parseSvgContent(const std::string& content);
    
    // ---------- Getter & Setter ----------
    const std::vector<SvgElement*>& getElements() const { return m_elements; }
    std::vector<SvgElement*>& getElements() { return m_elements; } // 非const版本，用于修改
    
    double getWidth() const { return m_width; }
    void setWidth(double w); 
    
    double getHeight() const { return m_height; }
    void setHeight(double h);
    
    Color getBackgroundColor() const { return m_backgroundColor; }
    void setBackgroundColor(const Color& color);
};
