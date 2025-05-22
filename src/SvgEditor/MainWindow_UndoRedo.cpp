#include "MainWindow.h"

void MainWindow::undo()
{
    qCDebug(mainWindowLog) << "Undo requested";
    
    if (CommandManager::instance()->canUndo()) {
        if (CommandManager::instance()->undo()) {
            m_documentModified = true;
            updateTitle();
            showStatusMessage(tr("Undo: %1").arg(CommandManager::instance()->redoText()), 2000);
        } else {
            showStatusMessage(tr("Failed to undo"), 2000);
        }
    } else {
        showStatusMessage(tr("Nothing to undo"), 2000);
    }
    
    updateUndoRedoActions();
}

void MainWindow::redo()
{
    qCDebug(mainWindowLog) << "Redo requested";
    
    if (CommandManager::instance()->canRedo()) {
        if (CommandManager::instance()->redo()) {
            m_documentModified = true;
            updateTitle();
            showStatusMessage(tr("Redo: %1").arg(CommandManager::instance()->undoText()), 2000);
        } else {
            showStatusMessage(tr("Failed to redo"), 2000);
        }
    } else {
        showStatusMessage(tr("Nothing to redo"), 2000);
    }
    
    updateUndoRedoActions();
}

void MainWindow::updateUndoRedoActions()
{
    if (m_undoAction && m_redoAction) {
        m_undoAction->setEnabled(CommandManager::instance()->canUndo());
        m_redoAction->setEnabled(CommandManager::instance()->canRedo());
        
        if (CommandManager::instance()->canUndo()) {
            m_undoAction->setText(tr("Undo %1").arg(CommandManager::instance()->undoText()));
        } else {
            m_undoAction->setText(tr("Undo"));
        }
        
        if (CommandManager::instance()->canRedo()) {
            m_redoAction->setText(tr("Redo %1").arg(CommandManager::instance()->redoText()));
        } else {
            m_redoAction->setText(tr("Redo"));
        }
    }
}
