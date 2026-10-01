package com.roblox.studiomobile.core
import java.util.UUID
open class Instance(val className:String,val id:String=UUID.randomUUID().toString()){
 var name:String=className;private set
 var parent:Instance?=null;private set
 val children=mutableListOf<Instance>()
 val changed=Signal<String>()
 fun rename(v:String){require(v.isNotBlank());if(name!=v){name=v;changed.fire("Name")}}
 fun setParent(p:Instance?){require(p!==this);require(p?.isDescendantOf(this)!=true);if(parent===p)return;parent?.children?.remove(this);parent=p;p?.children?.add(this);changed.fire("Parent")}
 fun findFirstChild(n:String):Instance?=children.firstOrNull{it.name==n}
 fun descendants():Sequence<Instance>=sequence{for(c in children){yield(c);yieldAll(c.descendants())}}
 private fun isDescendantOf(t:Instance):Boolean{var c:Instance?=this;while(c!=null){if(c===t)return true;c=c.parent};return false}
}
class DataModel:Instance("DataModel")
