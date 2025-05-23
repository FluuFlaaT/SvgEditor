#include "settingsdialog.h"
#include "ui_settingsdialog.h"
#include "settingsmanager.h"
#include <QColorDialog>

Q_LOGGING_CATEGORY(settingsDialogLog, "SettingsDialog")

SettingsDialog::SettingsDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SettingsDialog),
    m_canvasColor(Qt::white)
{
    ui->setupUi(this);
    
    // Set window title and properties
    setWindowTitle(tr("SVG Editor Settings"));
    setMinimumWidth(400);
    
    // Connect signals and slots
    connect(ui->buttonBox, &QDialogButtonBox::accepted, this, &SettingsDialog::onAccepted);
    connect(ui->canvasColorButton, &QPushButton::clicked, this, &SettingsDialog::onCanvasColorButtonClicked);
    connect(ui->resetDefaultsButton, &QPushButton::clicked, this, &SettingsDialog::onResetToDefaultsClicked);
    
    // Load current settings
    loadSettings();
    
    qCDebug(settingsDialogLog) << "SettingsDialog initialized";
}

SettingsDialog::~SettingsDialog()
{
    delete ui;
    qCDebug(settingsDialogLog) << "SettingsDialog destroyed";
}

int SettingsDialog::getCanvasWidth() const
{
    return ui->canvasWidthSpinBox->value();
}

int SettingsDialog::getCanvasHeight() const
{
    return ui->canvasHeightSpinBox->value();
}

QColor SettingsDialog::getCanvasColor() const
{
    return m_canvasColor;
}

void SettingsDialog::setCanvasWidth(int width)
{
    ui->canvasWidthSpinBox->setValue(width);
}

void SettingsDialog::setCanvasHeight(int height)
{
    ui->canvasHeightSpinBox->setValue(height);
}

void SettingsDialog::setCanvasColor(const QColor& color)
{
    if (color.isValid()) {
        m_canvasColor = color;
        QString qss = QString("background-color: %1").arg(color.name());
        ui->canvasColorButton->setStyleSheet(qss);
    }
}

void SettingsDialog::onCanvasColorButtonClicked()
{
    QColor color = QColorDialog::getColor(m_canvasColor, this, tr("Select Canvas Color"));
    if (color.isValid()) {
        setCanvasColor(color);
        qCDebug(settingsDialogLog) << "Canvas color changed to:" << color.name();
    }
}

void SettingsDialog::onResetToDefaultsClicked()
{
    // Reset to default values
    setCanvasWidth(SettingsManager::DEFAULT_CANVAS_WIDTH);
    setCanvasHeight(SettingsManager::DEFAULT_CANVAS_HEIGHT);
    setCanvasColor(Qt::white);
    
    qCDebug(settingsDialogLog) << "Settings reset to defaults";
}

void SettingsDialog::onAccepted()
{
    saveSettings();
    accept();
}

void SettingsDialog::loadSettings()
{
    // Get settings from the SettingsManager
    SettingsManager* manager = SettingsManager::instance();
    
    setCanvasWidth(manager->getCanvasWidth());
    setCanvasHeight(manager->getCanvasHeight());
    setCanvasColor(manager->getCanvasColor());
    
    qCDebug(settingsDialogLog) << "Settings loaded from SettingsManager";
}

void SettingsDialog::saveSettings()
{
    // Save settings to the SettingsManager
    SettingsManager* manager = SettingsManager::instance();
    
    manager->setCanvasWidth(getCanvasWidth());
    manager->setCanvasHeight(getCanvasHeight());
    manager->setCanvasColor(getCanvasColor());
    
    // Save to registry
    manager->saveSettings();
    
    qCDebug(settingsDialogLog) << "Settings saved to SettingsManager";
}
