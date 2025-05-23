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


class EditableTextItem : public QGraphicsTextItem
{
    Q_OBJECT

public:
    explicit EditableTextItem(const QString& text = QString(), QGraphicsItem* parent = nullptr);

    QString toPlainString() const;

    void setTextAlignment(Qt::Alignment alignment);

    Qt::Alignment textAlignment() const;

    void setBold(bool bold);

    bool isBold() const;

    void setItalic(bool italic);

    bool isItalic() const;

    void startEditing();

signals:
    void textChanged(const QString& text);
    void textChangedWithHistory(const QString& oldText, const QString& newText);
    void editingFinished();

protected:
    void mouseDoubleClickEvent(QGraphicsSceneMouseEvent* event) override;

    void focusOutEvent(QFocusEvent* event) override;

    void keyPressEvent(QKeyEvent* event) override;

private:
    void finishEditing();

    bool m_isEditing;
    Qt::Alignment m_textAlignment;
    QString m_originalText; // Store original text when editing starts
};
