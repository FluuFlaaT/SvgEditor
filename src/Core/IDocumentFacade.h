#pragma once
#include <memory>
#include <vector>
#include <string>
#include "../CoreSvgEngine/coresvgstructs.h"

class SvgElement;

class IDocumentFacade {
public:
    virtual ~IDocumentFacade() = default;

    virtual void addShape(SvgElement* shape) = 0;
    virtual void removeShape(SvgElement* shape) = 0;
    virtual bool removeShapeById(const std::string& id) = 0;
    virtual SvgElement* findShapeById(const std::string& id) const = 0;
    virtual std::vector<SvgElement*> getAllShapes() const = 0;
    virtual void clearShapes() = 0;

    virtual double getWidth() const = 0;
    virtual double getHeight() const = 0;
    virtual Color getBackgroundColor() const = 0;

    virtual std::string generateSvgContent() const = 0;
    virtual bool parseSvgContent(const std::string& content) = 0;
};
