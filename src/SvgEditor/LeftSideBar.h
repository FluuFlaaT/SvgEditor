#pragma once

#include <QWidget>
#include <vector>

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
    std::vector<QPushButton**> btnGroup = {&selectBtn, &drawBtn, &shapeBtn, &textBtn};

private:
    
};