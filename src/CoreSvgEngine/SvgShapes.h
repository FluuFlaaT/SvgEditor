#pragma once
#define _USE_MATH_DEFINES // 配合cmath引入圆周率
#include <cmath>
#include <vector>
#include "SvgElement.h"

class SvgLine : public SvgElement {
private: 
    Point m_p1; 
    Point m_p2;

public: 
    SvgLine(Point start = {0,0}, Point end = {0,0}) : m_p1(start), m_p2(end) {}
    
    SvgElementType getType() const override { return SvgElementType::Line; }
    std::string toSvgString() const override;
    
    // ---------- Getter & Setter ----------

    Point getP1() const { return m_p1; } 
    void setP1(const Point& p) { m_p1 = p; }
    Point getP2() const { return m_p2; } 
    void setP2(const Point& p) { m_p2 = p; }
};

class SvgRectangle : public SvgElement {
private: 
    Point m_topLeft; 
    double m_width; 
    double m_height; 
    double m_rx; 
    double m_ry; // 圆角半径

public: 
    SvgRectangle(Point tl = {0,0}, double w = 0, double h = 0, double rx_ = 0.0, double ry_ = 0.0)
        : m_topLeft(tl), m_width(w > 0 ? w : 0), m_height(h > 0 ? h : 0), 
          m_rx(rx_ > 0 ? rx_ : 0), m_ry(ry_ > 0 ? ry_ : 0) {}
    
    SvgElementType getType() const override { return SvgElementType::Rectangle; }
    std::string toSvgString() const override;
    
    // ---------- Getter & Setter ----------

    Point getTopLeft() const { return m_topLeft; } 
    void setTopLeft(const Point& p) { m_topLeft = p; }
    
    double getWidth() const { return m_width; } 
    void setWidth(double w) { m_width = (w > 0 ? w : 0); }
    
    double getHeight() const { return m_height; } 
    void setHeight(double h) { m_height = (h > 0 ? h : 0); }
    
    double getRx() const { return m_rx; } 
    void setRx(double rx_val) { m_rx = (rx_val > 0 ? rx_val : 0); }
    
    double getRy() const { return m_ry; } 
    void setRy(double ry_val) { m_ry = (ry_val > 0 ? ry_val : 0); }
};

class SvgCircle : public SvgElement {
private: 
    Point m_center; 
    double m_radius;

public: 
    SvgCircle(Point c = {0,0}, double r = 0) : m_center(c), m_radius(r > 0 ? r : 0) {}
    
    SvgElementType getType() const override { return SvgElementType::Circle; }
    std::string toSvgString() const override;
    
    // ---------- Getter & Setter ----------

    Point getCenter() const { return m_center; } 
    void setCenter(const Point& c) { m_center = c; }
    
    double getRadius() const { return m_radius; } 
    void setRadius(double r) { m_radius = (r > 0 ? r : 0); }
};

class SvgEllipse : public SvgElement {
private: 
    Point m_center; 
    double m_rx; 
    double m_ry;

public: 
    SvgEllipse(Point c = {0,0}, double r_x = 0, double r_y = 0) 
        : m_center(c), m_rx(r_x > 0 ? r_x : 0), m_ry(r_y > 0 ? r_y : 0) {}
    
    SvgElementType getType() const override { return SvgElementType::Ellipse; }
    std::string toSvgString() const override;
    
    // ---------- Getter & Setter ----------

    Point getCenter() const { return m_center; } 
    void setCenter(const Point& c) { m_center = c; }
    
    double getRx() const { return m_rx; } 
    void setRx(double r_x) { m_rx = (r_x > 0 ? r_x : 0); }
    
    double getRy() const { return m_ry; } 
    void setRy(double r_y) { m_ry = (r_y > 0 ? r_y : 0); }
};

class SvgPolygon : public SvgElement {
protected: // 改为 protected 以便派生类可以访问
    std::vector<Point> m_points;

public:
    SvgPolygon(const std::vector<Point>& pts = {}) : m_points(pts) {}
    
    SvgElementType getType() const override { return SvgElementType::Polygon; }
    std::string toSvgString() const override;
    
    // ---------- Getter & Setter ----------

    const std::vector<Point>& getPoints() const { return m_points; }
    void setPoints(const std::vector<Point>& pts) { m_points = pts; }
    
    void addPoint(const Point& p) { m_points.push_back(p); }
};


class SvgPolyline : public SvgElement {
private: 
    std::vector<Point> m_points;

public: 
    SvgPolyline(const std::vector<Point>& pts = {}) : m_points(pts) {}
    
    SvgElementType getType() const override { return SvgElementType::Polyline; }
    std::string toSvgString() const override;
    
    // ---------- Getter & Setter ----------

    const std::vector<Point>& getPoints() const { return m_points; } 
    void setPoints(const std::vector<Point>& pts) { m_points = pts; }
    
    void addPoint(const Point& p) { m_points.push_back(p); }
};

// 五边形
class SvgPentagon : public SvgPolygon {
public: 
    SvgPentagon(Point center = {0,0}, double radius = 0);
    
    SvgElementType getType() const override { return SvgElementType::Pentagon; }
};

// 六边形
class SvgHexagon : public SvgPolygon {
public: 
    SvgHexagon(Point center = {0,0}, double radius = 0);
    
    
    SvgElementType getType() const override { return SvgElementType::Hexagon; }
};

// 五角星
class SvgStar : public SvgPolygon {
public: 
    SvgStar(Point center = {0,0}, double outerRadius = 0, double innerRadius = 0, 
            int numPoints = 5, double startAngleDeg = -90);
    
    
    SvgElementType getType() const override { 
        return SvgElementType::Star; 
    }
};
