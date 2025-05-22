#pragma once
#include "SvgElement.h"

class SvgText : public SvgElement {
private: 
    Point m_position; 
    std::string m_textContent; 
    std::string m_fontFamily; 
    double m_fontSize;
    // 更多文本属性: font-weight, font-style, text-anchor, etc.

public: 
    SvgText(Point pos = {0,0}, const std::string& text = "");
    
    SvgElementType getType() const override { return SvgElementType::Text; }
    std::string toSvgString() const override;
    
    // ---------- Getter & Setter ----------

    Point getPosition() const { return m_position; } 
    void setPosition(const Point& p);
    
    std::string getTextContent() const { return m_textContent; } 
    void setTextContent(const std::string& text);
    
    std::string getFontFamily() const { return m_fontFamily; } 
    void setFontFamily(const std::string& family);
    
    double getFontSize() const { return m_fontSize; } 
    void setFontSize(double size);
};
