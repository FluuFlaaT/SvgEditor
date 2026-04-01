#pragma once
#include "svgdocument.h"
#include <memory>

class CoreSvgEngine {
private:
    // unique_ptr ensures automatic cleanup and prevents accidental copying
    std::unique_ptr<SvgDocument> m_document;

public:
    CoreSvgEngine();
    ~CoreSvgEngine();

    // Prevent copying to ensure single ownership of document
    CoreSvgEngine(const CoreSvgEngine&) = delete;
    CoreSvgEngine& operator=(const CoreSvgEngine&) = delete;

    SvgDocument* getCurrentDocument() const { return m_document.get(); }
    void createNewDocument(double width, double height, Color bgColor = {255,255,255,255});

    bool loadSvgFile(const std::string& filePath);
    bool saveSvgFile(const std::string& filePath) const;
};



