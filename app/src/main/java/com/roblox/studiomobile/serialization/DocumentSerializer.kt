package com.roblox.studiomobile.serialization
import com.roblox.studiomobile.core.*
data class SerializedInstance(val className:String,val name:String,val properties:Map<String,String>,val children:List<SerializedInstance>)
class DocumentSerializer(private val reflection:ReflectionRegistry,private val properties:PropertyStore){
 fun snapshot(root:Instance):SerializedInstance=serialize(root)
 private fun serialize(i:Instance):SerializedInstance{
  val props=reflection.get(i.className)?.all()?.associate{d->d.name to (properties.get<Any>(i,d.name)?.toString() ?: "")}?:emptyMap()
  return SerializedInstance(i.className,i.name,props,i.children.map(::serialize))
 }
}
