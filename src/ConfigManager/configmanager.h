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
    // Enables reactive UI updates when settings change
    void settingsChanged();

private:
    explicit ConfigManager(QObject* parent = nullptr);
    ~ConfigManager() = default;
    
    // Singleton instance prevents multiple config managers with inconsistent state
    static ConfigManager* s_instance;
    QSettings* m_settings;
    
    // Compile-time constants ensure consistent defaults across application lifecycle
    static constexpr int DEFAULT_CANVAS_WIDTH = 800;
    static constexpr int DEFAULT_CANVAS_HEIGHT = 600;
    static const QColor DEFAULT_BACKGROUND_COLOR;
}; 