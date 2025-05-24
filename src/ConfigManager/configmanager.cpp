#include "configmanager.h"
#include <QtWidgets/QApplication>

ConfigManager* ConfigManager::s_instance = nullptr;
// White background provides optimal contrast for design work
const QColor ConfigManager::DEFAULT_BACKGROUND_COLOR = QColor(255, 255, 255);

ConfigManager* ConfigManager::instance()
{
    if (!s_instance) {
        // Parent to qApp ensures cleanup during application shutdown
        s_instance = new ConfigManager(qApp);
    }
    return s_instance;
}

ConfigManager::ConfigManager(QObject* parent)
    : QObject(parent)
{
    // Registry storage persists across application sessions and system reboots
    m_settings = new QSettings("svgeditor", "SvgEditor", this);
    
    // First-run initialization prevents undefined behavior with missing settings
    if (!hasExistingSettings()) {
        initializeDefaults();
    }
}

QSize ConfigManager::getDefaultCanvasSize() const
{
    int width = m_settings->value("canvas/width", DEFAULT_CANVAS_WIDTH).toInt();
    int height = m_settings->value("canvas/height", DEFAULT_CANVAS_HEIGHT).toInt();
    return QSize(width, height);
}

void ConfigManager::setDefaultCanvasSize(const QSize& size)
{
    m_settings->setValue("canvas/width", size.width());
    m_settings->setValue("canvas/height", size.height());
    // Force immediate disk write to prevent data loss on unexpected shutdown
    m_settings->sync();
    emit settingsChanged();
}

QColor ConfigManager::getDefaultCanvasBackgroundColor() const
{
    QString colorName = m_settings->value("canvas/backgroundColor", DEFAULT_BACKGROUND_COLOR.name()).toString();
    return QColor(colorName);
}

void ConfigManager::setDefaultCanvasBackgroundColor(const QColor& color)
{
    m_settings->setValue("canvas/backgroundColor", color.name());
    m_settings->sync();
    emit settingsChanged();
}

bool ConfigManager::hasExistingSettings() const
{
    // Existence check prevents overwriting user customizations during startup
    return m_settings->contains("canvas/width") || 
           m_settings->contains("canvas/height") || 
           m_settings->contains("canvas/backgroundColor");
}

void ConfigManager::initializeDefaults()
{
    // Conditional initialization preserves partial user configurations
    if (!m_settings->contains("canvas/width")) {
        m_settings->setValue("canvas/width", DEFAULT_CANVAS_WIDTH);
    }
    if (!m_settings->contains("canvas/height")) {
        m_settings->setValue("canvas/height", DEFAULT_CANVAS_HEIGHT);
    }
    if (!m_settings->contains("canvas/backgroundColor")) {
        m_settings->setValue("canvas/backgroundColor", DEFAULT_BACKGROUND_COLOR.name());
    }
    m_settings->sync();
}

void ConfigManager::resetToDefaults()
{
    // Direct overwrite for reset functionality - ignores existing values intentionally
    m_settings->setValue("canvas/width", DEFAULT_CANVAS_WIDTH);
    m_settings->setValue("canvas/height", DEFAULT_CANVAS_HEIGHT);
    m_settings->setValue("canvas/backgroundColor", DEFAULT_BACKGROUND_COLOR.name());
    m_settings->sync();
    emit settingsChanged();
} 