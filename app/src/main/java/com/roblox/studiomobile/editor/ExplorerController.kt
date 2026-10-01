package com.roblox.studiomobile.editor
import com.roblox.studiomobile.core.*
class ExplorerController(private val runtime:CoreRuntime){
 val model=ExplorerModel(runtime.dataModel)
 fun create(className:String,parent:Instance):Instance=runtime.factory.create(className).also{it.setParent(parent)}
 fun delete(instance:Instance){if(instance.parent!=null){val old=instance.parent;runtime.transactions.execute(object:Command{override fun execute(){instance.setParent(null)};override fun undo(){instance.setParent(old)}})};runtime.selection.remove(instance)}
 fun reparent(instance:Instance,newParent:Instance){val old=instance.parent;if(old===newParent)return;runtime.transactions.execute(object:Command{override fun execute(){instance.setParent(newParent)};override fun undo(){instance.setParent(old)}})}
 fun select(instance:Instance){runtime.selection.set(listOf(instance))}
}
