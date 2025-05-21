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
    
    // 语言更换时调用
    void changeEvent(QEvent* event) override;

signals:
    void toolSelected(int toolId);

private:
    QVBoxLayout* m_mainLayout;
    QButtonGroup* m_toolButtons;

    QToolButton* createToolButton(const QString& text, const QString& iconName, int toolId);
};
