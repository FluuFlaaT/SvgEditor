#pragma once

#include <QWidget>
#include <QVBoxLayout>
#include <QPushButton>
#include <QVector>
#include <QLabel>
#include <QLoggingCategory>

Q_DECLARE_LOGGING_CATEGORY(shapeToolBarLog)

// Enum for shape types
enum class ShapeType {
    None,
    Line,
    Freehand,
    Rectangle,
    Ellipse,
    Pentagon,
    Star,
    Hexagon,
    Text
};

class ShapeToolBar : public QWidget
{
    Q_OBJECT

public:
    explicit ShapeToolBar(QWidget *parent = nullptr);
    ~ShapeToolBar();

    // Shape tool buttons
    QPushButton *lineBtn;
    QPushButton *freehandBtn;
    QPushButton *rectangleBtn;
    QPushButton *ellipseBtn;
    QPushButton *pentagonBtn;
    QPushButton *starBtn;
    QPushButton *hexagonBtn;

    QVector<QPushButton**> shapeButtons = {
        &lineBtn, &freehandBtn, &rectangleBtn, &ellipseBtn,
        &pentagonBtn, &starBtn, &hexagonBtn
    };

    // Get the currently selected shape type
    ShapeType getSelectedShapeType() const { return m_selectedShapeType; }

    // Set the selected shape type
    void setSelectedShapeType(ShapeType type);

    // Highlight the selected button
    void highlightButton(ShapeType type);

signals:
    // Signal emitted when a shape tool is selected
    void shapeToolSelected(ShapeType type);

private:
    ShapeType m_selectedShapeType;
    QVBoxLayout *m_mainLayout;
};
