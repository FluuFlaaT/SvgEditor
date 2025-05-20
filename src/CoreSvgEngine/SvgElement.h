#pragma once
#include "CoreSvgStructs.h"
#include <string>
#include <map>
#include <variant>


class SvgElement {
private:
    std::string m_id;
    Color m_strokeColor = {0,0,0,255}; // 默认为黑色边框
    double m_strokeWidth = 1.0;
    Color m_fillColor = {0,0,0,0}; // 默认透明填充
    Transform m_transform;
    double m_opacity = 1.0; // 透明度， 0.0 (透明) - 1.0 (不透明)
    // 更多通用属性，如 stroke-dasharray, stroke-linecap 等可以添加
    std::map<std::string, std::variant<std::string, double, int>> m_attributes; // 其他自定义或不常用属性

public:
    virtual ~SvgElement() = default;

    virtual SvgElementType getType() const = 0;
    virtual void draw() const ;
    virtual std::string toSvgString() const = 0; // 转换为SVG字符串片段
    virtual void parseFromSvgAttributes(const std::map<std::string, std::string>& attributes) { /* 从属性map解析，供SVG解析器调用 */ };

    std::variant<std::string, double, int> getAttribute(const std::string& name) const {
        auto it = m_attributes.find(name);
        if (it != m_attributes.end()) {
            return it->second;
        }
        return {}; // 或者抛出异常
    }
    void setAttribute(const std::string& name, const std::variant<std::string, double, int>& value) {
        m_attributes[name] = value;
    }
    const std::map<std::string, std::variant<std::string, double, int>>& getAllAttributes() const {
        return m_attributes;
    }

    // 生成通用的SVG属性字符串
    std::string getCommonAttributesString() const;    std::string getID() const;
    void setID(const std::string& id);


    // ---------- Getter & Setter ----------
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
