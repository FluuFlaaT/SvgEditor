#pragma once
#include <QGraphicsView>
#include <QGraphicsSvgItem>
#include <QGraphicsRectItem>

class CanvasArea : public QGraphicsView
{
    Q_OBJECT

public:
    explicit CanvasArea(QWidget *parent = nullptr);
    ~CanvasArea();

    bool openFile(const QString&);

private:
    QGraphicsScene* m_scene;
    QGraphicsRectItem* m_backgroundItem;
    QGraphicsSvgItem* m_svgItem;
    QGraphicsRectItem* m_outlineItem;
};