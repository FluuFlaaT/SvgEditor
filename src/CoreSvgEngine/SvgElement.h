#pragma once
#include "coresvgstructs.h"
#include <string>
#include <map>
#include <variant>


class SvgElement {
private:
    std::string m_id;
    Color m_strokeColor = {0,0,0,255};
    double m_strokeWidth = 1.0;
    Color m_fillColor = {0,0,0,0};
    Transform m_transform;
    double m_opacity = 1.0;
    std::map<std::string, std::variant<std::string, double, int>> m_attributes;

public:
    virtual ~SvgElement() = default;

    virtual SvgElementType getType() const = 0;
    virtual void draw() const;
    virtual std::string toSvgString() const = 0;
    virtual void parseFromSvgAttributes(const std::map<std::string, std::string>& attributes) {};    
    
    std::variant<std::string, double, int> getAttribute(const std::string& name) const {
        auto it = m_attributes.find(name);
        if (it != m_attributes.end()) {
            return it->second;
        }
        return {};
    }
    void setAttribute(const std::string& name, const std::variant<std::string, double, int>& value) {
        m_attributes[name] = value;
    }
    const std::map<std::string, std::variant<std::string, double, int>>& getAllAttributes() const {
        return m_attributes;
    }

    std::string getCommonAttributesString() const;
    std::string getID() const;
    void setID(const std::string& id);

    Color getStrokeColor() const;
    void setStrokeColor(const Color& color);

    double getStrokeWidth() const;
    void setStrokeWidth(double width);

    Color getFillColor() const;
    void setFillColor(const Color& color);

    Transform getTransform() const;
    void setTransform(const Transform& transform);

    double getOpacity() const;
    void setOpacity(double opacity);
};
