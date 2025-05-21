#include "SvgDisplayWidget.h"
#include <QPainter>
#include <QFile>
#include <QDebug>

SvgDisplayWidget::SvgDisplayWidget(QWidget *parent)
    : QWidget(parent), m_renderer(new QSvgRenderer(this)) {}

void SvgDisplayWidget::loadSvgContent(const QString& svgContent) {
    m_svgData = svgContent.toUtf8();
    m_renderer->load(m_svgData);
    update();
}

void SvgDisplayWidget::paintEvent(QPaintEvent *event) {
    Q_UNUSED(event);
    QPainter painter(this);
    if (m_renderer->isValid()) {
        m_renderer->render(&painter);
    }
}
