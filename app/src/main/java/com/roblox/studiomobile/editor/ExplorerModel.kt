package com.roblox.studiomobile.editor
import com.roblox.studiomobile.core.*
data class ExplorerNode(val instance:Instance,val depth:Int)
class ExplorerModel(private val root:DataModel){
 fun flatten():List<ExplorerNode>{val out=mutableListOf<ExplorerNode>();fun visit(i:Instance,d:Int){out+=ExplorerNode(i,d);i.children.forEach{visit(it,d+1)}};root.children.forEach{visit(it,0)};return out}
}
