// ================================
// src/Commands/CommandManager.cpp
// ================================
#include "Commands/CommandManager.h"

void CommandManager::executeCommand(std::unique_ptr<ICommand> command) {
    if (!command) {
        return;
    }

    command->execute();

    if (command->wasExecuted()) {
        m_undoStack.push(std::move(command));
        clearRedoStack();
    }
}

void CommandManager::undo() {
    if (!canUndo()) {
        return;
    }

    std::unique_ptr<ICommand> command = std::move(m_undoStack.top());
    m_undoStack.pop();

    command->undo();
    m_redoStack.push(std::move(command));
}

void CommandManager::redo() {
    if (!canRedo()) {
        return;
    }

    std::unique_ptr<ICommand> command = std::move(m_redoStack.top());
    m_redoStack.pop();

    command->execute();

    if (command->wasExecuted()) {
        m_undoStack.push(std::move(command));
    }
}

bool CommandManager::canUndo() const {
    return !m_undoStack.empty() &&
        m_undoStack.top() &&
        m_undoStack.top()->canUndo();
}

bool CommandManager::canRedo() const {
    return !m_redoStack.empty() &&
        m_redoStack.top();
}

void CommandManager::clear() {
    while (!m_undoStack.empty()) {
        m_undoStack.pop();
    }
    clearRedoStack();
}

size_t CommandManager::getUndoCount() const {
    return m_undoStack.size();
}

size_t CommandManager::getRedoCount() const {
    return m_redoStack.size();
}

std::string CommandManager::getNextUndoDescription() const {
    if (canUndo()) {
        return m_undoStack.top()->getDescription();
    }
    return "";
}

std::string CommandManager::getNextRedoDescription() const {
    if (canRedo()) {
        return m_redoStack.top()->getDescription();
    }
    return "";
}

void CommandManager::setMaxHistory(size_t maxHistory) {
    m_maxHistory = maxHistory;
}

void CommandManager::clearRedoStack() {
    while (!m_redoStack.empty()) {
        m_redoStack.pop();
    }
}
