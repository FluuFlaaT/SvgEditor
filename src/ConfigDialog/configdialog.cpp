#include "configdialog.h"
#include "../ConfigManager/configmanager.h"
#include <QApplication>

ConfigDialog::ConfigDialog(QWidget* parent)
    : QDialog(parent)
    , m_configManager(ConfigManager::instance())
{
    setWindowTitle(tr("Preferences"));
    setModal(true);
    setFixedSize(400, 300);
    
    setupUI();
    loadCurrentSettings();
}

void ConfigDialog::setupUI()
{
    auto* mainLayout = new QVBoxLayout(this);
    
    // Canvas settings group
    auto* canvasGroup = new QGroupBox(tr("Default Canvas Settings"), this);
    auto* canvasLayout = new QGridLayout(canvasGroup);
    
    // Canvas size settings
    canvasLayout->addWidget(new QLabel(tr("Width:")), 0, 0);
    m_widthSpinBox = new QSpinBox();
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
    
    // Add stretch to push buttons to bottom
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
    // Load canvas size
    QSize canvasSize = m_configManager->getDefaultCanvasSize();
    m_widthSpinBox->setValue(canvasSize.width());
    m_heightSpinBox->setValue(canvasSize.height());
    
    // Load background color
    m_currentBackgroundColor = m_configManager->getDefaultCanvasBackgroundColor();
    updateColorDisplay();
}

void ConfigDialog::updateColorDisplay()
{
    // Set button color to show current selection
    QString style = QString("QPushButton { background-color: %1; border: 1px solid #999; }")
                    .arg(m_currentBackgroundColor.name());
    m_colorButton->setStyleSheet(style);
    m_colorButton->setText(m_currentBackgroundColor.name());
}

void ConfigDialog::onColorButtonClicked()
{
    QColor newColor = QColorDialog::getColor(m_currentBackgroundColor, this, tr("Select Background Color"));
    if (newColor.isValid()) {
        m_currentBackgroundColor = newColor;
        updateColorDisplay();
    }
}

void ConfigDialog::onResetToDefaultsClicked()
{
    // Reset to default values
    m_widthSpinBox->setValue(800);  // DEFAULT_CANVAS_WIDTH
    m_heightSpinBox->setValue(600); // DEFAULT_CANVAS_HEIGHT
    m_currentBackgroundColor = QColor(255, 255, 255); // White
    updateColorDisplay();
}

void ConfigDialog::saveSettings()
{
    // Save canvas size
    QSize newSize(m_widthSpinBox->value(), m_heightSpinBox->value());
    m_configManager->setDefaultCanvasSize(newSize);
    
    // Save background color
    m_configManager->setDefaultCanvasBackgroundColor(m_currentBackgroundColor);
}

void ConfigDialog::accept()
{
    saveSettings();
    QDialog::accept();
}

void ConfigDialog::reject()
{
    // Don't save changes, just close
    QDialog::reject();
} 