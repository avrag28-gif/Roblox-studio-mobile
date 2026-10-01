package com.roblox.studiomobile.core
class InstanceFactory(private val reflection:ReflectionRegistry){
 fun create(className:String):Instance{require(reflection.get(className)!=null){"Unknown class $className"};return Instance(className)}
}
