#include <string>
#include <sstream>
#include "svgelement.h"
// #include "../LoggingService/LoggingService.h"
#include <QLoggingCategory>
#include <QString>
Q_DECLARE_LOGGING_CATEGORY(svgElementLog)
Q_LOGGING_CATEGORY(svgElementLog, "SvgElement")

std::string SvgElement::getCommonAttributesString() const {
    std::stringstream ss;
    if (!getID().empty()) {
        ss << " id=\"" << getID() << "\"";
    }
    ss << " stroke=\"" << getStrokeColor().toString() << "\"";
    ss << " stroke-width=\"" << getStrokeWidth() << "\"";
    // SVG specification requires "none" for transparent fill
    ss << " fill=\"" << (getFillColor().alpha == 0 ? "none" : getFillColor().toString()) << "\"";
    // Only output opacity when different from default to minimize SVG size
    if (getOpacity() < 1.0) {
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
    qCInfo(svgElementLog) << "Setting element ID: '" + QString::fromStdString(m_id) + "' to '" + QString::fromStdString(id) + "'";
    m_id = id;
}

Color SvgElement::getStrokeColor() const {
    return m_strokeColor;
}

void SvgElement::setStrokeColor(const Color& color) {
    qCInfo(svgElementLog) << "Setting element stroke color: " + QString::fromStdString(m_strokeColor.toString()) + " to " + QString::fromStdString(color.toString());
    m_strokeColor = color;
}

double SvgElement::getStrokeWidth() const {
    return m_strokeWidth;
}

void SvgElement::setStrokeWidth(double width) {
    qCInfo(svgElementLog) << "Setting element stroke width: " + QString::fromStdString(std::to_string(m_strokeWidth)) + " to " + QString::fromStdString(std::to_string((width < 0) ? 0 : width));
    // Negative stroke width is invalid in SVG specification
    m_strokeWidth = (width < 0) ? 0 : width;
}

Color SvgElement::getFillColor() const {
    return m_fillColor;
}

void SvgElement::setFillColor(const Color& color) {
    qCInfo(svgElementLog) << "Setting element fill color: " + QString::fromStdString(m_fillColor.toString()) + " to " + QString::fromStdString(color.toString());
    m_fillColor = color;
}

Transform SvgElement::getTransform() const {
    return m_transform;
}

void SvgElement::setTransform(const Transform& transform) {
    qCInfo(svgElementLog) << "Setting element transform: " + 
        (m_transform.transform_str.empty() ? "none" : QString::fromStdString(m_transform.transform_str)) + 
        " to " + 
        (transform.transform_str.empty() ? "none" : QString::fromStdString(transform.transform_str));
    m_transform = transform;
}

double SvgElement::getOpacity() const {
    return m_opacity;
}

void SvgElement::setOpacity(double opacity) {
    // SVG opacity must be clamped to [0.0, 1.0] range
    double newOpacity = (opacity < 0.0) ? 0.0 : (opacity > 1.0 ? 1.0 : opacity);
    qCInfo(svgElementLog) << "Setting element opacity: " + QString::fromStdString(std::to_string(m_opacity)) + " to " + QString::fromStdString(std::to_string(newOpacity));
    m_opacity = newOpacity;
}