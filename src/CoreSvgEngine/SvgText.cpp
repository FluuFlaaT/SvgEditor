#include "SvgText.h"

SvgText::SvgText(Point pos, const std::string& text) : 
    m_position(pos), m_textContent(text), m_fontFamily("Arial"), m_fontSize(12.0) {
    // 文本通常有填充无边框
    setFillColor({0,0,0,255}); // 黑色填充
    setStrokeColor({0,0,0,0}); // 无边框
    setStrokeWidth(0);
}

std::string SvgText::toSvgString() const {
    std::stringstream ss;
    ss << "<text x=\"" << m_position.x << "\" y=\"" << m_position.y << "\"";
    ss << " font-family=\"" << m_fontFamily << "\"";
    ss << " font-size=\"" << m_fontSize << "\"";
    ss << getCommonAttributesString();
    ss << ">" << m_textContent << "</text>";
    return ss.str();
}
