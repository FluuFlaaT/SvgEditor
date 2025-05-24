#pragma once

#include <QWidget>
#include <QLabel>
#include <QVBoxLayout>
#include <QTimer>
#include <QGraphicsDropShadowEffect>

class CustomTooltip : public QWidget
{
    Q_OBJECT

public:
    explicit CustomTooltip(QWidget* parent = nullptr);
    
    // Show tooltip with title and text
    void showTooltip(const QString& title, const QString& text, const QPoint& position);
    
    // Hide tooltip
    void hideTooltip();
    
    // Set delay before showing tooltip
    void setShowDelay(int milliseconds) { m_showDelay = milliseconds; }
    
    // Set delay before hiding tooltip
    void setHideDelay(int milliseconds) { m_hideDelay = milliseconds; }

public slots:
    void scheduleShow(const QString& title, const QString& text, const QPoint& position);
    void scheduleHide();

private slots:
    void onShowTimer();
    void onHideTimer();

protected:
    void paintEvent(QPaintEvent* event) override;

private:
    void setupUI();
    void updateContent(const QString& title, const QString& text);
    void positionTooltip(const QPoint& targetPosition);
    
    QLabel* m_titleLabel;
    QLabel* m_textLabel;
    QVBoxLayout* m_layout;
    
    QTimer* m_showTimer;
    QTimer* m_hideTimer;
    
    QString m_pendingTitle;
    QString m_pendingText;
    QPoint m_pendingPosition;
    
    int m_showDelay;
    int m_hideDelay;
    
    static constexpr int TOOLTIP_MAX_WIDTH = 300;
    static constexpr int TOOLTIP_MARGIN = 10;
}; 