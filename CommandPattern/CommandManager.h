#ifndef COMMANDMANAGER_H
#define COMMANDMANAGER_H

#include <stack>
#include "ICommand.h"
#include <memory>

typedef std::stack<std::shared_ptr<ICommand>> commandStack;

class CommandManager {
private:
    commandStack undoStack;
    commandStack redoStack;
public:
    void execute(const std::shared_ptr<ICommand>& command);

    void undo();
    void redo();

    bool isUndoStackEmpty();
    bool isRedoStackEmpty();

    void emptyUndoStack();
    void emptyRedoStack();
};

#endif // COMMANDMANAGER_H
