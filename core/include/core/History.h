#pragma once

#include <functional>
#include <memory>
#include <vector>

namespace comicos {

/// Abstract base for undoable commands.
class HistoryCommand {
public:
    virtual ~HistoryCommand() = default;
    virtual void undo() = 0;
    virtual void redo() = 0;

    /// Memory footprint estimate (for limiting history size).
    virtual size_t memoryUsage() const { return 0; }
};

/// Undo/redo history manager.
/// Uses the command pattern. Each action pushes a command that knows
/// how to undo and redo itself. Supports memory-based limiting.
class History {
public:
    explicit History(size_t maxMemoryBytes = 256 * 1024 * 1024);  // 256MB default
    ~History();

    // --- Operations ---
    void push(std::unique_ptr<HistoryCommand> command);
    void undo();
    void redo();

    bool canUndo() const;
    bool canRedo() const;

    void clear();

    // --- State ---
    int undoCount() const { return static_cast<int>(m_undoStack.size()); }
    int redoCount() const { return static_cast<int>(m_redoStack.size()); }
    size_t memoryUsage() const { return m_currentMemory; }

    // Extension point: history coalescing (merge rapid small strokes)
    // void setCoalesceWindow(int ms);

    // Extension point: named snapshots / save points
    // void markSavePoint();
    // bool isAtSavePoint() const;

private:
    void trimToMemoryLimit();

    std::vector<std::unique_ptr<HistoryCommand>> m_undoStack;
    std::vector<std::unique_ptr<HistoryCommand>> m_redoStack;
    size_t m_maxMemory;
    size_t m_currentMemory = 0;
};

}  // namespace comicos
