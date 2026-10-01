package com.roblox.studiomobile.core
interface Command{fun execute();fun undo()};class TransactionManager{private val u=ArrayDeque<Command>();private val r=ArrayDeque<Command>();fun execute(c:Command){c.execute();u.addLast(c);r.clear()};fun undo(){if(u.isNotEmpty()){val c=u.removeLast();c.undo();r.addLast(c)}};fun redo(){if(r.isNotEmpty()){val c=r.removeLast();c.execute();u.addLast(c)}}}
