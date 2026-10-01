package com.roblox.studiomobile.editor
import com.roblox.studiomobile.core.*
data class PropertyRow(val name:String,val type:PropertyType,val value:Any?)
class PropertiesModel(private val reflection:ReflectionRegistry,private val properties:PropertyStore){
 fun rows(instance:Instance):List<PropertyRow>{return reflection.get(instance.className)?.all()?.map{PropertyRow(it.name,it.type,properties.get<Any>(instance,it.name))}?:emptyList()}
}
