#pragma once
#include <QString>
#include <memory> // Add for std::unique_ptr

class SvgDocument; // Forward declaration
class DrawingView; // TODO: 先写在这里，后面完善Qt了之后再替换一下

class FileIOManager {
public:
    FileIOManager();
    bool openSvgFile(const QString& filePath, std::unique_ptr<SvgDocument>& outDocument); // Changed to unique_ptr
    bool saveSvgFile(const QString& filePath, const SvgDocument* doc); // Changed to const SvgDocument*
    bool exportToPng(const QString& filePath, DrawingView* view);
    QString lastError() const;
private:
    QString m_lastError;
};