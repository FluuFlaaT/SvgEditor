#include "SvgText.h"
#include <sstream>
#include <QLoggingCategory>
Q_DECLARE_LOGGING_CATEGORY(svgTextLog)
Q_LOGGING_CATEGORY(svgTextLog, "SvgText")

// 转义XML特殊字符
std::string escapeXmlChars(const std::string& input) {
    std::string result;
    result.reserve(input.size());
    for (char c : input) {
        switch (c) {
            case '<': result += "&lt;"; break;
            case '>': result += "&gt;"; break;
            case '&': result += "&amp;"; break;
            case '"': result += "&quot;"; break;
            case '\'': result += "&apos;"; break;
            default: result += c; break;
        }
    }
    return result;
}

SvgText::SvgText(Point pos, const std::string& text) : 
    m_position(pos), m_textContent(text), m_fontFamily("Arial"), m_fontSize(12.0) {
    qCInfo(svgTextLog) << "Creating Text element: position=(" + 
        QString::fromStdString(std::to_string(pos.x)) + "," + 
        QString::fromStdString(std::to_string(pos.y)) + 
        "), content=\"" + QString::fromStdString(text) + 
        "\", font-family=\"Arial\", font-size=12.0";
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
    // 转义文本内容中的特殊字符
    ss << ">" << escapeXmlChars(m_textContent) << "</text>";
    return ss.str();
}

void SvgText::setPosition(const Point& p) {
    qCInfo(svgTextLog) << "Setting Text position: (" + 
        QString::fromStdString(std::to_string(m_position.x)) + "," + 
        QString::fromStdString(std::to_string(m_position.y)) + ") to (" + 
        QString::fromStdString(std::to_string(p.x)) + "," + 
        QString::fromStdString(std::to_string(p.y)) + ")";
    m_position = p;
}

void SvgText::setTextContent(const std::string& text) {
    qCInfo(svgTextLog) << "Setting Text content: \"" + 
        QString::fromStdString(m_textContent) + "\" to \"" + 
        QString::fromStdString(text) + "\"";
    m_textContent = text;
}

void SvgText::setFontFamily(const std::string& family) {
    qCInfo(svgTextLog) << "Setting Text font-family: \"" + 
        QString::fromStdString(m_fontFamily) + "\" to \"" + 
        QString::fromStdString(family) + "\"";
    m_fontFamily = family;
}

void SvgText::setFontSize(double size) {
    double newSize = (size > 0 ? size : 1.0);
    qCInfo(svgTextLog) << "Setting Text font-size: " + 
        QString::fromStdString(std::to_string(m_fontSize)) + " to " + 
        QString::fromStdString(std::to_string(newSize));
    m_fontSize = newSize;
}
