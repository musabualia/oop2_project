#pragma once
#include "ICommand.h"
#include <memory>
#include <stack>
#include <vector>
#include <string>

class CommandManager {
public:
    CommandManager() = default;                 // Default constructor
    ~CommandManager() = default;                // Default destructor

    CommandManager(const CommandManager&) = delete;             // Disable copy constructor
    CommandManager& operator=(const CommandManager&) = delete;  // Disable assignment operator

    void executeCommand(std::unique_ptr<ICommand> command);     // Execute a command and store for undo
    void undo();                                                // Undo the last command
    void redo();                                                // Redo the last undone command
    bool canUndo() const;                                       // Check if undo is possible
    bool canRedo() const;                                       // Check if redo is possible
    void clear();                                               // Clear undo/redo history
    size_t getUndoCount() const;                                // Get number of undoable commands
    size_t getRedoCount() const;                                // Get number of redoable commands
    std::string getNextUndoDescription() const;                // Get description of next undo command
    std::string getNextRedoDescription() const;                // Get description of next redo command
    void setMaxHistory(size_t maxHistory);                     // Set max number of commands in history

private:
    std::stack<std::unique_ptr<ICommand>> m_undoStack;         // Stack for undo commands
    std::stack<std::unique_ptr<ICommand>> m_redoStack;         // Stack for redo commands
    size_t m_maxHistory = 50;                                  // Maximum number of commands to keep

    void clearRedoStack();                                     // Clear redo stack
};
