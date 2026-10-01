package com.roblox.studiomobile.editor
import com.roblox.studiomobile.core.*
class ExplorerController(private val runtime:CoreRuntime){
 val model=ExplorerModel(runtime.dataModel)
 fun create(className:String,parent:Instance):Instance{
  val instance=runtime.factory.create(className)
  runtime.transactions.execute(object:Command{
   override fun execute(){instance.setParent(parent)}
   override fun undo(){instance.setParent(null)}
  })
  return instance
 }
 fun delete(instance:Instance){
  val old=instance.parent?:return
  runtime.transactions.execute(object:Command{
   override fun execute(){instance.setParent(null)}
   override fun undo(){instance.setParent(old)}
  })
  runtime.selection.remove(instance)
 }
 fun reparent(instance:Instance,newParent:Instance){
  val old=instance.parent
  if(old===newParent)return
  runtime.transactions.execute(object:Command{
   override fun execute(){instance.setParent(newParent)}
   override fun undo(){instance.setParent(old)}
  })
 }
 fun rename(instance:Instance,newName:String){
  val old=instance.name
  if(old==newName)return
  runtime.transactions.execute(object:Command{
   override fun execute(){instance.rename(newName)}
   override fun undo(){instance.rename(old)}
  })
 }
 fun select(instance:Instance){runtime.selection.set(listOf(instance))}
}
