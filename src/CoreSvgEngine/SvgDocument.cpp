#include "SvgDocument.h"
#include "SvgElement.h"
#include "SvgShapes.h"
#include "SvgText.h"
#include <iostream>
#include <sstream>
#include <algorithm>
#include <tinyxml2.h>
// #include "../LoggingService/LoggingService.h"
#include <memory> 
#include <QLoggingCategory>
#include <QString>
Q_DECLARE_LOGGING_CATEGORY(svgDocumentLog)
Q_LOGGING_CATEGORY(svgDocumentLog, "SvgDocument")


SvgDocument::~SvgDocument() {
    qCDebug(svgDocumentLog) << "Destroying SVG document with" << m_elements.size() << "elements";
}

void SvgDocument::addElement(std::unique_ptr<SvgElement> element) {
    if (element) {
        LoggingService::getInstance().info("Adding new element to document, type: " + 
            std::to_string(static_cast<int>(element->getType())) + 
            (element->getID().empty() ? "" : ", ID: " + element->getID()));
        m_elements.push_back(std::move(element)); 
    }
}

bool SvgDocument::removeElementById(const std::string& id) {
    LoggingService::getInstance().info("Removing element by ID: " + id);
    auto it = std::remove_if(m_elements.begin(), m_elements.end(),
                             [&id](const std::unique_ptr<SvgElement>& elem){ return elem && elem->getID() == id; });
    if (it != m_elements.end()) {
        int removedCount = std::distance(it, m_elements.end());
        m_elements.erase(it, m_elements.end()); 
        LoggingService::getInstance().info("Successfully removed " + std::to_string(removedCount) + " element(s) with ID: " + id);
        return true;
    }
    LoggingService::getInstance().info("No element found with ID: " + id);
    return false;
}

bool SvgDocument::removeElement(const SvgElement* element_ptr) {
    if (element_ptr) {
        LoggingService::getInstance().info("Removing element by pointer, type: " + 
            std::to_string(static_cast<int>(element_ptr->getType())) + 
            (element_ptr->getID().empty() ? "" : ", ID: " + element_ptr->getID()));
    } else {
        LoggingService::getInstance().warn("Attempt to remove null element pointer");
        return false;
    }
    
    auto it = std::find_if(m_elements.begin(), m_elements.end(),
                           [element_ptr](const std::unique_ptr<SvgElement>& elem){ return elem.get() == element_ptr; });
    if (it != m_elements.end()) {
        m_elements.erase(it); 
        LoggingService::getInstance().info("Element successfully removed");
        return true;
    }
    LoggingService::getInstance().warn("Element not found in document");
    return false;
}

void SvgDocument::clearElements() {
    LoggingService::getInstance().info("Clearing all elements from document, count: " + std::to_string(m_elements.size()));
    m_elements.clear(); 
}

std::string SvgDocument::generateSvgContent() const {
    LoggingService::getInstance().info("Generating SVG content for document with " + std::to_string(m_elements.size()) + " elements");
    std::stringstream ss;
    // Use simple \" for quotes in attributes, as tinyxml2 expects standard XML format.
    ss << "<svg width=\"" << m_width << "\" height=\"" << m_height << "\" xmlns=\"http://www.w3.org/2000/svg\">\n";
    
    // 如果背景色不是完全透明的白色，则添加背景矩形
    if (m_backgroundColor.alpha > 0 && 
        !(m_backgroundColor.r == 255 && m_backgroundColor.g == 255 && 
          m_backgroundColor.b == 255 && m_backgroundColor.alpha == 255)) {
         // Ensure attributes in rect also use simple \"
         ss << "  <rect width=\"100%\" height=\"100%\" fill=\"" << m_backgroundColor.toString() << "\" />\n";
    }
    for (const auto& elem : m_elements) { // Iterate by const reference
        if (elem) {
            ss << "  " << elem->toSvgString() << "\\n";
        }
    }
    ss << "</svg>";
    return ss.str();
}

bool SvgDocument::parseSvgContent(const std::string& content) {
    LoggingService::getInstance().info("Parsing SVG content, content length: " + std::to_string(content.length()));
    
    clearElements();
    
    tinyxml2::XMLDocument doc;
    if (doc.Parse(content.c_str(), content.size()) != tinyxml2::XML_SUCCESS) {
        LoggingService::getInstance().error("Failed to parse SVG content: " + std::string(doc.ErrorStr()));
        return false;
    }
    
    tinyxml2::XMLElement* svgRootElement = doc.FirstChildElement("svg");
    if (!svgRootElement) {
        LoggingService::getInstance().error("Failed to find <svg> root element");
        return false;
    }
    
    // Parse width and height
    const char* widthStr = svgRootElement->Attribute("width");
    if (widthStr) {
        std::string width(widthStr);
        if (width.find("px") != std::string::npos) width = width.substr(0, width.find("px"));
        try { setWidth(std::stod(width)); } catch (const std::exception& e) {
            LoggingService::getInstance().warn("Failed to parse SVG width: " + std::string(e.what()));
        }
    }
    
    const char* heightStr = svgRootElement->Attribute("height");
    if (heightStr) {
        std::string height(heightStr);
        if (height.find("px") != std::string::npos) height = height.substr(0, height.find("px"));
        try { setHeight(std::stod(height)); } catch (const std::exception& e) {
            LoggingService::getInstance().warn("Failed to parse SVG height: " + std::string(e.what()));
        }
    }
    
    tinyxml2::XMLElement* childElementToParse = svgRootElement->FirstChildElement();

    // Check if the first child is a background rectangle
    if (childElementToParse && strcmp(childElementToParse->Name(), "rect") == 0) {
        const char* rectWidthAttr = childElementToParse->Attribute("width");
        const char* rectHeightAttr = childElementToParse->Attribute("height");
        const char* rectFillAttr = childElementToParse->Attribute("fill");

        if (rectWidthAttr && rectHeightAttr && rectFillAttr) {
            std::string widthVal(rectWidthAttr);
            std::string heightVal(rectHeightAttr);
            bool isFullWidth = (widthVal == "100%");
            if (!isFullWidth) try { isFullWidth = (std::stod(widthVal) >= m_width); } catch (...) {}
            
            bool isFullHeight = (heightVal == "100%");
            if (!isFullHeight) try { isFullHeight = (std::stod(heightVal) >= m_height); } catch (...) {}

            if (isFullWidth && isFullHeight) {
                setBackgroundColor(Color::fromString(rectFillAttr));
                childElementToParse = childElementToParse->NextSiblingElement(); // Skip this rect for general parsing
            }
        }
    }
    
    parseChildElements(childElementToParse); // Pass the potentially advanced child pointer
    
    LoggingService::getInstance().info("SVG content parsed successfully with " + 
                                      std::to_string(m_elements.size()) + " elements");
    return true;
}

void SvgDocument::setWidth(double w) { 
    LoggingService::getInstance().info("Setting document width: " + std::to_string(m_width) + " to " + std::to_string(w > 0 ? w : 1));
    m_width = (w > 0 ? w : 1); 
}

void SvgDocument::setHeight(double h) { 
    LoggingService::getInstance().info("Setting document height: " + std::to_string(m_height) + " to " + std::to_string(h > 0 ? h : 1));
    m_height = (h > 0 ? h : 1); 
}

void SvgDocument::setBackgroundColor(const Color& color) { 
    LoggingService::getInstance().info("Setting document background color: " + m_backgroundColor.toString() + " to " + color.toString());
    m_backgroundColor = color; 
}

// Modified to take the starting element to parse
void SvgDocument::parseChildElements(tinyxml2::XMLElement* element) {
    while (element) {
        std::string elementName = element->Name();
        
        if (elementName == "line") {
            parseSvgLine(element);
        } else if (elementName == "rect") {
            parseSvgRectangle(element);
        } else if (elementName == "circle") {
            parseSvgCircle(element);
        } else if (elementName == "ellipse") {
            parseSvgEllipse(element);
        } else if (elementName == "polygon") {
            parseSvgPolygon(element);
        } else if (elementName == "polyline") {
            parseSvgPolyline(element);
        } else if (elementName == "text") {
            parseSvgText(element);
        } else if (elementName == "g") {
            parseChildElements(element->FirstChildElement()); // Recursively parse children of <g>
        }
        
        element = element->NextSiblingElement();
    }
}

void SvgDocument::parseSvgLine(tinyxml2::XMLElement* element) {
    double x1 = 0, y1 = 0, x2 = 0, y2 = 0;
    element->QueryDoubleAttribute("x1", &x1);
    element->QueryDoubleAttribute("y1", &y1);
    element->QueryDoubleAttribute("x2", &x2);
    element->QueryDoubleAttribute("y2", &y2);
    
    auto line = std::make_unique<SvgLine>(Point{x1, y1}, Point{x2, y2});
    parseCommonAttributes(element, line.get());
    addElement(std::move(line));
}

void SvgDocument::parseSvgRectangle(tinyxml2::XMLElement* element) {
    double x = 0, y = 0, width = 0, height = 0, rx = 0, ry = 0;
    element->QueryDoubleAttribute("x", &x);
    element->QueryDoubleAttribute("y", &y);
    element->QueryDoubleAttribute("width", &width);
    element->QueryDoubleAttribute("height", &height);
    element->QueryDoubleAttribute("rx", &rx);
    element->QueryDoubleAttribute("ry", &ry);
    
    auto rect = std::make_unique<SvgRectangle>(Point{x, y}, width, height, rx, ry);
    parseCommonAttributes(element, rect.get());
    addElement(std::move(rect));
}

void SvgDocument::parseSvgCircle(tinyxml2::XMLElement* element) {
    double cx = 0, cy = 0, r = 0;
    element->QueryDoubleAttribute("cx", &cx);
    element->QueryDoubleAttribute("cy", &cy);
    element->QueryDoubleAttribute("r", &r);
    
    auto circle = std::make_unique<SvgCircle>(Point{cx, cy}, r);
    parseCommonAttributes(element, circle.get());
    addElement(std::move(circle));
}

void SvgDocument::parseSvgEllipse(tinyxml2::XMLElement* element) {
    double cx = 0, cy = 0, rx = 0, ry = 0;
    element->QueryDoubleAttribute("cx", &cx);
    element->QueryDoubleAttribute("cy", &cy);
    element->QueryDoubleAttribute("rx", &rx);
    element->QueryDoubleAttribute("ry", &ry);
    
    auto ellipse = std::make_unique<SvgEllipse>(Point{cx, cy}, rx, ry);
    parseCommonAttributes(element, ellipse.get());
    addElement(std::move(ellipse));
}

void SvgDocument::parseSvgPolygon(tinyxml2::XMLElement* element) {
    const char* pointsStr = element->Attribute("points");
    if (!pointsStr) {
        LoggingService::getInstance().warn("Polygon element missing 'points' attribute");
        return;
    }
    
    std::vector<Point> points;
    std::istringstream pointStream(pointsStr);
    std::string pointPair;
    
    while (std::getline(pointStream, pointPair, ' ')) {
        if (pointPair.empty()) continue;
        size_t commaPos = pointPair.find(',');
        if (commaPos != std::string::npos) {
            try {
                double x = std::stod(pointPair.substr(0, commaPos));
                double y = std::stod(pointPair.substr(commaPos + 1));
                points.push_back({x, y});
            } catch (const std::exception& e) {
                LoggingService::getInstance().warn("Failed to parse polygon point: " + std::string(e.what()));
            }
        }
    }
    
    if (points.size() < 3) {
        LoggingService::getInstance().warn("Polygon has fewer than 3 points, skipping");
        return;
    }
    
    auto polygon = std::make_unique<SvgPolygon>(points);
    parseCommonAttributes(element, polygon.get());
    addElement(std::move(polygon));
}

void SvgDocument::parseSvgPolyline(tinyxml2::XMLElement* element) {
    const char* pointsStr = element->Attribute("points");
    if (!pointsStr) {
        LoggingService::getInstance().warn("Polyline element missing 'points' attribute");
        return;
    }
    
    std::vector<Point> points;
    std::istringstream pointStream(pointsStr);
    std::string pointPair;
    
    while (std::getline(pointStream, pointPair, ' ')) {
        if (pointPair.empty()) continue;
        size_t commaPos = pointPair.find(',');
        if (commaPos != std::string::npos) {
            try {
                double x = std::stod(pointPair.substr(0, commaPos));
                double y = std::stod(pointPair.substr(commaPos + 1));
                points.push_back({x, y});
            } catch (const std::exception& e) {
                LoggingService::getInstance().warn("Failed to parse polyline point: " + std::string(e.what()));
            }
        }
    }
    
    if (points.size() < 2) {
        LoggingService::getInstance().warn("Polyline has fewer than 2 points, skipping");
        return;
    }
    
    auto polyline = std::make_unique<SvgPolyline>(points);
    parseCommonAttributes(element, polyline.get());
    addElement(std::move(polyline));
}

void SvgDocument::parseSvgText(tinyxml2::XMLElement* element) {
    double x = 0, y = 0;
    element->QueryDoubleAttribute("x", &x);
    element->QueryDoubleAttribute("y", &y);
    
    const char* textContent = element->GetText();
    std::string text = textContent ? textContent : "";
    
    auto textElement = std::make_unique<SvgText>(Point{x, y}, text);
    
    const char* fontFamily = element->Attribute("font-family");
    if (fontFamily) {
        textElement->setFontFamily(fontFamily);
    }
    
    double fontSize = 12.0; 
    if (element->QueryDoubleAttribute("font-size", &fontSize) == tinyxml2::XML_SUCCESS) {
        textElement->setFontSize(fontSize);
    }
    
    parseCommonAttributes(element, textElement.get());
    addElement(std::move(textElement));
}

void SvgDocument::parseCommonAttributes(tinyxml2::XMLElement* element, SvgElement* svgElement) {
    const char* id = element->Attribute("id");
    if (id) {
        svgElement->setID(id);
    }
    
    const char* fill = element->Attribute("fill");
    if (fill) {
        svgElement->setFillColor(Color::fromString(fill));
    }
    
    const char* stroke = element->Attribute("stroke");
    if (stroke) {
        svgElement->setStrokeColor(Color::fromString(stroke));
    }
    
    double strokeWidth = 1.0;
    if (element->QueryDoubleAttribute("stroke-width", &strokeWidth) == tinyxml2::XML_SUCCESS) {
        svgElement->setStrokeWidth(strokeWidth);
    }
    
    double opacity = 1.0;
    if (element->QueryDoubleAttribute("opacity", &opacity) == tinyxml2::XML_SUCCESS) {
        svgElement->setOpacity(opacity);
    }
    
    const char* transform = element->Attribute("transform");
    if (transform) {
        Transform t;
        t.transform_str = transform; 
        svgElement->setTransform(t);
    }
    
    const tinyxml2::XMLAttribute* attr = element->FirstAttribute();
    while (attr) {
        std::string name = attr->Name();
        std::string value = attr->Value();
        
        if (name != "id" && name != "fill" && name != "stroke" && 
            name != "stroke-width" && name != "opacity" && name != "transform" &&
            name != "x" && name != "y" && name != "width" && name != "height" &&
            name != "x1" && name != "y1" && name != "x2" && name != "y2" &&
            name != "cx" && name != "cy" && name != "r" && name != "rx" && name != "ry" &&
            name != "points" && name != "font-family" && name != "font-size") {
            
            try {
                double numValue = std::stod(value);
                svgElement->setAttribute(name, numValue);
            } catch (const std::exception&) {
                svgElement->setAttribute(name, value);
            }
        }
        attr = attr->Next();
    }
}