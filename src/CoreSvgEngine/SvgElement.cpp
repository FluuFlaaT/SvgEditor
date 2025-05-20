#include <string>
#include <sstream>
#include "SvgElement.h"

std::string SvgElement::getCommonAttributesString() const {
    std::stringstream ss;
    if (!getID().empty()) {
        ss << " id=\"" << getID() << "\"";
    }
    ss << " stroke=\"" << getStrokeColor().toString() << "\"";
    ss << " stroke-width=\"" << getStrokeWidth() << "\"";
    ss << " fill=\"" << (getFillColor().alpha == 0 ? "none" : getFillColor().toString()) << "\""; // "none" 表示透明填充
    if (getOpacity() < 1.0) { // opacity 默认是1，小于1才需要写出
        ss << " opacity=\"" << getOpacity() << "\"";
    }
    if (!getTransform().transform_str.empty()) {
        ss << " transform=\"" << getTransform().transform_str << "\"";
    }
    // 可以添加其他通用属性
    for (const auto& attr : m_attributes) {
        ss << " " << attr.first << "=\"";
        std::visit([&ss](const auto& value){ ss << value; }, attr.second);
        ss << "\"";
    }
    return ss.str();
}

// 默认实现为空，子类需要重写这个方法
void SvgElement::draw() const {
}

// ---------- Getter & Setter ----------

std::string SvgElement::getID() const {
    return m_id;
}

void SvgElement::setID(const std::string& id) {
    m_id = id;
}

Color SvgElement::getStrokeColor() const {
    return m_strokeColor;
}

void SvgElement::setStrokeColor(const Color& color) {
    m_strokeColor = color;
}

double SvgElement::getStrokeWidth() const {
    return m_strokeWidth;
}

void SvgElement::setStrokeWidth(double width) {
    m_strokeWidth = (width < 0) ? 0 : width;
}

Color SvgElement::getFillColor() const {
    return m_fillColor;
}

void SvgElement::setFillColor(const Color& color) {
    m_fillColor = color;
}

Transform SvgElement::getTransform() const {
    return m_transform;
}

void SvgElement::setTransform(const Transform& transform) {
    m_transform = transform;
}

double SvgElement::getOpacity() const {
    return m_opacity;
}

void SvgElement::setOpacity(double opacity) {
    m_opacity = (opacity < 0.0) ? 0.0 : (opacity > 1.0 ? 1.0 : opacity);
}