#pragma once
#pragma execution_character_set("utf-8")
#include <QWidget>
#include <QVBoxLayout>
#include <QButtonGroup>
#include <QPushButton>
#include <QToolButton>

class LeftSideBar : public QWidget {
    Q_OBJECT

public:
    explicit LeftSideBar(QWidget *parent = nullptr);
    ~LeftSideBar();

signals:
    void toolSelected(int toolId);

private:
    QVBoxLayout* m_mainLayout;
    QButtonGroup* m_toolButtons;
    
    // 绘图工具
    QToolButton* createToolButton(const QString& text, const QString& iconName, int toolId);
};
