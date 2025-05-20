#pragma once
#define _USE_MATH_DEFINES
#include <cmath>
#include <vector>
#include "SvgElement.h"

class SvgLine : public SvgElement {
private: 
    Point m_p1; 
    Point m_p2;

public: 
    SvgLine(Point start = {0,0}, Point end = {0,0}); 
    
    SvgElementType getType() const override { return SvgElementType::Line; }
    std::string toSvgString() const override;

    Point getP1() const { return m_p1; } 
    void setP1(const Point& p);
    
    Point getP2() const { return m_p2; } 
    void setP2(const Point& p);
};

class SvgRectangle : public SvgElement {
private: 
    Point m_topLeft; 
    double m_width; 
    double m_height; 
    double m_rx; 
    double m_ry; // 圆角半径

public: 
    SvgRectangle(Point tl = {0,0}, double w = 0, double h = 0, double rx_ = 0.0, double ry_ = 0.0);
    
    SvgElementType getType() const override { return SvgElementType::Rectangle; }
    std::string toSvgString() const override;

    Point getTopLeft() const { return m_topLeft; } 
    void setTopLeft(const Point& p);
    
    double getWidth() const { return m_width; } 
    void setWidth(double w);
    
    double getHeight() const { return m_height; } 
    void setHeight(double h);
    
    double getRx() const { return m_rx; } 
    void setRx(double rx_val);
    
    double getRy() const { return m_ry; } 
    void setRy(double ry_val);
};

class SvgCircle : public SvgElement {
private: 
    Point m_center; 
    double m_radius;

public: 
    SvgCircle(Point c = {0,0}, double r = 0);
    
    SvgElementType getType() const override { return SvgElementType::Circle; }
    std::string toSvgString() const override;

    Point getCenter() const { return m_center; } 
    void setCenter(const Point& c);
    
    double getRadius() const { return m_radius; } 
    void setRadius(double r);
};

class SvgEllipse : public SvgElement {
private: 
    Point m_center; 
    double m_rx; 
    double m_ry;

public: 
    SvgEllipse(Point c = {0,0}, double r_x = 0, double r_y = 0);
    
    SvgElementType getType() const override { return SvgElementType::Ellipse; }
    std::string toSvgString() const override;

    Point getCenter() const { return m_center; } 
    void setCenter(const Point& c);
    
    double getRx() const { return m_rx; } 
    void setRx(double r_x);
    
    double getRy() const { return m_ry; } 
    void setRy(double r_y);
};

class SvgPolygon : public SvgElement {
protected: // 改为 protected 以便派生类可以访问
    std::vector<Point> m_points;

public:
    SvgPolygon(const std::vector<Point>& pts = {});
    
    SvgElementType getType() const override { return SvgElementType::Polygon; }
    std::string toSvgString() const override;

    const std::vector<Point>& getPoints() const { return m_points; }
    void setPoints(const std::vector<Point>& pts);
    
    void addPoint(const Point& p);
};


class SvgPolyline : public SvgElement {
private: 
    std::vector<Point> m_points;

public: 
    SvgPolyline(const std::vector<Point>& pts = {});
    
    SvgElementType getType() const override { return SvgElementType::Polyline; }
    std::string toSvgString() const override;

    const std::vector<Point>& getPoints() const { return m_points; } 
    void setPoints(const std::vector<Point>& pts);
    
    void addPoint(const Point& p);
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
