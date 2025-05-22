#pragma once

#include <QWidget>
#include <QVBoxLayout>
#include <QLabel>
#include <QStackedWidget>
#include <QLoggingCategory>
#include <QColor>
#include <QColorDialog>
#include <QSpinBox>
#include <QPushButton>
#include <QLineEdit>
#include <QComboBox>
#include <QVariant>
#include <QPen>

Q_DECLARE_LOGGING_CATEGORY(rightAttrBarLog)
class RightAttrBar : public QWidget
{
    Q_OBJECT

public:
    explicit RightAttrBar(QWidget *parent = nullptr);
    ~RightAttrBar();

public slots:
    // Add future slots here

private:
    QVBoxLayout* m_mainLayout;
    QStackedWidget* m_stackedWidget;

    QWidget* createCommonAttributesWidget();
    QWidget* createCircleAttributesWidget();
    QWidget* createRectangleAttributesWidget();
    QWidget* createLineAttributesWidget();
};