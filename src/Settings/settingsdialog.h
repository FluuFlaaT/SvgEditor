#pragma once

#include <QDialog>
#include <QColor>
#include <QLoggingCategory>

Q_DECLARE_LOGGING_CATEGORY(settingsDialogLog)

namespace Ui {
class SettingsDialog;
}

class SettingsDialog : public QDialog
{
    Q_OBJECT

public:
    explicit SettingsDialog(QWidget *parent = nullptr);
    ~SettingsDialog();

    // Get the current settings from the dialog
    int getCanvasWidth() const;
    int getCanvasHeight() const;
    QColor getCanvasColor() const;

    // Set the dialog values from current settings
    void setCanvasWidth(int width);
    void setCanvasHeight(int height);
    void setCanvasColor(const QColor& color);

private slots:
    void onCanvasColorButtonClicked();
    void onResetToDefaultsClicked();
    void onAccepted();

private:
    Ui::SettingsDialog *ui;
    QColor m_canvasColor;

    // Load settings from the SettingsManager
    void loadSettings();
    
    // Save settings to the SettingsManager
    void saveSettings();
};
