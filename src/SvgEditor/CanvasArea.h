#pragma once
#include <QGraphicsView>
#include <QGraphicsSvgItem>
#include <QGraphicsRectItem>
#include <QGraphicsScene>
#include <QFileInfo>
#include <QScopedPointer>
#include <QSvgRenderer>
#include <QLoggingCategory>
#include "XMLParser.h"

Q_DECLARE_LOGGING_CATEGORY(canvasAreaLog)

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