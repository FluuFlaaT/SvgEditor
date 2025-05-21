#include "LeftSideBar.h"
#include <QIcon>
#include <QVBoxLayout>
#include <QLabel>
#include <QSpacerItem>
#include <QEvent>
#include "LoggingService.h"

// 定义工具ID常量
enum ToolId {
    TOOL_SELECT = 1,
    TOOL_LINE,
    TOOL_RECTANGLE,
    TOOL_CIRCLE,
    TOOL_ELLIPSE,
    TOOL_POLYLINE,
    TOOL_PATH
};

LeftSideBar::LeftSideBar(QWidget *parent)
    : QWidget(parent)
{
    setMinimumWidth(100);
    setMaximumWidth(150);
    
    // 创建主布局
    m_mainLayout = new QVBoxLayout(this);
    m_mainLayout->setContentsMargins(5, 10, 5, 10);
    m_mainLayout->setSpacing(5);
    // 创建标题
    QLabel* titleLabel = new QLabel(tr("Drawing Tools"), this);
    titleLabel->setAlignment(Qt::AlignCenter);
    titleLabel->setStyleSheet("font-weight: bold;");
    m_mainLayout->addWidget(titleLabel);
    
    // 创建按钮组
    m_toolButtons = new QButtonGroup(this);
    m_toolButtons->setExclusive(true); // 只允许选择一个工具
    
    // 添加各种工具按钮
    m_mainLayout->addWidget(createToolButton(tr("Select"), "select", TOOL_SELECT));
    m_mainLayout->addWidget(createToolButton(tr("Line"), "line", TOOL_LINE));
    m_mainLayout->addWidget(createToolButton(tr("Rectangle"), "rectangle", TOOL_RECTANGLE));
    m_mainLayout->addWidget(createToolButton(tr("Circle"), "circle", TOOL_CIRCLE));
    m_mainLayout->addWidget(createToolButton(tr("Ellipse"), "ellipse", TOOL_ELLIPSE));
    m_mainLayout->addWidget(createToolButton(tr("Polyline"), "polyline", TOOL_POLYLINE));
    m_mainLayout->addWidget(createToolButton(tr("Path"), "path", TOOL_PATH));
    
    // 添加底部弹簧
    m_mainLayout->addStretch();
    
    // 连接信号
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
        // 重新翻译所有文本
        QList<QAbstractButton*> buttons = m_toolButtons->buttons();
        for (int i = 0; i < buttons.size(); i++) {
            QToolButton* button = qobject_cast<QToolButton*>(buttons[i]);
            if (button) {
                // 根据ID重设文本
                switch (m_toolButtons->id(button)) {
                    case TOOL_SELECT:
                        button->setText(tr("Select"));
                        button->setToolTip(tr("Select"));
                        break;
                    case TOOL_LINE:
                        button->setText(tr("Line"));
                        button->setToolTip(tr("Line"));
                        break;
                    case TOOL_RECTANGLE:
                        button->setText(tr("Rectangle"));
                        button->setToolTip(tr("Rectangle"));
                        break;
                    case TOOL_CIRCLE:
                        button->setText(tr("Circle"));
                        button->setToolTip(tr("Circle"));
                        break;
                    case TOOL_ELLIPSE:
                        button->setText(tr("Ellipse"));
                        button->setToolTip(tr("Ellipse"));
                        break;
                    case TOOL_POLYLINE:
                        button->setText(tr("Polyline"));
                        button->setToolTip(tr("Polyline"));
                        break;
                    case TOOL_PATH:
                        button->setText(tr("Path"));
                        button->setToolTip(tr("Path"));
                        break;
                }
            }
        }
        
        // 更新标题标签
        QLabel* titleLabel = findChild<QLabel*>();
        if (titleLabel) {
            titleLabel->setText(tr("Drawing Tools"));
        }
        
        LoggingService::getInstance().debug("LeftSideBar language changed");
    }
    
    QWidget::changeEvent(event);
}
