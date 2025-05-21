#pragma once
#pragma execution_character_set("utf-8")
#include <QGraphicsView>
#include <QGraphicsScene>
#include <QSvgRenderer>
#include <QGraphicsSvgItem>
#include <QResizeEvent>
#include <QEvent>
#include <QMouseEvent>
#include <QWheelEvent>
#include <QContextMenuEvent>
#include <QKeyEvent>
#include <QHash>
#include <QPointer>
#include <QColor>
#include "CoreSvgEngine.h"
#include "LoggingService.h"
#include "ToolId.h"

class CommandManager{}; // TODO: 还没实现，临时占位

class DrawingLayer : public QGraphicsView {
    Q_OBJECT

public:

    explicit DrawingLayer(CoreSvgEngine* engine, CommandManager* commandManager, 
                          QWidget *parent = nullptr);
    explicit DrawingLayer(QWidget *parent = nullptr);
    ~DrawingLayer();

    void loadDocument(SvgDocument* document);      // Loads all elements from the SvgDocument
    void clearCanvasAndMappings();                 // Clears scene items and internal SvgElement-QGraphicsItem mappings

    void setCurrentToolType(ToolId toolType);
    ToolId getCurrentToolType() const;
    void setZoomLevel(double level);               // e.g., 1.0 = 100%, 0.5 = 50%
    double getZoomLevel() const;

    void fitCanvasToView();                                                     // Adjusts view to show all content
    void ensureVisible(const QRectF& rect, int xmargin = 50, int ymargin = 50); // Ensures a rect is visible

    void changeEvent(QEvent* event) override;

    void setSvgContent(const QString& svgContent); // MAY BE DEPRECATED
    void updateView();                             // MAY BE DEPRECATED

signals:
    void selectionChanged(const QList<QGraphicsItem*>& selectedItems);

    void zoomLevelChanged(double newZoomLevel);

    // For tools like text insertion or specific point clicks
    void sceneClicked(const QPointF& scenePos, Qt::MouseButton button, Qt::KeyboardModifiers modifiers);
    // For tools needing drag information (start, current, end)
    void sceneDragStarted(const QPointF& scenePos, Qt::MouseButton button, Qt::KeyboardModifiers modifiers);
    void sceneDragging(const QPointF& scenePos, const QPointF& startScenePos, Qt::MouseButton button, Qt::KeyboardModifiers modifiers);
    void sceneDragEnded(const QPointF& scenePos, const QPointF& startScenePos, Qt::MouseButton button, Qt::KeyboardModifiers modifiers);

    // Item Modification signals (emitted after a command has likely modified an item)
    void graphicsItemAboutToBeModified(QGraphicsItem* item); // Could be useful for RightAttrBar to temporarily disable updates
    void graphicsItemModified(QGraphicsItem* item);          // E.g., after move/resize via handles

public slots:
    // Slots to React to SvgDocument Changes (from CoreSvgEngine via CommandManager)
    void handleElementAddedToDocument(SvgElement* element);
    void handleElementRemovedFromDocument(const QString& elementId); // Use ID as SvgElement might be deleted
    void handleElementModifiedInDocument(SvgElement* element);
    void handleDocumentCleared();
    void handleDocumentDimensionsChanged(double width, double height);
    void handleDocumentBackgroundColorChanged(const Color& coreColor);

    // Public Slots for Actions (e.g., from MainWindow menus/toolbars)
    void triggerSelectAll();
    void triggerDeleteSelected();
    void triggerCopySelected();
    void triggerPaste(); // Paste location might need context (e.g., center of view or last mouse pos)
    void triggerBringToFront();
    void triggerSendToBack();
    void triggerMoveForward();
    void triggerMoveBackward();
    // TODO: Add slots for grouping/ungrouping if implemented

protected:
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void wheelEvent(QWheelEvent *event) override;
    void contextMenuEvent(QContextMenuEvent *event) override;
    void keyPressEvent(QKeyEvent *event) override;
    void resizeEvent(QResizeEvent *event) override; // Useful for fitCanvasToView on resize if needed
    void showEvent(QShowEvent* event) override;     // Useful for initial fitCanvasToView

    void drawBackground(QPainter *painter, const QRectF &rect) override; // To draw canvas background color

    void changeEvent(QEvent* event) override; // For language changes, etc.

private:
    // Core Pointers & Scene
    QGraphicsScene* m_scene;                     //
    CoreSvgEngine* m_svgEngine;                 // Non-owning pointer to the shared engine
    QPointer<SvgDocument> m_currentDocument;     // Non-owning QPointer to the current SvgDocument
    CommandManager* m_commandManager;            // Non-owning pointer for undo/redo

    // Tool & Interaction State
    ToolId m_currentToolType;
    QPointF m_lastMouseScenePos;
    QPointF m_dragStartScenePos;
    QPointer<QGraphicsItem> m_previewItem;       // For visual feedback during drawing (e.g., rubber band rect)
    bool m_isDragging;                           // Flag to indicate if a drag operation is in progress

    // Item Mapping
    // SvgElement ID (std::string/QString) to QGraphicsItem*
    QHash<QString, QGraphicsItem*> m_idToItemMap;
    // QGraphicsItem* to SvgElement ID (std::string/QString)
    QHash<QGraphicsItem*, QString> m_itemToIdMap;

    // Canvas Properties Cache from SvgDocument
    QSizeF m_canvasSize;
    QColor m_canvasBackgroundColor;

    // Private Helper Methods
    void setupSceneDefaults();
    void connectDocumentSignals(SvgDocument* doc);
    void disconnectDocumentSignals(SvgDocument* doc);

    // SvgElement <-> QGraphicsItem conversion and management
    QGraphicsItem* createAndAddItemForElement(SvgElement* element);
    void updateItemFromElement(QGraphicsItem* item, SvgElement* element);
    void removeItemAndMapping(const QString& elementId);
    void updateAllItemsFromDocument(); // Full refresh if needed

    // Context menu creation
    void createContextMenu(QMenu& menu, QGraphicsItem* clickedItem);

    // Mouse event dispatching based on current tool
    void dispatchMousePressEvent(QMouseEvent* event);
    void dispatchMouseMoveEvent(QMouseEvent* event);
    void dispatchMouseReleaseEvent(QMouseEvent* event);

    // Specific tool interaction handlers
    // Example:
    void handleToolPress_Selection(const QPointF& scenePos, Qt::KeyboardModifiers modifiers);
    void handleToolMove_Selection(const QPointF& scenePos, Qt::KeyboardModifiers modifiers);
    void handleToolRelease_Selection(const QPointF& scenePos, Qt::KeyboardModifiers modifiers);

    void handleToolPress_Rectangle(const QPointF& scenePos, Qt::KeyboardModifiers modifiers);
    void handleToolMove_Rectangle(const QPointF& scenePos, Qt::KeyboardModifiers modifiers);
    void handleToolRelease_Rectangle(const QPointF& scenePos, Qt::KeyboardModifiers modifiers);

    void handleToolPress_Line(const QPointF& scenePos, Qt::KeyboardModifiers modifiers);
    void handleToolMove_Line(const QPointF& scenePos, Qt::KeyboardModifiers modifiers);
    void handleToolRelease_Line(const QPointF& scenePos, Qt::KeyboardModifiers modifiers);

    void handleToolPress_Circle(const QPointF& scenePos, Qt::KeyboardModifiers modifiers);
    void handleToolMove_Circle(const QPointF& scenePos, Qt::KeyboardModifiers modifiers);
    void handleToolRelease_Circle(const QPointF& scenePos, Qt::KeyboardModifiers modifiers);

    void handleToolPress_Ellipse(const QPointF& scenePos, Qt::KeyboardModifiers modifiers);
    void handleToolMove_Ellipse(const QPointF& scenePos, Qt::KeyboardModifiers modifiers);
    void handleToolRelease_Ellipse(const QPointF& scenePos, Qt::KeyboardModifiers modifiers);

    void handleToolPress_Polyline(const QPointF& scenePos, Qt::KeyboardModifiers modifiers);
    void handleToolMove_Polyline(const QPointF& scenePos, Qt::KeyboardModifiers modifiers);
    void handleToolRelease_Polyline(const QPointF& scenePos, Qt::KeyboardModifiers modifiers);

    void handleToolPress_Path(const QPointF& scenePos, Qt::KeyboardModifiers modifiers);
    void handleToolMove_Path(const QPointF& scenePos, Qt::KeyboardModifiers modifiers);
    void handleToolRelease_Path(const QPointF& scenePos, Qt::KeyboardModifiers modifiers);

    // Utility to convert CoreSvg::Color to QColor and vice-versa
    QColor toQColor(const Color& coreColor) const;
    Color toCoreColor(const QColor& qColor) const;

    QGraphicsScene* m_scene;
    
};
