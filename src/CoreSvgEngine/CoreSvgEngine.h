#pragma once
#include "SvgDocument.h"

class CoreSvgEngine {
private:
    SvgDocument* m_document; // 当前操作的SVG文档

public:
    CoreSvgEngine() : m_document(new SvgDocument()) {}
    ~CoreSvgEngine();

    CoreSvgEngine(const CoreSvgEngine&) = delete;
    CoreSvgEngine& operator=(const CoreSvgEngine&) = delete;

    SvgDocument* getCurrentDocument() const { return m_document; }
    void createNewDocument(double width, double height, Color bgColor = {255,255,255,255});

    bool loadSvgFile(const std::string& filePath);    bool saveSvgFile(const std::string& filePath) const;
};

