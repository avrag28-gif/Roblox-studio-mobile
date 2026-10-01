package com.roblox.studiomobile.editor
import com.roblox.studiomobile.core.*
class ExplorerController(private val runtime:CoreRuntime){
 val model=ExplorerModel(runtime.dataModel)
 fun create(className:String,parent:Instance):Instance=runtime.factory.create(className).also{it.setParent(parent)}
 fun delete(instance:Instance){instance.setParent(null);runtime.selection.remove(instance)}
 fun select(instance:Instance){runtime.selection.set(listOf(instance))}
}
