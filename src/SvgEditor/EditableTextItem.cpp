#include "editabletextitem.h"
#include <QGraphicsSceneMouseEvent>
#include <QTextDocument>
#include <QTextCursor>
#include <QTextBlock>
#include <QDebug>

// Create logging category
Q_DECLARE_LOGGING_CATEGORY(editableTextItemLog)
Q_LOGGING_CATEGORY(editableTextItemLog, "EditableTextItem")

EditableTextItem::EditableTextItem(const QString& text, QGraphicsItem* parent)
    : QGraphicsTextItem(text, parent),
      m_isEditing(false),
      m_textAlignment(Qt::AlignLeft)
{
    // Make the item selectable and movable
    setFlag(QGraphicsItem::ItemIsSelectable, true);
    setFlag(QGraphicsItem::ItemIsMovable, true);
    setFlag(QGraphicsItem::ItemIsFocusable, true);

    // Set default text color
    setDefaultTextColor(Qt::black);

    // Set default font
    QFont font("Arial", 12);
    setFont(font);

    // Set default text options
    QTextOption textOption;
    textOption.setAlignment(m_textAlignment);
    document()->setDefaultTextOption(textOption);

    qCDebug(editableTextItemLog) << "Created EditableTextItem with text:" << text;
}

QString EditableTextItem::toPlainString() const
{
    return toPlainText();
}

void EditableTextItem::setTextAlignment(Qt::Alignment alignment)
{
    m_textAlignment = alignment;

    QTextOption textOption = document()->defaultTextOption();
    textOption.setAlignment(alignment);
    document()->setDefaultTextOption(textOption);

    // Force update
    update();

    qCDebug(editableTextItemLog) << "Text alignment set to:" << alignment;
}

Qt::Alignment EditableTextItem::textAlignment() const
{
    return m_textAlignment;
}

void EditableTextItem::setBold(bool bold)
{
    QTextCursor cursor = textCursor();
    cursor.select(QTextCursor::Document);

    QFont font = this->font();
    font.setBold(bold);
    setFont(font);

    // Apply to selected text
    QTextCharFormat format;
    format.setFontWeight(bold ? QFont::Bold : QFont::Normal);
    cursor.mergeCharFormat(format);

    // Force update
    update();

    qCDebug(editableTextItemLog) << "Bold set to:" << bold;
}

bool EditableTextItem::isBold() const
{
    return font().bold();
}

void EditableTextItem::setItalic(bool italic)
{
    QTextCursor cursor = textCursor();
    cursor.select(QTextCursor::Document);

    QFont font = this->font();
    font.setItalic(italic);
    setFont(font);

    // Apply to selected text
    QTextCharFormat format;
    format.setFontItalic(italic);
    cursor.mergeCharFormat(format);

    // Force update
    update();

    qCDebug(editableTextItemLog) << "Italic set to:" << italic;
}

bool EditableTextItem::isItalic() const
{
    return font().italic();
}

void EditableTextItem::mouseDoubleClickEvent(QGraphicsSceneMouseEvent* event)
{
    if (!m_isEditing) {
        startEditing();
        event->accept();
    } else {
        QGraphicsTextItem::mouseDoubleClickEvent(event);
    }
}

void EditableTextItem::focusOutEvent(QFocusEvent* event)
{
    if (m_isEditing) {
        finishEditing();
    }
    QGraphicsTextItem::focusOutEvent(event);
}

void EditableTextItem::keyPressEvent(QKeyEvent* event)
{
    if (m_isEditing) {
        if (event->key() == Qt::Key_Return && (event->modifiers() & Qt::ControlModifier)) {
            // Ctrl+Enter finishes editing
            finishEditing();
            event->accept();
            return;
        } else if (event->key() == Qt::Key_Escape) {
            // Escape cancels editing
            finishEditing();
            event->accept();
            return;
        }
    }

    QGraphicsTextItem::keyPressEvent(event);
}

void EditableTextItem::startEditing()
{
    m_isEditing = true;
    setTextInteractionFlags(Qt::TextEditorInteraction);
    setFocus();

    // Select all text to make it easy to replace
    QTextCursor cursor = textCursor();
    cursor.select(QTextCursor::Document);
    setTextCursor(cursor);

    qCDebug(editableTextItemLog) << "Started editing text:" << toPlainText();
}

void EditableTextItem::finishEditing()
{
    m_isEditing = false;
    setTextInteractionFlags(Qt::NoTextInteraction);
    clearFocus();

    // Emit signal that text has changed
    emit textChanged(toPlainText());
    emit editingFinished();

    qCDebug(editableTextItemLog) << "Finished editing text:" << toPlainText();
}
