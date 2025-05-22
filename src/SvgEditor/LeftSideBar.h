#pragma once

#include <QWidget>
#include <vector>
#include <QVector>
#include <QPushButton>
#include <QToolButton>
#include <QMenu>
#include <QAction>
#include <QButtonGroup>
#include <QLoggingCategory>

Q_DECLARE_LOGGING_CATEGORY(leftSideBarLog)

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
    QPushButton *dragBtn;
    QPushButton *zoomBtn;  // New zoom tool button
    QVector<QPushButton**> btnGroup = {&selectBtn, &drawBtn, &shapeBtn, &textBtn, &dragBtn, &zoomBtn};

    // Zoom sub-buttons
    QPushButton *zoomInBtn;
    QPushButton *zoomOutBtn;
    QPushButton *resetZoomBtn;
    QPushButton *fitToWindowBtn;
    QVector<QPushButton*> zoomBtnGroup = {zoomInBtn, zoomOutBtn, resetZoomBtn, fitToWindowBtn};

    // Widget for zoom tools
    QWidget *zoomToolsWidget;
    bool zoomExpanded = false;
    int lastSelectedZoomBtn = -1; // Track which zoom button was last selected

    // Highlight the selected tool button
    void highlightButton(int toolId);

    // Highlight a zoom sub-button
    void highlightZoomButton(int btnIndex);

signals:
    // Zoom signals
    void zoomInRequested();
    void zoomOutRequested();
    void resetZoomRequested();
    void fitToWindowRequested();

    // Tool signals
    void dragToolRequested();
    void zoomToolRequested();

private:
    QButtonGroup *m_toolButtonGroup; // For mutually exclusive selection
};