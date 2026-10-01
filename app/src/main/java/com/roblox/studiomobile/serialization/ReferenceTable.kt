package com.roblox.studiomobile.serialization
import com.roblox.studiomobile.core.Instance
class ReferenceTable{
 private val byId=linkedMapOf<String,Instance>()
 fun register(i:Instance){byId[i.id]=i}
 fun registerTree(root:Instance){register(root);root.children.forEach(::registerTree)}
 fun resolve(id:String):Instance?=byId[id]
 fun all():List<Instance>=byId.values.toList()
}