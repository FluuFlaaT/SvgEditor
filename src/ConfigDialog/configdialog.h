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

class ConfigManager;

class ConfigDialog : public QDialog
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
    
    ConfigManager* m_configManager;
    
    // Canvas size controls
    QSpinBox* m_widthSpinBox;
    QSpinBox* m_heightSpinBox;
    
    // Background color controls
    QPushButton* m_colorButton;
    QColor m_currentBackgroundColor;
    
    // Button box
    QDialogButtonBox* m_buttonBox;
    QPushButton* m_resetButton;
}; 