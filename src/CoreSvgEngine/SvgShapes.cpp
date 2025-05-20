#include "SvgShapes.h"
#include <sstream>

// SvgLine
std::string SvgLine::toSvgString() const {
    std::stringstream ss;
    ss << "<line x1=\"" << m_p1.x << "\" y1=\"" << m_p1.y
       << "\" x2=\"" << m_p2.x << "\" y2=\"" << m_p2.y << "\""
       << getCommonAttributesString() << " />";
    return ss.str();
}

// SvgRectangle
std::string SvgRectangle::toSvgString() const {
    std::stringstream ss;
    ss << "<rect x=\"" << m_topLeft.x << "\" y=\"" << m_topLeft.y
       << "\" width=\"" << m_width << "\" height=\"" << m_height << "\"";
    if (m_rx > 0) ss << " rx=\"" << m_rx << "\"";
    if (m_ry > 0) ss << " ry=\"" << m_ry << "\"";
    ss << getCommonAttributesString() << " />";
    return ss.str();
}

// SvgCircle    
std::string SvgCircle::toSvgString() const {
    std::stringstream ss;
    ss << "<circle cx=\"" << m_center.x << "\" cy=\"" << m_center.y
       << "\" r=\"" << m_radius << "\""
       << getCommonAttributesString() << " />";
    return ss.str();
}

// SvgEllipse    
std::string SvgEllipse::toSvgString() const {
    std::stringstream ss;
    ss << "<ellipse cx=\"" << m_center.x << "\" cy=\"" << m_center.y
       << "\" rx=\"" << m_rx << "\" ry=\"" << m_ry << "\""
       << getCommonAttributesString() << " />";
    return ss.str();
}

// SvgPolygon    
std::string SvgPolygon::toSvgString() const {
    std::stringstream ss;
    ss << "<polygon points=\"";
    for (size_t i = 0; i < m_points.size(); ++i) {
        ss << m_points[i].x << "," << m_points[i].y << (i == m_points.size() - 1 ? "" : " ");
    }
    ss << "\"" << getCommonAttributesString() << " />";
    return ss.str();
}


// SvgPolyline    
std::string SvgPolyline::toSvgString() const {
    std::stringstream ss;
    // Polyline 通常没有填充，只有边框
    std::string commonAttrs = getCommonAttributesString();
    // 查找并替换 fill 属性为 "none"
    size_t fillPos = commonAttrs.find(" fill=");
    std::string fillAttr = " fill=\"none\"";
    if (fillPos != std::string::npos) {
        size_t fillEndPos = commonAttrs.find("\"", fillPos + strlen(" fill=\""));
        if (fillEndPos != std::string::npos) {
            commonAttrs.replace(fillPos, fillEndPos - fillPos + 1, fillAttr);
        }
    } else {
        commonAttrs += fillAttr; // 如果原来没有fill属性，则添加 fill="none"
    }

    ss << "<polyline points=\"";
    for (size_t i = 0; i < m_points.size(); ++i) {
        ss << m_points[i].x << "," << m_points[i].y << (i == m_points.size() - 1 ? "" : " ");
    }
    ss << "\"" << commonAttrs << " />";
    return ss.str();
}

// SvgPentagon    
SvgPentagon::SvgPentagon(Point center, double radius) {
    m_points.resize(5);
    for (int i = 0; i < 5; ++i) {
        // 起始角度调整为使一个顶点朝上或一个边水平，这里假设顶点朝上
        double angle_deg = 90 + i * (360.0 / 5.0); // 90度是顶部开始
        double angle_rad = angle_deg * M_PI / 180.0;
        m_points[i] = {center.x + radius * std::cos(angle_rad), center.y - radius * std::sin(angle_rad)}; // y轴向下为正，sin取负
    }
}

// SvgHexagon    
SvgHexagon::SvgHexagon(Point center, double radius) {
    m_points.resize(6);
    for (int i = 0; i < 6; ++i) {
        // 起始角度调整为使一个顶点朝上或一个边水平，这里假设顶点朝右
        double angle_deg = 0 + i * (360.0 / 6.0); // 0度是右侧开始
        double angle_rad = angle_deg * M_PI / 180.0;
        m_points[i] = {center.x + radius * std::cos(angle_rad), center.y - radius * std::sin(angle_rad)};
    }
}

// SvgStar    
SvgStar::SvgStar(Point center, double outerRadius, double innerRadius, int numPoints, double startAngleDeg) {
    if (numPoints < 2) return; // 至少需要两个顶点才能形成星形
    m_points.resize(numPoints * 2);
    double angleStep = M_PI / numPoints; // 每次前进的角度是 PI/numPoints
    double currentAngle = startAngleDeg * M_PI / 180.0; // 转换为弧度

    for (int i = 0; i < numPoints * 2; ++i) {
        double r = (i % 2 == 0) ? outerRadius : innerRadius; // 交替使用内外半径
        m_points[i] = {center.x + r * std::cos(currentAngle), center.y + r * std::sin(currentAngle)}; // SVG y轴向下，但通常几何计算y向上为正，sin结果可能需要反转，取决于坐标系
        currentAngle += angleStep;
    }
}