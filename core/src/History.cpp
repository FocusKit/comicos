#include "core/History.h"

namespace comicos {

History::History(size_t maxMemoryBytes) : m_maxMemory(maxMemoryBytes) {}
History::~History() = default;

void History::push(std::unique_ptr<HistoryCommand> command) {
    // Execute the action (redo) and push onto undo stack
    command->redo();
    m_currentMemory += command->memoryUsage();
    m_undoStack.push_back(std::move(command));

    // Clear redo stack (new branch)
    for (auto& cmd : m_redoStack) {
        m_currentMemory -= cmd->memoryUsage();
    }
    m_redoStack.clear();

    trimToMemoryLimit();
}

void History::undo() {
    if (!canUndo()) return;

    auto command = std::move(m_undoStack.back());
    m_undoStack.pop_back();
    command->undo();
    m_redoStack.push_back(std::move(command));
}

void History::redo() {
    if (!canRedo()) return;

    auto command = std::move(m_redoStack.back());
    m_redoStack.pop_back();
    command->redo();
    m_undoStack.push_back(std::move(command));
}

bool History::canUndo() const {
    return !m_undoStack.empty();
}

bool History::canRedo() const {
    return !m_redoStack.empty();
}

void History::clear() {
    m_undoStack.clear();
    m_redoStack.clear();
    m_currentMemory = 0;
}

void History::trimToMemoryLimit() {
    while (m_currentMemory > m_maxMemory && !m_undoStack.empty()) {
        m_currentMemory -= m_undoStack.front()->memoryUsage();
        m_undoStack.erase(m_undoStack.begin());
    }
}

}  // namespace comicos
