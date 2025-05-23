#pragma once

#include <QObject>
#include <QSettings>
#include <QtCore/QSize>
#include <QtGui/QColor>

class ConfigManager : public QObject
{
    Q_OBJECT

public:
    static ConfigManager* instance();
    
    // Default canvas settings
    QSize getDefaultCanvasSize() const;
    void setDefaultCanvasSize(const QSize& size);
    
    QColor getDefaultCanvasBackgroundColor() const;
    void setDefaultCanvasBackgroundColor(const QColor& color);
    
    // Check if settings exist in registry
    bool hasExistingSettings() const;
    
    // Initialize with default values if no settings exist
    void initializeDefaults();
    
    // Reset to factory defaults
    void resetToDefaults();

signals:
    void settingsChanged();

private:
    explicit ConfigManager(QObject* parent = nullptr);
    ~ConfigManager() = default;
    
    static ConfigManager* s_instance;
    QSettings* m_settings;
    
    // Default values
    static constexpr int DEFAULT_CANVAS_WIDTH = 800;
    static constexpr int DEFAULT_CANVAS_HEIGHT = 600;
    static const QColor DEFAULT_BACKGROUND_COLOR;
}; 