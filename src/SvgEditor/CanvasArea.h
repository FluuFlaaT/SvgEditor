#pragma once
#include <QGraphicsView>
#include <QGraphicsSvgItem>

class CanvasArea : public QGraphicsView
{
    Q_OBJECT

public:
    explicit CanvasArea(QWidget *parent = nullptr);
    ~CanvasArea();

    bool openFile(const QString&);

private:
    QGraphicsScene* m_scene;
};