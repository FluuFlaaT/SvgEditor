#pragma once
#pragma execution_character_set("utf-8")
#include <QWidget>
#include <QVBoxLayout>
#include <QLabel>
#include <QScrollArea>
#include <QStackedWidget>

class RightAttrBar : public QWidget {
    Q_OBJECT

public:
    explicit RightAttrBar(QWidget *parent = nullptr);
    ~RightAttrBar();

    void showAttributesForTool(int toolId);
    void showAttributesForElement(int elementType);
    
    void changeEvent(QEvent* event) override;

private:
    QVBoxLayout* m_mainLayout;
    QLabel* m_titleLabel;
    QStackedWidget* m_stackedWidget;
    
    QWidget* createCommonAttributesWidget();
    QWidget* createCircleAttributesWidget();
    QWidget* createRectangleAttributesWidget();
    QWidget* createLineAttributesWidget();
};
