#pragma once

#include <QWidget>
#include <QSvgRenderer>

class SvgDisplayWidget : public QWidget {
    Q_OBJECT

public:
    explicit SvgDisplayWidget(QWidget *parent = nullptr);
    void loadSvgContent(const QString& svgContent);

protected:
    void paintEvent(QPaintEvent *event) override;

private:
    QSvgRenderer *m_renderer;
    QByteArray m_svgData;
};
