package com.roblox.studiomobile.core

interface Command {
    fun execute()
    fun undo()
}

class CompositeCommand(private val commands: List<Command>) : Command {
    override fun execute() = commands.forEach { it.execute() }
    override fun undo() = commands.asReversed().forEach { it.undo() }
}

class TransactionManager {
    private val undoStack = ArrayDeque<Command>()
    private val redoStack = ArrayDeque<Command>()
    private var batch: MutableList<Command>? = null
    val changed = Signal<Unit>()

    fun execute(c: Command) {
        c.execute()
        val active = batch
        if (active != null) {
            active += c
        } else {
            undoStack.addLast(c)
            redoStack.clear()
            changed.fire(Unit)
        }
    }

    fun beginBatch() {
        check(batch == null) { "Transaction batch already active" }
        batch = mutableListOf()
    }

    fun endBatch(commit: Boolean = true) {
        val active = batch ?: return
        batch = null
        if (active.isEmpty()) return
        if (commit) {
            undoStack.addLast(CompositeCommand(active.toList()))
            redoStack.clear()
        } else {
            active.asReversed().forEach { it.undo() }
        }
        changed.fire(Unit)
    }

    fun undo() {
        if (undoStack.isNotEmpty()) {
            val c = undoStack.removeLast()
            c.undo()
            redoStack.addLast(c)
            changed.fire(Unit)
        }
    }

    fun redo() {
        if (redoStack.isNotEmpty()) {
            val c = redoStack.removeLast()
            c.execute()
            undoStack.addLast(c)
            changed.fire(Unit)
        }
    }

    fun canUndo() = undoStack.isNotEmpty()
    fun canRedo() = redoStack.isNotEmpty()
}
