#pragma once
#include "SvgDocument.h"
#include <memory>

class CoreSvgEngine {
private:
    std::unique_ptr<SvgDocument> m_document; // 唯一的文档指针

public:
    CoreSvgEngine();
    ~CoreSvgEngine();

    CoreSvgEngine(const CoreSvgEngine&) = delete;
    CoreSvgEngine& operator=(const CoreSvgEngine&) = delete;

    SvgDocument* getCurrentDocument() const { return m_document.get(); }
    void createNewDocument(double width, double height, Color bgColor = {255,255,255,255});

    bool loadSvgFile(const std::string& filePath);    bool saveSvgFile(const std::string& filePath) const;
};

