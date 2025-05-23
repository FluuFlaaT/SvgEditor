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
#include "shapetoolbar.h"

Q_DECLARE_LOGGING_CATEGORY(leftSideBarLog)

class LeftSideBar : public QWidget
{
    Q_OBJECT

public:
    explicit LeftSideBar(QWidget *parent = nullptr);
    ~LeftSideBar();

    // Main tool buttons (reordered according to requirements)
    QPushButton *dragBtn;
    QPushButton *shapeBtn;
    QPushButton *textBtn;
    QPushButton *zoomBtn;
    QVector<QPushButton**> btnGroup = {&dragBtn, &shapeBtn, &textBtn, &zoomBtn};

    // Shape sub-buttons
    QPushButton *lineBtn;
    QPushButton *freehandBtn;
    QPushButton *ellipseBtn;
    QPushButton *pentagonBtn;
    QPushButton *starBtn;
    QPushButton *hexagonBtn;
    QVector<QPushButton*> shapeBtnGroup = {lineBtn, freehandBtn, ellipseBtn, pentagonBtn, starBtn, hexagonBtn};

    // Zoom sub-buttons
    QPushButton *zoomInBtn;
    QPushButton *zoomOutBtn;
    QPushButton *resetZoomBtn;
    QPushButton *fitToWindowBtn;
    QVector<QPushButton*> zoomBtnGroup = {zoomInBtn, zoomOutBtn, resetZoomBtn, fitToWindowBtn};

    // Widgets for sub-tools
    QWidget *shapeToolsWidget;
    QWidget *zoomToolsWidget;
    bool shapeExpanded = false;
    bool zoomExpanded = false;
    int lastSelectedShapeBtn = -1; // Track which shape button was last selected
    int lastSelectedZoomBtn = -1; // Track which zoom button was last selected

    // Highlight the selected tool button
    void highlightButton(int toolId);

    // Highlight a shape sub-button
    void highlightShapeButton(int btnIndex);

    // Highlight a zoom sub-button
    void highlightZoomButton(int btnIndex);

    // Get the currently selected shape type
    ShapeType getSelectedShapeType() const { return m_selectedShapeType; }

    // Set the selected shape type
    void setSelectedShapeType(ShapeType type);

signals:
    // Shape signals
    void shapeToolSelected(ShapeType type);

    // Zoom signals
    void zoomInRequested();
    void zoomOutRequested();
    void resetZoomRequested();
    void fitToWindowRequested();

    // Tool signals
    void dragToolRequested();
    void shapeGroupRequested();
    void textToolRequested();
    void zoomToolRequested();

private:
    QButtonGroup *m_toolButtonGroup; // For mutually exclusive selection
    ShapeType m_selectedShapeType;
};