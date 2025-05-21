#pragma once
#pragma execution_character_set("utf-8")
#include <QGraphicsView>
#include <QGraphicsScene>
#include "CoreSvgEngine.h"
#include <QSvgRenderer>
#include <QGraphicsSvgItem>
#include <QResizeEvent>
#include <QEvent>
#include "LoggingService.h"

class DrawingLayer : public QGraphicsView {
    Q_OBJECT

public:
    explicit DrawingLayer(QWidget *parent = nullptr);
    ~DrawingLayer();

    void setSvgContent(const QString& svgContent);
    void updateView();
    void changeEvent(QEvent* event) override;

private:
    QGraphicsScene* m_scene;
};
