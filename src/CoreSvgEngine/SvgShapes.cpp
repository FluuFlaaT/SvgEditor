#include "svgshapes.h"
#include <sstream>
// #include "../LoggingService/LoggingService.h"
#include <QLoggingCategory>
Q_DECLARE_LOGGING_CATEGORY(svgShapesLog)
Q_LOGGING_CATEGORY(svgShapesLog, "SvgShapes")

// SvgLine
SvgLine::SvgLine(Point start, Point end) : m_p1(start), m_p2(end) {
    qCInfo(svgShapesLog) << "Creating Line element: (" << start.x << "," << start.y
                          << ") to (" << end.x << "," << end.y << ")";
}

void SvgLine::setP1(const Point& p) {
    qCInfo(svgShapesLog) << "Setting Line start point: (" << m_p1.x << "," << m_p1.y
                          << ") to (" << p.x << "," << p.y << ")";
    m_p1 = p;
}

void SvgLine::setP2(const Point& p) {
    qCInfo(svgShapesLog) << "Setting Line end point: (" << m_p2.x << "," << m_p2.y
                          << ") to (" << p.x << "," << p.y << ")";
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
    qCInfo(svgShapesLog) << "Creating Rectangle element: (" << tl.x << "," << tl.y
                          << ") width=" << m_width << ", height=" << m_height
                          << ", rx=" << m_rx << ", ry=" << m_ry;
}

void SvgRectangle::setTopLeft(const Point& p) {
    qCInfo(svgShapesLog) << "Setting Rectangle top-left: (" << m_topLeft.x << "," << m_topLeft.y
                          << ") to (" << p.x << "," << p.y << ")";
    m_topLeft = p;
}

void SvgRectangle::setWidth(double w) {
    double newWidth = (w > 0 ? w : 0);
    qCInfo(svgShapesLog) << "Setting Rectangle width:" << m_width << "to" << newWidth;
    m_width = newWidth;
}

void SvgRectangle::setHeight(double h) {
    double newHeight = (h > 0 ? h : 0);
    qCInfo(svgShapesLog) << "Setting Rectangle height:" << m_height << "to" << newHeight;
    m_height = newHeight;
}

void SvgRectangle::setRx(double rx_val) {
    double newRx = (rx_val > 0 ? rx_val : 0);
    qCInfo(svgShapesLog) << "Setting Rectangle rx (corner radius x):" << m_rx << "to" << newRx;
    m_rx = newRx;
}

void SvgRectangle::setRy(double ry_val) {
    double newRy = (ry_val > 0 ? ry_val : 0);
    qCInfo(svgShapesLog) << "Setting Rectangle ry (corner radius y):" << m_ry << "to" << newRy;
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
    qCInfo(svgShapesLog) << "Creating Circle element: center=(" << c.x << "," << c.y
                          << "), radius=" << m_radius;
}

void SvgCircle::setCenter(const Point& c) {
    qCInfo(svgShapesLog) << "Setting Circle center: (" << m_center.x << "," << m_center.y
                          << ") to (" << c.x << "," << c.y << ")";
    m_center = c;
}

void SvgCircle::setRadius(double r) {
    double newRadius = (r > 0 ? r : 0);
    qCInfo(svgShapesLog) << "Setting Circle radius:" << m_radius << "to" << newRadius;
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
    qCInfo(svgShapesLog) << "Creating Ellipse element: center=(" << c.x << "," << c.y
                          << "), rx=" << m_rx << ", ry=" << m_ry;
}

void SvgEllipse::setCenter(const Point& c) {
    qCInfo(svgShapesLog) << "Setting Ellipse center: (" << m_center.x << "," << m_center.y
                          << ") to (" << c.x << "," << c.y << ")";
    m_center = c;
}

void SvgEllipse::setRx(double r_x) {
    double newRx = (r_x > 0 ? r_x : 0);
    qCInfo(svgShapesLog) << "Setting Ellipse x-radius:" << m_rx << "to" << newRx;
    m_rx = newRx;
}

void SvgEllipse::setRy(double r_y) {
    double newRy = (r_y > 0 ? r_y : 0);
    qCInfo(svgShapesLog) << "Setting Ellipse y-radius:" << m_ry << "to" << newRy;
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
    qCInfo(svgShapesLog) << "Creating Polygon element with" << pts.size() << "points";
}

void SvgPolygon::setPoints(const std::vector<Point>& pts) {
    qCInfo(svgShapesLog) << "Updating Polygon points: from" << m_points.size()
                          << "points to" << pts.size() << "points";
    m_points = pts;
}

void SvgPolygon::addPoint(const Point& p) {
    qCInfo(svgShapesLog) << "Adding point (" << p.x << "," << p.y << ") to Polygon";
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
    qCInfo(svgShapesLog) << "Creating Polyline element with" << pts.size() << "points";
}

void SvgPolyline::setPoints(const std::vector<Point>& pts) {
    qCInfo(svgShapesLog) << "Updating Polyline points: from" << m_points.size()
                          << "points to" << pts.size() << "points";
    m_points = pts;
}

void SvgPolyline::addPoint(const Point& p) {
    qCInfo(svgShapesLog) << "Adding point (" << p.x << "," << p.y << ") to Polyline";
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
    qCInfo(svgShapesLog) << "Creating Pentagon element: center=(" << center.x << "," << center.y
                          << "), radius=" << radius;
    
    m_points.resize(5);
    for (int i = 0; i < 5; ++i) {
        // Start from top vertex (-90°) for consistent orientation
        double angle_deg = -90 + i * (360.0 / 5.0);
        double angle_rad = angle_deg * M_PI / 180.0;
        // Y-axis is inverted in SVG coordinate system
        m_points[i] = {center.x + radius * std::cos(angle_rad), center.y + radius * std::sin(angle_rad)};
    }
}

// SvgHexagon    
SvgHexagon::SvgHexagon(Point center, double radius) {
    qCInfo(svgShapesLog) << "Creating Hexagon element: center=(" << center.x << "," << center.y
                          << "), radius=" << radius;
        
    m_points.resize(6);
    for (int i = 0; i < 6; ++i) {
        // Start from right vertex (0°) for flat-topped hexagon
        double angle_deg = 0 + i * (360.0 / 6.0);
        double angle_rad = angle_deg * M_PI / 180.0;
        m_points[i] = {center.x + radius * std::cos(angle_rad), center.y + radius * std::sin(angle_rad)};
    }
}

// SvgStar    
SvgStar::SvgStar(Point center, double outerRadius, double innerRadius, int numPoints, double startAngleDeg) {
    qCInfo(svgShapesLog) << "Creating Star element: center=(" << center.x << "," << center.y
                          << "), outerRadius=" << outerRadius
                          << ", innerRadius=" << innerRadius
                          << ", points=" << numPoints;
        
    // Minimum 2 points required for star geometry
    if (numPoints < 2) return;
    m_points.resize(numPoints * 2);
    // Each star point consists of outer and inner vertices
    double angleStep = M_PI / numPoints;
    double currentAngle = startAngleDeg * M_PI / 180.0;

    for (int i = 0; i < numPoints * 2; ++i) {
        // Alternate between outer and inner radius for star shape
        double r = (i % 2 == 0) ? outerRadius : innerRadius;
        m_points[i] = {center.x + r * std::cos(currentAngle), center.y + r * std::sin(currentAngle)};
        currentAngle += angleStep;
    }
}