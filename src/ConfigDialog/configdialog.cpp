#include "configdialog.h"
#include "../ConfigManager/configmanager.h"
#include <QApplication>

ConfigDialog::ConfigDialog(QWidget* parent)
    : QDialog(parent)
    , m_configManager(ConfigManager::instance())
{
    setWindowTitle(tr("Preferences"));
    setModal(true);
    // Fixed size prevents UI layout issues with dynamic content
    setFixedSize(400, 300);
    
    setupUI();
    loadCurrentSettings();
}

void ConfigDialog::setupUI()
{
    auto* mainLayout = new QVBoxLayout(this);
    
    // Group related settings to improve visual organization
    auto* canvasGroup = new QGroupBox(tr("Default Canvas Settings"), this);
    auto* canvasLayout = new QGridLayout(canvasGroup);
    
    // Canvas size settings
    canvasLayout->addWidget(new QLabel(tr("Width:")), 0, 0);
    m_widthSpinBox = new QSpinBox();
    // Range limits prevent unreasonable canvas sizes that could cause performance issues
    m_widthSpinBox->setRange(100, 5000);
    m_widthSpinBox->setSuffix(" px");
    canvasLayout->addWidget(m_widthSpinBox, 0, 1);
    
    canvasLayout->addWidget(new QLabel(tr("Height:")), 1, 0);
    m_heightSpinBox = new QSpinBox();
    m_heightSpinBox->setRange(100, 5000);
    m_heightSpinBox->setSuffix(" px");
    canvasLayout->addWidget(m_heightSpinBox, 1, 1);
    
    // Background color setting
    canvasLayout->addWidget(new QLabel(tr("Background Color:")), 2, 0);
    m_colorButton = new QPushButton();
    m_colorButton->setFixedSize(60, 30);
    connect(m_colorButton, &QPushButton::clicked, this, &ConfigDialog::onColorButtonClicked);
    canvasLayout->addWidget(m_colorButton, 2, 1, Qt::AlignLeft);
    
    mainLayout->addWidget(canvasGroup);
    
    // Push buttons to bottom for better visual hierarchy
    mainLayout->addStretch();
    
    // Reset to defaults button
    m_resetButton = new QPushButton(tr("Reset to Defaults"));
    connect(m_resetButton, &QPushButton::clicked, this, &ConfigDialog::onResetToDefaultsClicked);
    
    // Dialog buttons
    m_buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
    connect(m_buttonBox, &QDialogButtonBox::accepted, this, &ConfigDialog::accept);
    connect(m_buttonBox, &QDialogButtonBox::rejected, this, &ConfigDialog::reject);
    
    auto* buttonLayout = new QHBoxLayout();
    buttonLayout->addWidget(m_resetButton);
    buttonLayout->addStretch();
    buttonLayout->addWidget(m_buttonBox);
    
    mainLayout->addLayout(buttonLayout);
}

void ConfigDialog::loadCurrentSettings()
{
    // Initialize UI with current values to maintain user context
    QSize canvasSize = m_configManager->getDefaultCanvasSize();
    m_widthSpinBox->setValue(canvasSize.width());
    m_heightSpinBox->setValue(canvasSize.height());
    
    m_currentBackgroundColor = m_configManager->getDefaultCanvasBackgroundColor();
    updateColorDisplay();
}

void ConfigDialog::updateColorDisplay()
{
    // Visual feedback shows current color selection without opening color dialog
    QString style = QString("QPushButton { background-color: %1; border: 1px solid #999; }")
                    .arg(m_currentBackgroundColor.name());
    m_colorButton->setStyleSheet(style);
    m_colorButton->setText(m_currentBackgroundColor.name());
}

void ConfigDialog::onColorButtonClicked()
{
    QColor newColor = QColorDialog::getColor(m_currentBackgroundColor, this, tr("Select Background Color"));
    // Only update if user didn't cancel the color dialog
    if (newColor.isValid()) {
        m_currentBackgroundColor = newColor;
        updateColorDisplay();
    }
}

void ConfigDialog::onResetToDefaultsClicked()
{
    // Hardcoded defaults ensure consistent behavior even if config file is corrupted
    m_widthSpinBox->setValue(800);  // Standard web-friendly default
    m_heightSpinBox->setValue(600); // 4:3 aspect ratio for better compatibility
    m_currentBackgroundColor = QColor(255, 255, 255); // White background for design clarity
    updateColorDisplay();
}

void ConfigDialog::saveSettings()
{
    // Batch save to avoid multiple config file writes
    QSize newSize(m_widthSpinBox->value(), m_heightSpinBox->value());
    m_configManager->setDefaultCanvasSize(newSize);
    m_configManager->setDefaultCanvasBackgroundColor(m_currentBackgroundColor);
}

void ConfigDialog::accept()
{
    // Only save on explicit acceptance to respect user workflow
    saveSettings();
    QDialog::accept();
}

void ConfigDialog::reject()
{
    // Discard changes to maintain expected dialog behavior
    QDialog::reject();
} 