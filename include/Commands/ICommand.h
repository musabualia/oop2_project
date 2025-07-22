#pragma once
#include <memory>
#include <string>

class ICommand {
public:
    virtual ~ICommand() = default;                         // Virtual destructor

    virtual void execute() = 0;                            // Execute the command
    virtual void undo() = 0;                               // Undo the command
    virtual bool canUndo() const { return true; }          // Return if command can be undone
    virtual std::string getDescription() const { return "Command"; }  // Return command description
    virtual bool wasExecuted() const { return m_executed; }           // Check if command was executed

protected:
    bool m_executed = false;                               // Track if command was executed
};
