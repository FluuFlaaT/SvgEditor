#pragma once

#include <QMainWindow>
#include "SvgDisplayWidget.h"
#include "CoreSvgEngine.h"
#include "LoggingService.h"

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void createCircle();

private:
    SvgDisplayWidget *m_svgWidget;
    CoreSvgEngine m_svgEngine;
};
