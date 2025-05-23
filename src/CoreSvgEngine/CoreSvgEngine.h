#pragma once
#include "svgdocument.h"
#include <memory>
#include "../Settings/settingsmanager.h"

#if defined(_MSC_VER) || defined(WIN64) || defined(_WIN64) || defined(__WIN64__) || defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__NT__)
#  define DECL_EXPORT __declspec(dllexport)
#  define DECL_IMPORT __declspec(dllimport)
#else
#  define DECL_EXPORT     __attribute__((visibility("default")))
#  define DECL_IMPORT     __attribute__((visibility("default")))
#endif

#if defined(CORESVGENGINE_LIBRARY)
#  define CORESVGENGINE_EXPORT DECL_EXPORT
#else
#  define CORESVGENGINE_EXPORT DECL_IMPORT
#endif

class CORESVGENGINE_EXPORT CoreSvgEngine {
private:
    std::unique_ptr<SvgDocument> m_document; // 唯一的文档指针

public:
    CoreSvgEngine();
    ~CoreSvgEngine();

    CoreSvgEngine(const CoreSvgEngine&) = delete;
    CoreSvgEngine& operator=(const CoreSvgEngine&) = delete;

    SvgDocument* getCurrentDocument() const { return m_document.get(); }
    void createNewDocument(double width, double height, Color bgColor = {255,255,255,255});

    bool loadSvgFile(const std::string& filePath);
    bool saveSvgFile(const std::string& filePath) const;
};
