#pragma once

#include <QGraphicsTextItem>
#include <QTextDocument>
#include <QTextCursor>
#include <QKeyEvent>
#include <QFocusEvent>
#include <QInputDialog>
#include <QLoggingCategory>

// Forward declarations
class QGraphicsSceneMouseEvent;

/**
 * @brief A custom QGraphicsTextItem that supports in-place editing
 * 
 * This class extends QGraphicsTextItem to provide in-place text editing
 * functionality. It allows users to edit text directly on the canvas by
 * double-clicking on the text item.
 */
class EditableTextItem : public QGraphicsTextItem
{
    Q_OBJECT

public:
    /**
     * @brief Constructor
     * @param text The initial text content
     * @param parent The parent item
     */
    explicit EditableTextItem(const QString& text = QString(), QGraphicsItem* parent = nullptr);

    /**
     * @brief Get the plain text content
     * @return The text content as a QString
     */
    QString toPlainString() const;

    /**
     * @brief Set the text alignment
     * @param alignment The text alignment (Qt::AlignLeft, Qt::AlignCenter, Qt::AlignRight)
     */
    void setTextAlignment(Qt::Alignment alignment);

    /**
     * @brief Get the current text alignment
     * @return The current text alignment
     */
    Qt::Alignment textAlignment() const;

    /**
     * @brief Set the font weight (bold)
     * @param bold True to set bold, false to set normal
     */
    void setBold(bool bold);

    /**
     * @brief Check if the text is bold
     * @return True if the text is bold, false otherwise
     */
    bool isBold() const;

    /**
     * @brief Set the font style (italic)
     * @param italic True to set italic, false to set normal
     */
    void setItalic(bool italic);

    /**
     * @brief Check if the text is italic
     * @return True if the text is italic, false otherwise
     */
    bool isItalic() const;

signals:
    /**
     * @brief Signal emitted when the text content changes
     * @param text The new text content
     */
    void textChanged(const QString& text);

    /**
     * @brief Signal emitted when editing is finished
     */
    void editingFinished();

protected:
    /**
     * @brief Handle mouse double-click events to start editing
     * @param event The mouse event
     */
    void mouseDoubleClickEvent(QGraphicsSceneMouseEvent* event) override;

    /**
     * @brief Handle focus out events to finish editing
     * @param event The focus event
     */
    void focusOutEvent(QFocusEvent* event) override;

    /**
     * @brief Handle key press events
     * @param event The key event
     */
    void keyPressEvent(QKeyEvent* event) override;

private:
    /**
     * @brief Start editing the text
     */
    void startEditing();

    /**
     * @brief Finish editing the text
     */
    void finishEditing();

    bool m_isEditing;
    Qt::Alignment m_textAlignment;
};
