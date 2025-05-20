#pragma once
#include <QString>
#include "CoreSvgEngine.h"

class DrawingView; // TODO: 先写在这里，后面完善Qt了之后在替换一下

class FileIOManager {
public:
    FileIOManager(CoreSvgEngine* engine);
    bool openSvgFile(const QString& filePath, SvgDocument*& outDocument);
    bool saveSvgFile(const QString& filePath, SvgDocument* doc);
    bool exportToPng(const QString& filePath, DrawingView* view);
    QString lastError() const;
    // deprecated: 
    // bool importSvgFragment(const QString& filePath, SvgDocument* currentDoc);
private:
    CoreSvgEngine* m_engine;
    QString m_lastError;
};