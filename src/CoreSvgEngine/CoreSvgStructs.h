#pragma once
#include <string>
#include <iostream>
#include <sstream>

enum class SvgElementType {
    Line,
    Rectangle,
    Circle,
    Ellipse,
    Polygon,
    Text,
    Polyline,
    Pentagon,
    Hexagon,
    Star
};

struct Point {
    double x = 0.0;
    double y = 0.0;
};

struct Color {
    int r = 0;
    int g = 0;
    int b = 0;
    int alpha = 255;

    std::string toString() const {
        std::stringstream ss;
        if (alpha == 255) {
            ss << "rgb(" << r << "," << g << "," << b << ")";
        } else {
            ss << "rgba(" << r << "," << g << "," << b << "," << static_cast<double>(alpha) / 255.0 << ")";
        }
        return ss.str();
    }

    static Color fromString(const std::string& s) {
        Color c;
        if (s.rfind("rgba", 0) == 0) {
            sscanf(s.c_str(), "rgba(%d,%d,%d,%lf)", &c.r, &c.g, &c.b, &reinterpret_cast<double&>(c.alpha));
            c.alpha = static_cast<int>(c.alpha * 255.0);
        } else if (s.rfind("rgb", 0) == 0) {
            sscanf(s.c_str(), "rgb(%d,%d,%d)", &c.r, &c.g, &c.b);
            c.alpha = 255;
        } else if (s == "none" || s.empty()) {
            c.alpha = 0;
        }
        return c;
    }
};

struct Transform {
    std::string transform_str;
    
    void translate(double tx, double ty) {
        std::stringstream ss;
        ss << "translate(" << tx << "," << ty << ")";
        if (!transform_str.empty()) {
            transform_str += " ";
        }
        transform_str += ss.str();
    }
    
    void rotate(double angle, double cx = -1, double cy = -1) {
        std::stringstream ss;
        ss << "rotate(" << angle;
        if (cx != -1 && cy != -1) {
            ss << "," << cx << "," << cy;
        }
        ss << ")";
        if (!transform_str.empty()) {
            transform_str += " ";
        }
        transform_str += ss.str();
    }
};
