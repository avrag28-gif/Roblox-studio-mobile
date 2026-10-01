package com.roblox.studiomobile.editor
import com.roblox.studiomobile.core.Instance
import com.roblox.studiomobile.core.PropertyStore

data class Vec3(val x:Float=0f,val y:Float=0f,val z:Float=0f)
data class SceneTransform(var position:Vec3=Vec3(),var rotation:Vec3=Vec3(),var scale:Vec3=Vec3(1f,1f,1f))
data class SceneNode(val instance:Instance,val transform:SceneTransform=SceneTransform(),val children:MutableList<SceneNode> = mutableListOf())

class SceneGraph(private val root:Instance, private val properties:PropertyStore? = null){
 private val nodes=linkedMapOf<String,SceneNode>()
 fun sync(){
  nodes.clear()
  fun visit(i:Instance):SceneNode{
   val position=properties?.get<Vec3>(i,"Position") ?: Vec3()
   val size=properties?.get<Vec3>(i,"Size") ?: Vec3(1f,1f,1f)
   val n=SceneNode(i,SceneTransform(position=position,scale=Vec3(size.x/2f,size.y/2f,size.z/2f)))
   nodes[i.id]=n
   i.children.forEach{n.children+=visit(it)}
   return n
  }
  visit(root)
 }
 fun node(i:Instance):SceneNode?=nodes[i.id]
 fun all():List<SceneNode>=nodes.values.toList()
 fun renderable():List<SceneNode>=nodes.values.filter{it.instance.className=="Part"}
}
