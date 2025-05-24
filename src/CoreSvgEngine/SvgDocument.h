#pragma once
#include <vector>
#include <string>
#include <memory>
#include <QGraphicsItem>
#include <QGraphicsLineItem>
#include <QGraphicsRectItem>
#include <QGraphicsEllipseItem>
#include <QGraphicsPolygonItem>
#include <QGraphicsSimpleTextItem>
#include "svgelement.h"

namespace tinyxml2 {
    class XMLElement;
}

class SvgDocument {
private:
    std::vector<std::unique_ptr<SvgElement>> m_elements;
    double m_width;
    double m_height;
    Color m_backgroundColor;

    // SVG parsing helper methods to handle different element types
    void parseChildElements(tinyxml2::XMLElement* parentElement);
    void parseSvgLine(tinyxml2::XMLElement* element);
    void parseSvgRectangle(tinyxml2::XMLElement* element);
    void parseSvgCircle(tinyxml2::XMLElement* element);
    void parseSvgEllipse(tinyxml2::XMLElement* element);
    void parseSvgPolygon(tinyxml2::XMLElement* element);
    void parseSvgPolyline(tinyxml2::XMLElement* element);
    void parseSvgText(tinyxml2::XMLElement* element);
    void parseCommonAttributes(tinyxml2::XMLElement* element, SvgElement* svgElement);

    // Configure graphics items with consistent behavior
    void setGraphicsItemFlags(QGraphicsItem* item);

    public:
    // Default to standard A4-like dimensions with white background
    SvgDocument(double w = 600, double h = 400, Color bg = {255,255,255,255})
    : m_width(w), m_height(h), m_backgroundColor(bg) {}

    ~SvgDocument();

    // Prevent copying to avoid element ownership conflicts
    SvgDocument(const SvgDocument&) = delete;
    SvgDocument& operator=(const SvgDocument&) = delete;
    QVector<QGraphicsItem*> m_graphicsItems;

    SvgDocument(SvgDocument&&) = default;
    SvgDocument& operator=(SvgDocument&&) = default;

    void addElement(std::unique_ptr<SvgElement> element);
    bool removeElementById(const std::string& id);
    bool removeElement(const SvgElement* element_ptr);
    void clearElements();
    std::string generateSvgContent() const;
    bool parseSvgContent(const std::string& content);

    const std::vector<std::unique_ptr<SvgElement>>& getElements() const { return m_elements; }
    // Non-const version for direct modification when needed
    std::vector<std::unique_ptr<SvgElement>>& getElements() { return m_elements; }

    double getWidth() const { return m_width; }
    void setWidth(double w);

    double getHeight() const { return m_height; }
    void setHeight(double h);

    Color getBackgroundColor() const { return m_backgroundColor; }
    void setBackgroundColor(const Color& color);
};
