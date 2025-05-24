#pragma once
#include "svgelement.h"

// SVG text-anchor attribute values for text alignment
enum class TextAnchor {
    Start,  // Left-aligned for LTR text (SVG default)
    Middle, // Center-aligned
    End     // Right-aligned for LTR text
};

class SvgText : public SvgElement {
private:
    Point m_position;
    std::string m_textContent;
    std::string m_fontFamily;
    double m_fontSize;
    bool m_fontBold;
    bool m_fontItalic;
    TextAnchor m_textAnchor;

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

    bool isBold() const { return m_fontBold; }
    void setBold(bool bold);

    bool isItalic() const { return m_fontItalic; }
    void setItalic(bool italic);

    TextAnchor getTextAnchor() const { return m_textAnchor; }
    void setTextAnchor(TextAnchor anchor);

    // Helper method to convert TextAnchor enum to string
    static std::string textAnchorToString(TextAnchor anchor);
};
