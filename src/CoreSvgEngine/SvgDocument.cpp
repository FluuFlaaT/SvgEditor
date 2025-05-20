#include "SvgDocument.h"
#include "SvgElement.h"
#include "SvgShapes.h"
#include "SvgText.h"
#include <iostream>
#include <sstream>
#include <algorithm>
#include <tinyxml2.h>
#include "../LoggingService/LoggingService.h"

// 析构函数
SvgDocument::~SvgDocument() {
    LoggingService::getInstance().info("Destroying SVG document with " + std::to_string(m_elements.size()) + " elements");
    for (SvgElement* elem : m_elements) {
        delete elem;
    }
    m_elements.clear();
}

// 添加元素
void SvgDocument::addElement(SvgElement* element) {
    if (element) {
        LoggingService::getInstance().info("Adding new element to document, type: " + 
            std::to_string(static_cast<int>(element->getType())) + 
            (element->getID().empty() ? "" : ", ID: " + element->getID()));
        m_elements.push_back(element);
    }
}

// 根据ID移除元素
bool SvgDocument::removeElementById(const std::string& id) {
    LoggingService::getInstance().info("Removing element by ID: " + id);
    auto it = std::remove_if(m_elements.begin(), m_elements.end(),
                             [&id](SvgElement* elem){ return elem && elem->getID() == id; });
    if (it != m_elements.end()) {
        for (auto del_it = it; del_it != m_elements.end(); ++del_it) {
            delete *del_it; // 删除找到的元素
        }
        int removedCount = std::distance(it, m_elements.end());
        m_elements.erase(it, m_elements.end());
        LoggingService::getInstance().info("Successfully removed " + std::to_string(removedCount) + " element(s) with ID: " + id);
        return true;
    }
    LoggingService::getInstance().info("No element found with ID: " + id);
    return false;
}

// 根据指针移除元素
bool SvgDocument::removeElement(SvgElement* element) {
    if (element) {
        LoggingService::getInstance().info("Removing element by pointer, type: " + 
            std::to_string(static_cast<int>(element->getType())) + 
            (element->getID().empty() ? "" : ", ID: " + element->getID()));
    } else {
        LoggingService::getInstance().warn("Attempt to remove null element pointer");
        return false;
    }
    
    auto it = std::find(m_elements.begin(), m_elements.end(), element);
    if (it != m_elements.end()) {
        m_elements.erase(it);
        delete element; // 删除元素
        LoggingService::getInstance().info("Element successfully removed");
        return true;
    }
    LoggingService::getInstance().warn("Element not found in document");
    return false;
}

// 清除所有元素
void SvgDocument::clearElements() {
    LoggingService::getInstance().info("Clearing all elements from document, count: " + std::to_string(m_elements.size()));
    for (SvgElement* elem : m_elements) {
        delete elem;
    }
    m_elements.clear();
}

// 生成SVG内容
std::string SvgDocument::generateSvgContent() const {
    LoggingService::getInstance().info("Generating SVG content for document with " + std::to_string(m_elements.size()) + " elements");
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
    LoggingService::getInstance().info("Parsing SVG content, content length: " + std::to_string(content.length()));
    
    // 清除当前所有元素，准备重新加载
    clearElements();
    
    // 创建XML文档
    tinyxml2::XMLDocument doc;
    tinyxml2::XMLError xmlResult = doc.Parse(content.c_str(), content.size());
    if (xmlResult != tinyxml2::XML_SUCCESS) {
        LoggingService::getInstance().error("Failed to parse SVG content: " + std::string(doc.ErrorStr()));
        return false;
    }
    
    // 查找root <svg> 元素
    tinyxml2::XMLElement* svgElement = doc.FirstChildElement("svg");
    if (!svgElement) {
        LoggingService::getInstance().error("Failed to find <svg> root element");
        return false;
    }
    
    // 获取SVG的宽度和高度
    const char* widthStr = svgElement->Attribute("width");
    const char* heightStr = svgElement->Attribute("height");
    
    if (widthStr) {
        // 处理可能的单位（如px，mm等）
        std::string width(widthStr);
        if (width.find("px") != std::string::npos) {
            width = width.substr(0, width.find("px"));
        }
        try {
            double w = std::stod(width);
            setWidth(w);
        } catch (std::exception& e) {
            LoggingService::getInstance().warn("Failed to parse SVG width: " + std::string(e.what()));
        }
    }
    
    if (heightStr) {
        std::string height(heightStr);
        if (height.find("px") != std::string::npos) {
            height = height.substr(0, height.find("px"));
        }
        try {
            double h = std::stod(height);
            setHeight(h);
        } catch (std::exception& e) {
            LoggingService::getInstance().warn("Failed to parse SVG height: " + std::string(e.what()));
        }
    }
    
    // 查找是否有背景色定义（通常是第一个矩形元素，且覆盖整个SVG）
    tinyxml2::XMLElement* rectElement = svgElement->FirstChildElement("rect");
    if (rectElement) {
        const char* widthAttr = rectElement->Attribute("width");
        const char* heightAttr = rectElement->Attribute("height");
        const char* fillAttr = rectElement->Attribute("fill");
        
        // 检查是否是全尺寸背景矩形
        if (widthAttr && heightAttr && fillAttr) {
            std::string widthVal(widthAttr);
            std::string heightVal(heightAttr);
            
            bool isFullWidth = false;
            bool isFullHeight = false;
            
            if (widthVal == "100%") {
                isFullWidth = true;
            } else {
                try {
                    double w = std::stod(widthVal);
                    isFullWidth = (w >= m_width);
                } catch (std::exception&) {
                    // 解析失败，忽略
                }
            }
            
            if (heightVal == "100%") {
                isFullHeight = true;
            } else {
                try {
                    double h = std::stod(heightVal);
                    isFullHeight = (h >= m_height);
                } catch (std::exception&) {
                    // 解析失败，忽略
                }
            }
            
            if (isFullWidth && isFullHeight) {
                // 这很可能是背景矩形
                setBackgroundColor(Color::fromString(fillAttr));
                // 跳过添加此元素，继续处理下一个
                rectElement = rectElement->NextSiblingElement();
            }
        }
    }
    
    // 解析所有子元素
    parseChildElements(svgElement);
    
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

// 解析SVG子元素
void SvgDocument::parseChildElements(tinyxml2::XMLElement* parentElement) {
    // 遍历所有子元素
    tinyxml2::XMLElement* element = parentElement->FirstChildElement();
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
            // 解析组元素，递归处理
            parseChildElements(element);
        }
        // 可以根据需要添加更多元素类型的解析
        
        // 移动到下一个元素
        element = element->NextSiblingElement();
    }
}

// 解析SVG线条元素
void SvgDocument::parseSvgLine(tinyxml2::XMLElement* element) {
    double x1 = 0, y1 = 0, x2 = 0, y2 = 0;
    
    element->QueryDoubleAttribute("x1", &x1);
    element->QueryDoubleAttribute("y1", &y1);
    element->QueryDoubleAttribute("x2", &x2);
    element->QueryDoubleAttribute("y2", &y2);
    
    SvgLine* line = new SvgLine({x1, y1}, {x2, y2});
    parseCommonAttributes(element, line);
    
    addElement(line);
}

// 解析SVG矩形元素
void SvgDocument::parseSvgRectangle(tinyxml2::XMLElement* element) {
    double x = 0, y = 0, width = 0, height = 0, rx = 0, ry = 0;
    
    element->QueryDoubleAttribute("x", &x);
    element->QueryDoubleAttribute("y", &y);
    element->QueryDoubleAttribute("width", &width);
    element->QueryDoubleAttribute("height", &height);
    element->QueryDoubleAttribute("rx", &rx);
    element->QueryDoubleAttribute("ry", &ry);
    
    SvgRectangle* rect = new SvgRectangle({x, y}, width, height, rx, ry);
    parseCommonAttributes(element, rect);
    
    addElement(rect);
}

// 解析SVG圆形元素
void SvgDocument::parseSvgCircle(tinyxml2::XMLElement* element) {
    double cx = 0, cy = 0, r = 0;
    
    element->QueryDoubleAttribute("cx", &cx);
    element->QueryDoubleAttribute("cy", &cy);
    element->QueryDoubleAttribute("r", &r);
    
    SvgCircle* circle = new SvgCircle({cx, cy}, r);
    parseCommonAttributes(element, circle);
    
    addElement(circle);
}

// 解析SVG椭圆元素
void SvgDocument::parseSvgEllipse(tinyxml2::XMLElement* element) {
    double cx = 0, cy = 0, rx = 0, ry = 0;
    
    element->QueryDoubleAttribute("cx", &cx);
    element->QueryDoubleAttribute("cy", &cy);
    element->QueryDoubleAttribute("rx", &rx);
    element->QueryDoubleAttribute("ry", &ry);
    
    SvgEllipse* ellipse = new SvgEllipse({cx, cy}, rx, ry);
    parseCommonAttributes(element, ellipse);
    
    addElement(ellipse);
}

// 解析SVG多边形元素
void SvgDocument::parseSvgPolygon(tinyxml2::XMLElement* element) {
    const char* pointsStr = element->Attribute("points");
    if (!pointsStr) {
        LoggingService::getInstance().warn("Polygon element missing 'points' attribute");
        return;
    }
    
    std::vector<Point> points;
    std::istringstream pointStream(pointsStr);
    std::string pointPair;
    
    // 分割成点对
    while (std::getline(pointStream, pointPair, ' ')) {
        if (pointPair.empty()) continue;
        
        size_t commaPos = pointPair.find(',');
        if (commaPos != std::string::npos) {
            try {
                double x = std::stod(pointPair.substr(0, commaPos));
                double y = std::stod(pointPair.substr(commaPos + 1));
                points.push_back({x, y});
            } catch (std::exception& e) {
                LoggingService::getInstance().warn("Failed to parse polygon point: " + std::string(e.what()));
            }
        }
    }
    
    if (points.size() < 3) {
        LoggingService::getInstance().warn("Polygon has fewer than 3 points, skipping");
        return;
    }
    
    SvgPolygon* polygon = new SvgPolygon(points);
    parseCommonAttributes(element, polygon);
    
    addElement(polygon);
}

// 解析SVG折线元素
void SvgDocument::parseSvgPolyline(tinyxml2::XMLElement* element) {
    const char* pointsStr = element->Attribute("points");
    if (!pointsStr) {
        LoggingService::getInstance().warn("Polyline element missing 'points' attribute");
        return;
    }
    
    std::vector<Point> points;
    std::istringstream pointStream(pointsStr);
    std::string pointPair;
    
    // 分割成点对
    while (std::getline(pointStream, pointPair, ' ')) {
        if (pointPair.empty()) continue;
        
        size_t commaPos = pointPair.find(',');
        if (commaPos != std::string::npos) {
            try {
                double x = std::stod(pointPair.substr(0, commaPos));
                double y = std::stod(pointPair.substr(commaPos + 1));
                points.push_back({x, y});
            } catch (std::exception& e) {
                LoggingService::getInstance().warn("Failed to parse polyline point: " + std::string(e.what()));
            }
        }
    }
    
    if (points.size() < 2) {
        LoggingService::getInstance().warn("Polyline has fewer than 2 points, skipping");
        return;
    }
    
    SvgPolyline* polyline = new SvgPolyline(points);
    parseCommonAttributes(element, polyline);
    
    addElement(polyline);
}

// 解析SVG文本元素
void SvgDocument::parseSvgText(tinyxml2::XMLElement* element) {
    double x = 0, y = 0;
    element->QueryDoubleAttribute("x", &x);
    element->QueryDoubleAttribute("y", &y);
    
    const char* textContent = element->GetText();
    std::string text = textContent ? textContent : "";
    
    SvgText* textElement = new SvgText({x, y}, text);
    
    // 解析字体相关属性
    const char* fontFamily = element->Attribute("font-family");
    if (fontFamily) {
        textElement->setFontFamily(fontFamily);
    }
    
    double fontSize = 12.0; // 默认字体大小
    if (element->QueryDoubleAttribute("font-size", &fontSize) == tinyxml2::XML_SUCCESS) {
        textElement->setFontSize(fontSize);
    }
    
    parseCommonAttributes(element, textElement);
    
    addElement(textElement);
}

// 解析SVG元素通用属性
void SvgDocument::parseCommonAttributes(tinyxml2::XMLElement* element, SvgElement* svgElement) {
    // ID属性
    const char* id = element->Attribute("id");
    if (id) {
        svgElement->setID(id);
    }
    
    // 填充颜色
    const char* fill = element->Attribute("fill");
    if (fill) {
        svgElement->setFillColor(Color::fromString(fill));
    }
    
    // 描边颜色
    const char* stroke = element->Attribute("stroke");
    if (stroke) {
        svgElement->setStrokeColor(Color::fromString(stroke));
    }
    
    // 描边宽度
    double strokeWidth = 1.0;
    if (element->QueryDoubleAttribute("stroke-width", &strokeWidth) == tinyxml2::XML_SUCCESS) {
        svgElement->setStrokeWidth(strokeWidth);
    }
    
    // 透明度
    double opacity = 1.0;
    if (element->QueryDoubleAttribute("opacity", &opacity) == tinyxml2::XML_SUCCESS) {
        svgElement->setOpacity(opacity);
    }
    
    // 变换
    const char* transform = element->Attribute("transform");
    if (transform) {
        Transform t;
        t.transform_str = transform;
        svgElement->setTransform(t);
    }
    
    // 处理其他自定义属性 (style, class等)
    const tinyxml2::XMLAttribute* attr = element->FirstAttribute();
    while (attr) {
        std::string name = attr->Name();
        std::string value = attr->Value();
        
        // 跳过已经处理过的属性
        if (name != "id" && name != "fill" && name != "stroke" && 
            name != "stroke-width" && name != "opacity" && name != "transform" &&
            name != "x" && name != "y" && name != "width" && name != "height" &&
            name != "x1" && name != "y1" && name != "x2" && name != "y2" &&
            name != "cx" && name != "cy" && name != "r" && name != "rx" && name != "ry" &&
            name != "points" && name != "font-family" && name != "font-size") {
            
            // 尝试将值解析为数字
            try {
                double numValue = std::stod(value);
                svgElement->setAttribute(name, numValue);
            } catch (std::exception&) {
                // 如果不是数字就存为字符串
                svgElement->setAttribute(name, value);
            }
        }
        
        attr = attr->Next();
    }
}