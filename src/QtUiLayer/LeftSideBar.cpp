#include "LeftSideBar.h"
#include "ToolId.h" // 包含新的头文件

LeftSideBar::LeftSideBar(QWidget *parent)
    : QWidget(parent)
{
    setMinimumWidth(100);
    setMaximumWidth(150);
    
    m_mainLayout = new QVBoxLayout(this);
    m_mainLayout->setContentsMargins(5, 10, 5, 10);
    m_mainLayout->setSpacing(5);

    /* 原来有个标题放在上面，现在没必要了
    QLabel* titleLabel = new QLabel(tr(""), this);
    titleLabel->setAlignment(Qt::AlignCenter);
    titleLabel->setStyleSheet("font-weight: bold;");
    m_mainLayout->addWidget(titleLabel);
    */
    
    // 创建按钮组
    m_toolButtons = new QButtonGroup(this);
    m_toolButtons->setExclusive(true); // 只允许选择一个工具
    
    m_mainLayout->addWidget(createToolButton(tr("Select"), "select", static_cast<int>(ToolId::TOOL_SELECT)));
    m_mainLayout->addWidget(createToolButton(tr("Line"), "line", static_cast<int>(ToolId::TOOL_LINE)));
    m_mainLayout->addWidget(createToolButton(tr("Rectangle"), "rectangle", static_cast<int>(ToolId::TOOL_RECTANGLE)));
    m_mainLayout->addWidget(createToolButton(tr("Circle"), "circle", static_cast<int>(ToolId::TOOL_CIRCLE)));
    m_mainLayout->addWidget(createToolButton(tr("Ellipse"), "ellipse", static_cast<int>(ToolId::TOOL_ELLIPSE)));
    m_mainLayout->addWidget(createToolButton(tr("Polyline"), "polyline", static_cast<int>(ToolId::TOOL_POLYLINE)));
    m_mainLayout->addWidget(createToolButton(tr("Path"), "path", static_cast<int>(ToolId::TOOL_PATH)));
    
    m_mainLayout->addStretch(); // 底部缓冲
    
    connect(m_toolButtons, QOverload<int>::of(&QButtonGroup::buttonClicked), 
        [this](int id) {
            LoggingService::getInstance().debug("Tool selected: " + std::to_string(id));
            emit toolSelected(id);
        });
    
    LoggingService::getInstance().debug("LeftSideBar constructed");
}

LeftSideBar::~LeftSideBar()
{
    LoggingService::getInstance().debug("LeftSideBar destroyed");
}

QToolButton* LeftSideBar::createToolButton(const QString& text, const QString& iconName, int toolId)
{
    QToolButton* button = new QToolButton(this);
    button->setText(text);
    button->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
    button->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
    button->setMinimumHeight(60);
    button->setCheckable(true);
    
    // 添加工具提示，在鼠标悬停时显示完整文本
    button->setToolTip(text);
    
    // 注意：这里只是占位，实际开发中应该加载真实图标
    // button->setIcon(QIcon(":/icons/" + iconName + ".png"));
    
    m_toolButtons->addButton(button, toolId);
    
    return button;
}

void LeftSideBar::changeEvent(QEvent* event)
{
    if (event->type() == QEvent::LanguageChange) {
        QList<QAbstractButton*> buttons = m_toolButtons->buttons();
        for (int i = 0; i < buttons.size(); i++) {
            QToolButton* button = qobject_cast<QToolButton*>(buttons[i]);
            if (button) {
                switch (static_cast<ToolId>(m_toolButtons->id(button))) { // 使用强类型枚举
                    case ToolId::TOOL_SELECT:
                        button->setText(tr("Select"));
                        button->setToolTip(tr("Select"));
                        break;
                    case ToolId::TOOL_LINE:
                        button->setText(tr("Line"));
                        button->setToolTip(tr("Line"));
                        break;
                    case ToolId::TOOL_RECTANGLE:
                        button->setText(tr("Rectangle"));
                        button->setToolTip(tr("Rectangle"));
                        break;
                    case ToolId::TOOL_CIRCLE:
                        button->setText(tr("Circle"));
                        button->setToolTip(tr("Circle"));
                        break;
                    case ToolId::TOOL_ELLIPSE:
                        button->setText(tr("Ellipse"));
                        button->setToolTip(tr("Ellipse"));
                        break;
                    case ToolId::TOOL_POLYLINE:
                        button->setText(tr("Polyline"));
                        button->setToolTip(tr("Polyline"));
                        break;
                    case ToolId::TOOL_PATH:
                        button->setText(tr("Path"));
                        button->setToolTip(tr("Path"));
                        break;
                }
            }
        }

        LoggingService::getInstance().debug("LeftSideBar language changed");
    }
    
    QWidget::changeEvent(event);
}
