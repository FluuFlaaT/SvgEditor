#include "CustomTooltip.h"
#include <QPainter>
#include <QPainterPath>
#include <QApplication>
#include <QScreen>
#include <QDesktopWidget>
#include <QFontMetrics>

CustomTooltip::CustomTooltip(QWidget* parent)
    : QWidget(parent, Qt::ToolTip | Qt::FramelessWindowHint)
    , m_showDelay(500)    // Default 500ms delay before showing
    , m_hideDelay(100)    // Default 100ms delay before hiding
{
    setAttribute(Qt::WA_TranslucentBackground);
    setAttribute(Qt::WA_ShowWithoutActivating);
    setWindowOpacity(0.95);
    
    setupUI();
    
    m_showTimer = new QTimer(this);
    m_showTimer->setSingleShot(true);
    connect(m_showTimer, &QTimer::timeout, this, &CustomTooltip::onShowTimer);
    
    m_hideTimer = new QTimer(this);
    m_hideTimer->setSingleShot(true);
    connect(m_hideTimer, &QTimer::timeout, this, &CustomTooltip::onHideTimer);
}

void CustomTooltip::setupUI()
{
    m_layout = new QVBoxLayout(this);
    m_layout->setContentsMargins(TOOLTIP_MARGIN, TOOLTIP_MARGIN, TOOLTIP_MARGIN, TOOLTIP_MARGIN);
    m_layout->setSpacing(5);
    
    // Title label with bold font
    m_titleLabel = new QLabel(this);
    m_titleLabel->setStyleSheet(
        "QLabel {"
        "font-weight: bold;"
        "font-size: 14px;"
        "color: #2c3e50;"
        "margin: 0px;"
        "}"
    );
    m_titleLabel->setWordWrap(true);
    
    // Text label with normal font
    m_textLabel = new QLabel(this);
    m_textLabel->setStyleSheet(
        "QLabel {"
        "font-size: 12px;"
        "color: #34495e;"
        "margin: 0px;"
        "line-height: 1.4;"
        "}"
    );
    m_textLabel->setWordWrap(true);
    
    m_layout->addWidget(m_titleLabel);
    m_layout->addWidget(m_textLabel);
    
    // Add shadow effect for better visual appearance
    QGraphicsDropShadowEffect* shadowEffect = new QGraphicsDropShadowEffect(this);
    shadowEffect->setBlurRadius(8);
    shadowEffect->setColor(QColor(0, 0, 0, 100));
    shadowEffect->setOffset(2, 2);
    setGraphicsEffect(shadowEffect);
}

void CustomTooltip::showTooltip(const QString& title, const QString& text, const QPoint& position)
{
    updateContent(title, text);
    positionTooltip(position);
    show();
    raise();
}

void CustomTooltip::hideTooltip()
{
    hide();
}

void CustomTooltip::scheduleShow(const QString& title, const QString& text, const QPoint& position)
{
    m_pendingTitle = title;
    m_pendingText = text;
    m_pendingPosition = position;
    
    // Cancel any pending hide
    m_hideTimer->stop();
    
    // Start show timer
    m_showTimer->start(m_showDelay);
}

void CustomTooltip::scheduleHide()
{
    // Cancel any pending show
    m_showTimer->stop();
    
    // Start hide timer
    m_hideTimer->start(m_hideDelay);
}

void CustomTooltip::onShowTimer()
{
    showTooltip(m_pendingTitle, m_pendingText, m_pendingPosition);
}

void CustomTooltip::onHideTimer()
{
    hideTooltip();
}

void CustomTooltip::updateContent(const QString& title, const QString& text)
{
    m_titleLabel->setText(title);
    m_textLabel->setText(text);
    
    // Calculate optimal size
    QFontMetrics titleMetrics(m_titleLabel->font());
    QFontMetrics textMetrics(m_textLabel->font());
    
    int titleWidth = titleMetrics.boundingRect(0, 0, TOOLTIP_MAX_WIDTH, 0, Qt::TextWordWrap, title).width();
    int textWidth = textMetrics.boundingRect(0, 0, TOOLTIP_MAX_WIDTH, 0, Qt::TextWordWrap, text).width();
    
    int maxWidth = qMax(titleWidth, textWidth);
    maxWidth = qMin(maxWidth + 2 * TOOLTIP_MARGIN, TOOLTIP_MAX_WIDTH);
    
    setMaximumWidth(maxWidth);
    adjustSize();
}

void CustomTooltip::positionTooltip(const QPoint& targetPosition)
{
    QRect screenGeometry = QApplication::primaryScreen()->geometry();
    
    // targetPosition should already be in global coordinates
    QPoint globalPos = targetPosition;
    
    // Calculate tooltip position with small offset to the right and slightly down
    QPoint tooltipPos = globalPos + QPoint(3, -5); // Position slightly above and to the right
    
    // Ensure tooltip stays within screen bounds
    if (tooltipPos.x() + width() > screenGeometry.right()) {
        // If no space on the right, position to the left of the button
        tooltipPos.setX(globalPos.x() - width() - 3);
    }
    
    if (tooltipPos.y() + height() > screenGeometry.bottom()) {
        // If no space below, position above
        tooltipPos.setY(globalPos.y() - height() + 5);
    }
    
    // Ensure minimum bounds
    if (tooltipPos.x() < screenGeometry.left()) {
        tooltipPos.setX(screenGeometry.left() + 5);
    }
    
    if (tooltipPos.y() < screenGeometry.top()) {
        tooltipPos.setY(screenGeometry.top() + 5);
    }
    
    move(tooltipPos);
}

void CustomTooltip::paintEvent(QPaintEvent* event)
{
    Q_UNUSED(event)
    
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    
    // Draw rounded rectangle background
    QRect rect = this->rect().adjusted(2, 2, -2, -2); // Account for shadow
    QPainterPath path;
    path.addRoundedRect(rect, 6, 6);
    
    // Background gradient
    QLinearGradient gradient(0, 0, 0, height());
    gradient.setColorAt(0, QColor(255, 255, 255, 240));
    gradient.setColorAt(1, QColor(248, 249, 250, 240));
    
    painter.fillPath(path, gradient);
    
    // Border
    painter.setPen(QPen(QColor(189, 195, 199), 1));
    painter.drawPath(path);
}