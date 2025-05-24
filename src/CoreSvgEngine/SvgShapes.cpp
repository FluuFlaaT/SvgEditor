#include "svgshapes.h"
#include <sstream>
// #include "../LoggingService/LoggingService.h"
#include <QLoggingCategory>
Q_DECLARE_LOGGING_CATEGORY(svgShapesLog)
Q_LOGGING_CATEGORY(svgShapesLog, "SvgShapes")

// SvgLine
SvgLine::SvgLine(Point start, Point end) : m_p1(start), m_p2(end) {
    qCInfo(svgShapesLog) << "Creating Line element: (" + QString::fromStdString(std::to_string(start.x)) + "," + QString::fromStdString(std::to_string(start.y)) + ") to (" + QString::fromStdString(std::to_string(end.x)) + "," + QString::fromStdString(std::to_string(end.y)) + ")";
}

void SvgLine::setP1(const Point& p) {
    qCInfo(svgShapesLog) << "Setting Line start point: (" + QString::fromStdString(std::to_string(m_p1.x)) + "," + QString::fromStdString(std::to_string(m_p1.y)) + ") to (" + QString::fromStdString(std::to_string(p.x)) + "," + QString::fromStdString(std::to_string(p.y)) + ")";
    m_p1 = p;
}

void SvgLine::setP2(const Point& p) {
    qCInfo(svgShapesLog) << "Setting Line end point: (" + QString::fromStdString(std::to_string(m_p2.x)) + "," + QString::fromStdString(std::to_string(m_p2.y)) + ") to (" + QString::fromStdString(std::to_string(p.x)) + "," + QString::fromStdString(std::to_string(p.y)) + ")";
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
    qCInfo(svgShapesLog) << "Creating Rectangle element: (" + QString::fromStdString(std::to_string(tl.x)) + "," + QString::fromStdString(std::to_string(tl.y)) + ") width=" + QString::fromStdString(std::to_string(m_width)) + ", height=" + QString::fromStdString(std::to_string(m_height)) + ", rx=" + QString::fromStdString(std::to_string(m_rx)) + ", ry=" + QString::fromStdString(std::to_string(m_ry));
}

void SvgRectangle::setTopLeft(const Point& p) {
    qCInfo(svgShapesLog) << "Setting Rectangle top-left: (" + QString::fromStdString(std::to_string(m_topLeft.x)) + "," + QString::fromStdString(std::to_string(m_topLeft.y)) + ") to (" + QString::fromStdString(std::to_string(p.x)) + "," + QString::fromStdString(std::to_string(p.y)) + ")";
    m_topLeft = p;
}

void SvgRectangle::setWidth(double w) {
    double newWidth = (w > 0 ? w : 0);
    qCInfo(svgShapesLog) << "Setting Rectangle width: " + QString::fromStdString(std::to_string(m_width)) + " to " + QString::fromStdString(std::to_string(newWidth));
    m_width = newWidth;
}

void SvgRectangle::setHeight(double h) {
    double newHeight = (h > 0 ? h : 0);
    qCInfo(svgShapesLog) << "Setting Rectangle height: " + QString::fromStdString(std::to_string(m_height)) + " to " + QString::fromStdString(std::to_string(newHeight));
    m_height = newHeight;
}

void SvgRectangle::setRx(double rx_val) {
    double newRx = (rx_val > 0 ? rx_val : 0);
    qCInfo(svgShapesLog) << "Setting Rectangle rx (corner radius x): " + QString::fromStdString(std::to_string(m_rx)) + " to " + QString::fromStdString(std::to_string(newRx));
    m_rx = newRx;
}

void SvgRectangle::setRy(double ry_val) {
    double newRy = (ry_val > 0 ? ry_val : 0);
    qCInfo(svgShapesLog) << "Setting Rectangle ry (corner radius y): " + QString::fromStdString(std::to_string(m_ry)) + " to " + QString::fromStdString(std::to_string(newRy));
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
    qCInfo(svgShapesLog) << "Creating Circle element: center=(" + QString::fromStdString(std::to_string(c.x)) + "," + QString::fromStdString(std::to_string(c.y)) + "), radius=" + QString::fromStdString(std::to_string(m_radius));
}

void SvgCircle::setCenter(const Point& c) {
    qCInfo(svgShapesLog) << "Setting Circle center: (" + QString::fromStdString(std::to_string(m_center.x)) + "," + QString::fromStdString(std::to_string(m_center.y)) + ") to (" + QString::fromStdString(std::to_string(c.x)) + "," + QString::fromStdString(std::to_string(c.y)) + ")";
    m_center = c;
}

void SvgCircle::setRadius(double r) {
    double newRadius = (r > 0 ? r : 0);
    qCInfo(svgShapesLog) << "Setting Circle radius: " + QString::fromStdString(std::to_string(m_radius)) + " to " + QString::fromStdString(std::to_string(newRadius));
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
    qCInfo(svgShapesLog) << "Creating Ellipse element: center=(" + QString::fromStdString(std::to_string(c.x)) + "," + QString::fromStdString(std::to_string(c.y)) + "), rx=" + QString::fromStdString(std::to_string(m_rx)) + ", ry=" + QString::fromStdString(std::to_string(m_ry));
}

void SvgEllipse::setCenter(const Point& c) {
    qCInfo(svgShapesLog) << "Setting Ellipse center: (" + QString::fromStdString(std::to_string(m_center.x)) + "," + QString::fromStdString(std::to_string(m_center.y)) + ") to (" + QString::fromStdString(std::to_string(c.x)) + "," + QString::fromStdString(std::to_string(c.y)) + ")";
    m_center = c;
}

void SvgEllipse::setRx(double r_x) {
    double newRx = (r_x > 0 ? r_x : 0);
    qCInfo(svgShapesLog) << "Setting Ellipse x-radius: " + QString::fromStdString(std::to_string(m_rx)) + " to " + QString::fromStdString(std::to_string(newRx));
    m_rx = newRx;
}

void SvgEllipse::setRy(double r_y) {
    double newRy = (r_y > 0 ? r_y : 0);
    qCInfo(svgShapesLog) << "Setting Ellipse y-radius: " + QString::fromStdString(std::to_string(m_ry)) + " to " + QString::fromStdString(std::to_string(newRy));
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
    qCInfo(svgShapesLog) << "Creating Polygon element with " + QString::fromStdString(std::to_string(pts.size())) + " points";
}

void SvgPolygon::setPoints(const std::vector<Point>& pts) {
    qCInfo(svgShapesLog) << "Updating Polygon points: from " + QString::fromStdString(std::to_string(m_points.size())) + " points to " + QString::fromStdString(std::to_string(pts.size())) + " points";
    m_points = pts;
}

void SvgPolygon::addPoint(const Point& p) {
    qCInfo(svgShapesLog) << "Adding point (" + QString::fromStdString(std::to_string(p.x)) + "," + QString::fromStdString(std::to_string(p.y)) + ") to Polygon";
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
    qCInfo(svgShapesLog) << "Creating Polyline element with " + QString::fromStdString(std::to_string(pts.size())) + " points";
}

void SvgPolyline::setPoints(const std::vector<Point>& pts) {
    qCInfo(svgShapesLog) << "Updating Polyline points: from " + QString::fromStdString(std::to_string(m_points.size())) + " points to " + QString::fromStdString(std::to_string(pts.size())) + " points";
    m_points = pts;
}

void SvgPolyline::addPoint(const Point& p) {
    qCInfo(svgShapesLog) << "Adding point (" + QString::fromStdString(std::to_string(p.x)) + "," + QString::fromStdString(std::to_string(p.y)) + ") to Polyline";
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
    qCInfo(svgShapesLog) << "Creating Pentagon element: center=(" + QString::fromStdString(std::to_string(center.x)) + "," + QString::fromStdString(std::to_string(center.y)) + "), radius=" + QString::fromStdString(std::to_string(radius));
    
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
    qCInfo(svgShapesLog) << "Creating Hexagon element: center=(" + QString::fromStdString(std::to_string(center.x)) + "," + QString::fromStdString(std::to_string(center.y)) + "), radius=" + QString::fromStdString(std::to_string(radius));
        
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
    qCInfo(svgShapesLog) << "Creating Star element: center=(" + QString::fromStdString(std::to_string(center.x)) + "," + QString::fromStdString(std::to_string(center.y)) + 
        "), outerRadius=" + QString::fromStdString(std::to_string(outerRadius)) + 
        ", innerRadius=" + QString::fromStdString(std::to_string(innerRadius)) + 
        ", points=" + QString::fromStdString(std::to_string(numPoints));
        
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