#pragma once

#include <QWidget>
#include <vector>
#include <QVector>
#include <QPushButton>
#include <QToolButton>
#include <QMenu>
#include <QAction>

class LeftSideBar : public QWidget
{
    Q_OBJECT

public:
    explicit LeftSideBar(QWidget *parent = nullptr);
    ~LeftSideBar();

    // Tool buttons
    QPushButton *selectBtn;
    QPushButton *drawBtn;
    QPushButton *shapeBtn;
    QPushButton *textBtn;
    QPushButton *dragBtn;  // New drag tool button
    QVector<QPushButton**> btnGroup = {&selectBtn, &drawBtn, &shapeBtn, &textBtn, &dragBtn};

    // Zoom buttons - simplified to only keep zoomInBtn
    QPushButton *zoomInBtn;

    QToolButton *zoomMenuBtn;
    QMenu *zoomMenu;
    QAction *zoomInAction;
    QAction *zoomOutAction;
    QAction *resetZoomAction;
    QAction *fitToWindowAction;

signals:
    // Only keep the zoomInRequested signal
    void zoomInRequested();

    // Add a signal for the drag tool
    void dragToolRequested();

    void zoomOutRequested();
    void resetZoomRequested();
    void fitToWindowRequested();

private:

};