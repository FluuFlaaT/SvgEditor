#pragma once
#include <string>
#include <iostream>
#include <sstream>
#include <algorithm> // Required for std::transform
#include <map> // Required for color name mapping

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

    static Color fromString(const std::string& s_in) {
        Color c;
        std::string s = s_in;
        // Case-insensitive comparison required for SVG color name standards
        std::transform(s.begin(), s.end(), s.begin(), ::tolower);

        // SVG 1.1 standard color names mapping
        static const std::map<std::string, Color> colorNameToHex = {
            {"black", {0, 0, 0, 255}}, {"silver", {192, 192, 192, 255}},
            {"gray", {128, 128, 128, 255}}, {"white", {255, 255, 255, 255}},
            {"maroon", {128, 0, 0, 255}}, {"red", {255, 0, 0, 255}},
            {"purple", {128, 0, 128, 255}}, {"fuchsia", {255, 0, 255, 255}},
            {"green", {0, 128, 0, 255}}, {"lime", {0, 255, 0, 255}},
            {"olive", {128, 128, 0, 255}}, {"yellow", {255, 255, 0, 255}},
            {"navy", {0, 0, 128, 255}}, {"blue", {0, 0, 255, 255}},
            {"teal", {0, 128, 128, 255}}, {"aqua", {0, 255, 255, 255}},
            {"transparent", {0, 0, 0, 0}}
        };

        // SVG specification: empty or "none" means transparent
        if (s.empty() || s == "none") {
            c.alpha = 0;
            return c;
        }

        auto it = colorNameToHex.find(s);
        if (it != colorNameToHex.end()) {
            return it->second;
        }

        if (s.rfind("rgba", 0) == 0) {
            double alpha_double;
            if (sscanf(s.c_str(), "rgba(%d,%d,%d,%lf)", &c.r, &c.g, &c.b, &alpha_double) == 4) {
                c.alpha = static_cast<int>(alpha_double * 255.0);
            } else {
                // Malformed rgba format - default to transparent for safety
                c.alpha = 0; 
            }
        } else if (s.rfind("rgb", 0) == 0) {
            if (sscanf(s.c_str(), "rgb(%d,%d,%d)", &c.r, &c.g, &c.b) == 3) {
                c.alpha = 255;
            } else {
                c.alpha = 0;
            }
        } else if (s[0] == '#') {
            // Standard hex formats: #RGB, #RGBA, #RRGGBB, #RRGGBBAA
            if (s.length() == 7) {
                sscanf(s.c_str(), "#%02x%02x%02x", &c.r, &c.g, &c.b);
                c.alpha = 255;
            } else if (s.length() == 9) {
                 sscanf(s.c_str(), "#%02x%02x%02x%02x", &c.r, &c.g, &c.b, &c.alpha);
            } else if (s.length() == 4) {
                // Short hex format requires expansion (e.g., #RGB -> #RRGGBB)
                int r_short, g_short, b_short;
                sscanf(s.c_str(), "#%1x%1x%1x", &r_short, &g_short, &b_short);
                c.r = r_short * 16 + r_short;
                c.g = g_short * 16 + g_short;
                c.b = b_short * 16 + b_short;
                c.alpha = 255;
            } else if (s.length() == 5) {
                int r_short, g_short, b_short, a_short;
                sscanf(s.c_str(), "#%1x%1x%1x%1x", &r_short, &g_short, &b_short, &a_short);
                c.r = r_short * 16 + r_short;
                c.g = g_short * 16 + g_short;
                c.b = b_short * 16 + b_short;
                c.alpha = a_short * 16 + a_short;
            }
             else {
                c.alpha = 0;
            }
        } else {
            // Unknown format - default to transparent for graceful degradation
            c.r = 0; c.g = 0; c.b = 0; c.alpha = 0;
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
        // Center point is optional for rotation
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
