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
    Polyline,   // 自由线
    Pentagon,   // 五边形
    Hexagon,    // 六边形
    Star        // 五角星
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
        // TODO: 可以后续修改用更好的判断逻辑
        if (s.rfind("rgba", 0) == 0) { // RGBA
            sscanf(s.c_str(), "rgba(%d,%d,%d,%lf)", &c.r, &c.g, &c.b, &reinterpret_cast<double&>(c.alpha)); // 简化，实际应解析0-1的alpha再转换
            c.alpha = static_cast<int>(c.alpha * 255.0); // 假设输入 alpha 是0-1
        } else if (s.rfind("rgb", 0) == 0) { // RGB
            sscanf(s.c_str(), "rgb(%d,%d,%d)", &c.r, &c.g, &c.b);
            c.alpha = 255;
        } else if (s == "none" || s.empty()) {
            c.alpha = 0; // 透明
        }
        // TODO: 可以添加对十六进制颜色等的支持
        return c;
    }
};

struct Transform {
    std::string transform_str; 
    // 例如 "translate(10,20) rotate(45)"
    // TODO: 可以在这里添加更结构化的变换数据和操作
    void translate(double tx, double ty) {
        std::stringstream ss;
        ss << "translate(" << tx << "," << ty << ")";
        if (!transform_str.empty()) {
            transform_str += " ";
        }
        transform_str += ss.str();
    }
    void rotate(double angle, double cx = -1, double cy = -1) { // cx, cy为可选旋转中心
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
    // TODO: ... 其他变换方法如 scale, skewX, skewY
};
