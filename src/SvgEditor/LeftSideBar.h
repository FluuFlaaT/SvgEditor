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

    QPushButton *selectBtn;
    QPushButton *drawBtn;
    QPushButton *shapeBtn;
    QPushButton *textBtn;
    QVector<QPushButton**> btnGroup = {&selectBtn, &drawBtn, &shapeBtn, &textBtn};

private:
    
};