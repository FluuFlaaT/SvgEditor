#include "ModifyTextCommand.h"
#include <QLoggingCategory>

Q_LOGGING_CATEGORY(modifyTextCommandLog, "ModifyTextCommand")

// Constructor for text content modification
ModifyTextCommand::ModifyTextCommand(EditableTextItem* textItem, const QString& oldContent, const QString& newContent)
    : Command(QObject::tr("Modify Text Content")),
      m_textItem(textItem),
      m_modificationType(TextModificationType::Content),
      m_oldStringValue(oldContent),
      m_newStringValue(newContent)
{
    qCDebug(modifyTextCommandLog) << "ModifyTextCommand created for content change:" << oldContent << "->" << newContent;
}

// Constructor for font family modification
ModifyTextCommand::ModifyTextCommand(EditableTextItem* textItem, const QString& oldFamily, const QString& newFamily, TextModificationType type)
    : Command(QObject::tr("Modify Font Family")),
      m_textItem(textItem),
      m_modificationType(type),
      m_oldStringValue(oldFamily),
      m_newStringValue(newFamily)
{
    qCDebug(modifyTextCommandLog) << "ModifyTextCommand created for font family change:" << oldFamily << "->" << newFamily;
}

// Constructor for font size modification
ModifyTextCommand::ModifyTextCommand(EditableTextItem* textItem, int oldSize, int newSize)
    : Command(QObject::tr("Modify Font Size")),
      m_textItem(textItem),
      m_modificationType(TextModificationType::FontSize),
      m_oldIntValue(oldSize),
      m_newIntValue(newSize)
{
    qCDebug(modifyTextCommandLog) << "ModifyTextCommand created for font size change:" << oldSize << "->" << newSize;
}

// Constructor for font style modification (bold/italic)
ModifyTextCommand::ModifyTextCommand(EditableTextItem* textItem, bool oldValue, bool newValue, TextModificationType type)
    : Command(type == TextModificationType::FontBold ? QObject::tr("Modify Font Bold") : QObject::tr("Modify Font Italic")),
      m_textItem(textItem),
      m_modificationType(type),
      m_oldBoolValue(oldValue),
      m_newBoolValue(newValue)
{
    qCDebug(modifyTextCommandLog) << "ModifyTextCommand created for font style change:" << oldValue << "->" << newValue;
}

// Constructor for text alignment modification
ModifyTextCommand::ModifyTextCommand(EditableTextItem* textItem, Qt::Alignment oldAlignment, Qt::Alignment newAlignment)
    : Command(QObject::tr("Modify Text Alignment")),
      m_textItem(textItem),
      m_modificationType(TextModificationType::TextAlignment),
      m_oldAlignment(oldAlignment),
      m_newAlignment(newAlignment)
{
    qCDebug(modifyTextCommandLog) << "ModifyTextCommand created for alignment change:" << static_cast<int>(oldAlignment) << "->" << static_cast<int>(newAlignment);
}

// Constructor for text color modification
ModifyTextCommand::ModifyTextCommand(EditableTextItem* textItem, const QColor& oldColor, const QColor& newColor)
    : Command(QObject::tr("Modify Text Color")),
      m_textItem(textItem),
      m_modificationType(TextModificationType::TextColor),
      m_oldColor(oldColor),
      m_newColor(newColor)
{
    qCDebug(modifyTextCommandLog) << "ModifyTextCommand created for color change:" << oldColor.name() << "->" << newColor.name();
}

bool ModifyTextCommand::execute()
{
    if (!m_textItem) {
        qCWarning(modifyTextCommandLog) << "Cannot execute ModifyTextCommand: text item is null";
        return false;
    }

    qCDebug(modifyTextCommandLog) << "Executing ModifyTextCommand";
    applyModification(false);
    return true;
}

bool ModifyTextCommand::undo()
{
    if (!m_textItem) {
        qCWarning(modifyTextCommandLog) << "Cannot undo ModifyTextCommand: text item is null";
        return false;
    }

    qCDebug(modifyTextCommandLog) << "Undoing ModifyTextCommand";
    applyModification(true);
    return true;
}

void ModifyTextCommand::applyModification(bool isUndo)
{
    switch (m_modificationType) {
        case TextModificationType::Content:
            m_textItem->setPlainText(isUndo ? m_oldStringValue : m_newStringValue);
            break;
            
        case TextModificationType::FontFamily: {
            QFont font = m_textItem->font();
            font.setFamily(isUndo ? m_oldStringValue : m_newStringValue);
            m_textItem->setFont(font);
            break;
        }
        
        case TextModificationType::FontSize: {
            QFont font = m_textItem->font();
            font.setPointSize(isUndo ? m_oldIntValue : m_newIntValue);
            m_textItem->setFont(font);
            break;
        }
        
        case TextModificationType::FontBold:
            m_textItem->setBold(isUndo ? m_oldBoolValue : m_newBoolValue);
            break;
            
        case TextModificationType::FontItalic:
            m_textItem->setItalic(isUndo ? m_oldBoolValue : m_newBoolValue);
            break;
            
        case TextModificationType::TextAlignment:
            m_textItem->setTextAlignment(isUndo ? m_oldAlignment : m_newAlignment);
            break;
            
        case TextModificationType::TextColor:
            m_textItem->setDefaultTextColor(isUndo ? m_oldColor : m_newColor);
            break;
    }
    
    // Emit signal to update the document
    if (!isUndo) {
        emit m_textItem->textChanged(m_textItem->toPlainString());
    }
} 