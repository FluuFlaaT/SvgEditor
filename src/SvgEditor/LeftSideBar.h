#pragma once

#include <QWidget>
#include <vector>
#include <QVector>
#include <QPushButton>

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
    QVector<QPushButton**> btnGroup = {&selectBtn, &drawBtn, &shapeBtn, &textBtn};

    // Zoom buttons
    QPushButton *zoomInBtn;
    QPushButton *zoomOutBtn;
    QPushButton *zoomResetBtn;
    QPushButton *zoomFitBtn;

signals:
    void zoomInRequested();
    void zoomOutRequested();
    void zoomResetRequested();
    void zoomFitRequested();

private:

};