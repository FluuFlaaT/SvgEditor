#pragma once
#include "Command.h"
#include <QGraphicsItem>
#include <QString>
#include <QColor>
#include <QFont>
#include <memory>
#include "SvgEditorForwards.h"
#include "SvgEditor/editabletextitem.h"

enum class TextModificationType {
    Content,
    FontFamily,
    FontSize,
    FontBold,
    FontItalic,
    TextAlignment,
    TextColor
};

class ModifyTextCommand : public Command {
public:
    // Constructor for text content modification
    ModifyTextCommand(EditableTextItem* textItem, const QString& oldContent, const QString& newContent);
    
    // Constructor for font family modification
    ModifyTextCommand(EditableTextItem* textItem, const QString& oldFamily, const QString& newFamily, TextModificationType type);
    
    // Constructor for font size modification
    ModifyTextCommand(EditableTextItem* textItem, int oldSize, int newSize);
    
    // Constructor for font style modification (bold/italic)
    ModifyTextCommand(EditableTextItem* textItem, bool oldValue, bool newValue, TextModificationType type);
    
    // Constructor for text alignment modification
    ModifyTextCommand(EditableTextItem* textItem, Qt::Alignment oldAlignment, Qt::Alignment newAlignment);
    
    // Constructor for text color modification
    ModifyTextCommand(EditableTextItem* textItem, const QColor& oldColor, const QColor& newColor);
    
    ~ModifyTextCommand() override = default;

    bool execute() override;
    bool undo() override;

private:
    EditableTextItem* m_textItem;
    TextModificationType m_modificationType;
    
    // Store old and new values for different types
    QString m_oldStringValue;
    QString m_newStringValue;
    int m_oldIntValue;
    int m_newIntValue;
    bool m_oldBoolValue;
    bool m_newBoolValue;
    Qt::Alignment m_oldAlignment;
    Qt::Alignment m_newAlignment;
    QColor m_oldColor;
    QColor m_newColor;
    
    void applyModification(bool isUndo);
}; 