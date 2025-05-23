#include "settingsmanager.h"

Q_LOGGING_CATEGORY(settingsManagerLog, "SettingsManager")

// Static instance
static SettingsManager* s_instance = nullptr;

SettingsManager* SettingsManager::instance()
{
    if (!s_instance) {
        s_instance = new SettingsManager();
    }
    return s_instance;
}

SettingsManager::SettingsManager(QObject* parent)
    : QObject(parent),
      m_settings("HKEY_CURRENT_USER\\SOFTWARE\\svgeditor", QSettings::NativeFormat),
      m_canvasWidth(DEFAULT_CANVAS_WIDTH),
      m_canvasHeight(DEFAULT_CANVAS_HEIGHT),
      m_canvasColor(Qt::white)
{
    qCDebug(settingsManagerLog) << "SettingsManager initialized";
    loadSettings();
}

SettingsManager::~SettingsManager()
{
    saveSettings();
    qCDebug(settingsManagerLog) << "SettingsManager destroyed";
}

int SettingsManager::getCanvasWidth() const
{
    return m_canvasWidth;
}

void SettingsManager::setCanvasWidth(int width)
{
    if (width > 0 && width != m_canvasWidth) {
        m_canvasWidth = width;
        qCDebug(settingsManagerLog) << "Canvas width set to:" << width;
    }
}

int SettingsManager::getCanvasHeight() const
{
    return m_canvasHeight;
}

void SettingsManager::setCanvasHeight(int height)
{
    if (height > 0 && height != m_canvasHeight) {
        m_canvasHeight = height;
        qCDebug(settingsManagerLog) << "Canvas height set to:" << height;
    }
}

QColor SettingsManager::getCanvasColor() const
{
    return m_canvasColor;
}

void SettingsManager::setCanvasColor(const QColor& color)
{
    if (color.isValid() && color != m_canvasColor) {
        m_canvasColor = color;
        qCDebug(settingsManagerLog) << "Canvas color set to:" << color.name();
    }
}

void SettingsManager::saveSettings()
{
    m_settings.setValue("Canvas/Width", m_canvasWidth);
    m_settings.setValue("Canvas/Height", m_canvasHeight);
    m_settings.setValue("Canvas/Color", m_canvasColor.name());
    
    m_settings.sync();
    qCDebug(settingsManagerLog) << "Settings saved to registry";
}

void SettingsManager::loadSettings()
{
    m_canvasWidth = m_settings.value("Canvas/Width", DEFAULT_CANVAS_WIDTH).toInt();
    m_canvasHeight = m_settings.value("Canvas/Height", DEFAULT_CANVAS_HEIGHT).toInt();
    m_canvasColor = QColor(m_settings.value("Canvas/Color", "#FFFFFF").toString());
    
    qCDebug(settingsManagerLog) << "Settings loaded from registry:"
                               << "Canvas width =" << m_canvasWidth
                               << "Canvas height =" << m_canvasHeight
                               << "Canvas color =" << m_canvasColor.name();
}
