#pragma once
#include "IDocumentFacade.h"
#include "../CoreSvgEngine/svgdocument.h"
#include "../CoreSvgEngine/svgelement.h"
#include <vector>
#include <string>

class DocumentFacade : public IDocumentFacade {
public:
    DocumentFacade(SvgDocument* document) : m_document(document) {}
    
    void addShape(SvgElement* shape) override {
        if (m_document && shape) {
            m_document->addElement(std::unique_ptr<SvgElement>(shape));
        }
    }
    
    void removeShape(SvgElement* shape) override {
        if (m_document && shape) {
            m_document->removeElement(shape);
        }
    }
    
    bool removeShapeById(const std::string& id) override {
        if (m_document) {
            return m_document->removeElementById(id);
        }
        return false;
    }
    
    SvgElement* findShapeById(const std::string& id) const override {
        if (!m_document) return nullptr;
        
        for (const auto& elem : m_document->getElements()) {
            if (elem->getID() == id) {
                return elem.get();
            }
        }
        return nullptr;
    }
    
    std::vector<SvgElement*> getAllShapes() const override {
        std::vector<SvgElement*> shapes;
        if (m_document) {
            for (const auto& elem : m_document->getElements()) {
                shapes.push_back(elem.get());
            }
        }
        return shapes;
    }
    
    void clearShapes() override {
        if (m_document) {
            m_document->clearElements();
        }
    }
    
    double getWidth() const override {
        return m_document ? m_document->getWidth() : 0.0;
    }
    
    double getHeight() const override {
        return m_document ? m_document->getHeight() : 0.0;
    }
    
    Color getBackgroundColor() const override {
        return m_document ? m_document->getBackgroundColor() : Color{};
    }
    
    std::string generateSvgContent() const override {
        return m_document ? m_document->generateSvgContent() : "";
    }
    
    bool parseSvgContent(const std::string& content) override {
        return m_document ? m_document->parseSvgContent(content) : false;
    }
    
    SvgDocument* getDocument() const { return m_document; }

private:
    SvgDocument* m_document;
};
