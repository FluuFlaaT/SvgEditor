#pragma once

#include <QDialog>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QLabel>
#include <QSpinBox>
#include <QPushButton>
#include <QColorDialog>
#include <QFrame>
#include <QGroupBox>
#include <QDialogButtonBox>
#include "configdialog_export.h"

class ConfigManager;

/**
 * Modal dialog for configuring application-wide default settings.
 * Provides a centralized interface to modify user preferences that affect
 * new canvas creation, avoiding the need to set these values repeatedly.
 */
class CONFIGDIALOG_EXPORT ConfigDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ConfigDialog(QWidget* parent = nullptr);
    ~ConfigDialog() = default;

public slots:
    void accept() override;
    void reject() override;

private slots:
    void onColorButtonClicked();
    void onResetToDefaultsClicked();
    void updateColorDisplay();

private:
    void setupUI();
    void loadCurrentSettings();
    void saveSettings();
    
    // Direct reference to singleton to avoid unnecessary coupling
    ConfigManager* m_configManager;
    
    // Canvas size controls
    QSpinBox* m_widthSpinBox;
    QSpinBox* m_heightSpinBox;
    
    // Background color controls - using button instead of label for better UX
    QPushButton* m_colorButton;
    QColor m_currentBackgroundColor;  // Cache to prevent frequent config reads
    
    // Button box
    QDialogButtonBox* m_buttonBox;
    QPushButton* m_resetButton;
}; 