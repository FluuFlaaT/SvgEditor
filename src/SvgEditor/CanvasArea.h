#pragma once
#include <QGraphicsView>

class CanvasArea : public QGraphicsView
{
    Q_OBJECT

public:
    explicit CanvasArea(QWidget *parent = nullptr);
    ~CanvasArea();

private:
    QGraphicsScene* m_scene;
};