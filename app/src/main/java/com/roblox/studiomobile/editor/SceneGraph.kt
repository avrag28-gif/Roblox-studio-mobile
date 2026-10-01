package com.roblox.studiomobile.editor
import com.roblox.studiomobile.core.Instance
data class Vec3(val x:Float=0f,val y:Float=0f,val z:Float=0f)
data class SceneTransform(var position:Vec3=Vec3(),var rotation:Vec3=Vec3(),var scale:Vec3=Vec3(1f,1f,1f))
data class SceneNode(val instance:Instance,val transform:SceneTransform=SceneTransform(),val children:MutableList<SceneNode> = mutableListOf())
class SceneGraph(private val root:Instance){
 private val nodes=linkedMapOf<String,SceneNode>()
 fun sync(){nodes.clear();fun visit(i:Instance){val n=SceneNode(i);nodes[i.id]=n;i.children.forEach{visit(it)}};visit(root)}
 fun node(i:Instance):SceneNode?=nodes[i.id]
 fun all():List<SceneNode>=nodes.values.toList()
}