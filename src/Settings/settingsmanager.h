#pragma once

#include <QObject>
#include <QSettings>
#include <QColor>
#include <QLoggingCategory>

Q_DECLARE_LOGGING_CATEGORY(settingsManagerLog)

class SettingsManager : public QObject
{
    Q_OBJECT

public:
    // Singleton instance
    static SettingsManager* instance();
    ~SettingsManager();

    // Canvas settings
    int getCanvasWidth() const;
    void setCanvasWidth(int width);

    int getCanvasHeight() const;
    void setCanvasHeight(int height);

    QColor getCanvasColor() const;
    void setCanvasColor(const QColor& color);

    // Default values
    static constexpr int DEFAULT_CANVAS_WIDTH = 800;
    static constexpr int DEFAULT_CANVAS_HEIGHT = 600;
    // Default canvas color is white (defined in implementation)

    // Save and load settings
    void saveSettings();
    void loadSettings();

private:
    SettingsManager(QObject* parent = nullptr);
    SettingsManager(const SettingsManager&) = delete;
    SettingsManager& operator=(const SettingsManager&) = delete;

    // Settings storage
    QSettings m_settings;

    // Canvas settings
    int m_canvasWidth;
    int m_canvasHeight;
    QColor m_canvasColor;
};
