#include "SvgText.h"
#include <sstream>
#include "../LoggingService/LoggingService.h"

SvgText::SvgText(Point pos, const std::string& text) : 
    m_position(pos), m_textContent(text), m_fontFamily("Arial"), m_fontSize(12.0) {
    LoggingService::getInstance().info("Creating Text element: position=(" + 
        std::to_string(pos.x) + "," + std::to_string(pos.y) + 
        "), content=\"" + text + "\", font-family=\"Arial\", font-size=12.0");
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

void SvgText::setPosition(const Point& p) {
    LoggingService::getInstance().info("Setting Text position: (" + 
        std::to_string(m_position.x) + "," + std::to_string(m_position.y) + ") to (" + 
        std::to_string(p.x) + "," + std::to_string(p.y) + ")");
    m_position = p;
}

void SvgText::setTextContent(const std::string& text) {
    LoggingService::getInstance().info("Setting Text content: \"" + m_textContent + "\" to \"" + text + "\"");
    m_textContent = text;
}

void SvgText::setFontFamily(const std::string& family) {
    LoggingService::getInstance().info("Setting Text font-family: \"" + m_fontFamily + "\" to \"" + family + "\"");
    m_fontFamily = family;
}

void SvgText::setFontSize(double size) {
    double newSize = (size > 0 ? size : 1.0);
    LoggingService::getInstance().info("Setting Text font-size: " + std::to_string(m_fontSize) + " to " + std::to_string(newSize));
    m_fontSize = newSize;
}
