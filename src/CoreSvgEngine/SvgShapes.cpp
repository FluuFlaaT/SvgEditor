#include "SvgShapes.h"
#include <sstream>
#include "../LoggingService/LoggingService.h"

// SvgLine
SvgLine::SvgLine(Point start, Point end) : m_p1(start), m_p2(end) {
    LoggingService::getInstance().info("Creating Line element: (" + 
        std::to_string(start.x) + "," + std::to_string(start.y) + ") to (" + 
        std::to_string(end.x) + "," + std::to_string(end.y) + ")");
}

void SvgLine::setP1(const Point& p) {
    LoggingService::getInstance().info("Setting Line start point: (" + 
        std::to_string(m_p1.x) + "," + std::to_string(m_p1.y) + ") to (" + 
        std::to_string(p.x) + "," + std::to_string(p.y) + ")");
    m_p1 = p;
}

void SvgLine::setP2(const Point& p) {
    LoggingService::getInstance().info("Setting Line end point: (" + 
        std::to_string(m_p2.x) + "," + std::to_string(m_p2.y) + ") to (" + 
        std::to_string(p.x) + "," + std::to_string(p.y) + ")");
    m_p2 = p;
}

std::string SvgLine::toSvgString() const {
    std::stringstream ss;
    ss << "<line x1=\"" << m_p1.x << "\" y1=\"" << m_p1.y
       << "\" x2=\"" << m_p2.x << "\" y2=\"" << m_p2.y << "\""
       << getCommonAttributesString() << " />";
    return ss.str();
}

// SvgRectangle
SvgRectangle::SvgRectangle(Point tl, double w, double h, double rx_, double ry_)
    : m_topLeft(tl), m_width(w > 0 ? w : 0), m_height(h > 0 ? h : 0), 
      m_rx(rx_ > 0 ? rx_ : 0), m_ry(ry_ > 0 ? ry_ : 0) {
    LoggingService::getInstance().info("Creating Rectangle element: (" + 
        std::to_string(tl.x) + "," + std::to_string(tl.y) + ") width=" + 
        std::to_string(m_width) + ", height=" + std::to_string(m_height) + 
        ", rx=" + std::to_string(m_rx) + ", ry=" + std::to_string(m_ry));
}

void SvgRectangle::setTopLeft(const Point& p) {
    LoggingService::getInstance().info("Setting Rectangle top-left: (" + 
        std::to_string(m_topLeft.x) + "," + std::to_string(m_topLeft.y) + ") to (" + 
        std::to_string(p.x) + "," + std::to_string(p.y) + ")");
    m_topLeft = p;
}

void SvgRectangle::setWidth(double w) {
    double newWidth = (w > 0 ? w : 0);
    LoggingService::getInstance().info("Setting Rectangle width: " + 
        std::to_string(m_width) + " to " + std::to_string(newWidth));
    m_width = newWidth;
}

void SvgRectangle::setHeight(double h) {
    double newHeight = (h > 0 ? h : 0);
    LoggingService::getInstance().info("Setting Rectangle height: " + 
        std::to_string(m_height) + " to " + std::to_string(newHeight));
    m_height = newHeight;
}

void SvgRectangle::setRx(double rx_val) {
    double newRx = (rx_val > 0 ? rx_val : 0);
    LoggingService::getInstance().info("Setting Rectangle rx (corner radius x): " + 
        std::to_string(m_rx) + " to " + std::to_string(newRx));
    m_rx = newRx;
}

void SvgRectangle::setRy(double ry_val) {
    double newRy = (ry_val > 0 ? ry_val : 0);
    LoggingService::getInstance().info("Setting Rectangle ry (corner radius y): " + 
        std::to_string(m_ry) + " to " + std::to_string(newRy));
    m_ry = newRy;
}

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
SvgCircle::SvgCircle(Point c, double r) : m_center(c), m_radius(r > 0 ? r : 0) {
    LoggingService::getInstance().info("Creating Circle element: center=(" + 
        std::to_string(c.x) + "," + std::to_string(c.y) + "), radius=" + 
        std::to_string(m_radius));
}

void SvgCircle::setCenter(const Point& c) {
    LoggingService::getInstance().info("Setting Circle center: (" + 
        std::to_string(m_center.x) + "," + std::to_string(m_center.y) + ") to (" + 
        std::to_string(c.x) + "," + std::to_string(c.y) + ")");
    m_center = c;
}

void SvgCircle::setRadius(double r) {
    double newRadius = (r > 0 ? r : 0);
    LoggingService::getInstance().info("Setting Circle radius: " + 
        std::to_string(m_radius) + " to " + std::to_string(newRadius));
    m_radius = newRadius;
}

std::string SvgCircle::toSvgString() const {
    std::stringstream ss;
    ss << "<circle cx=\"" << m_center.x << "\" cy=\"" << m_center.y
       << "\" r=\"" << m_radius << "\""
       << getCommonAttributesString() << " />";
    return ss.str();
}

// SvgEllipse    
SvgEllipse::SvgEllipse(Point c, double r_x, double r_y) 
    : m_center(c), m_rx(r_x > 0 ? r_x : 0), m_ry(r_y > 0 ? r_y : 0) {
    LoggingService::getInstance().info("Creating Ellipse element: center=(" + 
        std::to_string(c.x) + "," + std::to_string(c.y) + "), rx=" + 
        std::to_string(m_rx) + ", ry=" + std::to_string(m_ry));
}

void SvgEllipse::setCenter(const Point& c) {
    LoggingService::getInstance().info("Setting Ellipse center: (" + 
        std::to_string(m_center.x) + "," + std::to_string(m_center.y) + ") to (" + 
        std::to_string(c.x) + "," + std::to_string(c.y) + ")");
    m_center = c;
}

void SvgEllipse::setRx(double r_x) {
    double newRx = (r_x > 0 ? r_x : 0);
    LoggingService::getInstance().info("Setting Ellipse x-radius: " + 
        std::to_string(m_rx) + " to " + std::to_string(newRx));
    m_rx = newRx;
}

void SvgEllipse::setRy(double r_y) {
    double newRy = (r_y > 0 ? r_y : 0);
    LoggingService::getInstance().info("Setting Ellipse y-radius: " + 
        std::to_string(m_ry) + " to " + std::to_string(newRy));
    m_ry = newRy;
}

std::string SvgEllipse::toSvgString() const {
    std::stringstream ss;
    ss << "<ellipse cx=\"" << m_center.x << "\" cy=\"" << m_center.y
       << "\" rx=\"" << m_rx << "\" ry=\"" << m_ry << "\""
       << getCommonAttributesString() << " />";
    return ss.str();
}

// SvgPolygon    
SvgPolygon::SvgPolygon(const std::vector<Point>& pts) : m_points(pts) {
    LoggingService::getInstance().info("Creating Polygon element with " + 
        std::to_string(pts.size()) + " points");
}

void SvgPolygon::setPoints(const std::vector<Point>& pts) {
    LoggingService::getInstance().info("Updating Polygon points: from " + 
        std::to_string(m_points.size()) + " points to " + std::to_string(pts.size()) + " points");
    m_points = pts;
}

void SvgPolygon::addPoint(const Point& p) {
    LoggingService::getInstance().info("Adding point (" + 
        std::to_string(p.x) + "," + std::to_string(p.y) + ") to Polygon");
    m_points.push_back(p);
}

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
SvgPolyline::SvgPolyline(const std::vector<Point>& pts) : m_points(pts) {
    LoggingService::getInstance().info("Creating Polyline element with " + 
        std::to_string(pts.size()) + " points");
}

void SvgPolyline::setPoints(const std::vector<Point>& pts) {
    LoggingService::getInstance().info("Updating Polyline points: from " + 
        std::to_string(m_points.size()) + " points to " + std::to_string(pts.size()) + " points");
    m_points = pts;
}

void SvgPolyline::addPoint(const Point& p) {
    LoggingService::getInstance().info("Adding point (" + 
        std::to_string(p.x) + "," + std::to_string(p.y) + ") to Polyline");
    m_points.push_back(p);
}

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
    LoggingService::getInstance().info("Creating Pentagon element: center=(" + 
        std::to_string(center.x) + "," + std::to_string(center.y) + "), radius=" + 
        std::to_string(radius));
    
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
    LoggingService::getInstance().info("Creating Hexagon element: center=(" + 
        std::to_string(center.x) + "," + std::to_string(center.y) + "), radius=" + 
        std::to_string(radius));
        
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
    LoggingService::getInstance().info("Creating Star element: center=(" + 
        std::to_string(center.x) + "," + std::to_string(center.y) + 
        "), outerRadius=" + std::to_string(outerRadius) + 
        ", innerRadius=" + std::to_string(innerRadius) + 
        ", points=" + std::to_string(numPoints));
        
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