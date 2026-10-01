package com.roblox.studiomobile.core
enum class PropertyType{Bool,Int,Float,Double,String,Enum,Vector2,Vector3,CFrame,Color3,InstanceReference}
data class PropertyDescriptor<T>(val name:String,val type:PropertyType,val defaultValue:T,val writable:Boolean=true)
class ClassDescriptor(val name:String){
 private val properties=linkedMapOf<String,PropertyDescriptor<*>>()
 fun <T>property(d:PropertyDescriptor<T>):ClassDescriptor{properties[d.name]=d;return this}
 fun get(n:String):PropertyDescriptor<*>?=properties[n]
 fun all()=properties.values.toList()
}
class ReflectionRegistry{
 private val classes=linkedMapOf<String,ClassDescriptor>()
 fun register(d:ClassDescriptor){classes[d.name]=d}
 fun get(n:String)=classes[n]
 fun all()=classes.values.toList()
}