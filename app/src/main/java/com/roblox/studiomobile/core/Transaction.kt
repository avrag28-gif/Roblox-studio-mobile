package com.roblox.studiomobile.core
interface Command{fun execute();fun undo()}
class TransactionManager{
 private val undoStack=ArrayDeque<Command>()
 private val redoStack=ArrayDeque<Command>()
 val changed=Signal<Unit>()
 fun execute(c:Command){c.execute();undoStack.addLast(c);redoStack.clear();changed.fire(Unit)}
 fun undo(){if(undoStack.isNotEmpty()){val c=undoStack.removeLast();c.undo();redoStack.addLast(c);changed.fire(Unit)}}
 fun redo(){if(redoStack.isNotEmpty()){val c=redoStack.removeLast();c.execute();undoStack.addLast(c);changed.fire(Unit)}}
 fun canUndo()=undoStack.isNotEmpty()
 fun canRedo()=redoStack.isNotEmpty()
}
