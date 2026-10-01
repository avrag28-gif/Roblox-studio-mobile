package com.roblox.studiomobile.editor
import com.roblox.studiomobile.core.*
class SceneSynchronizer(private val runtime:CoreRuntime){
 val graph=SceneGraph(runtime.dataModel)
 init{
  graph.sync()
  runtime.selection.changed.connect{graph.sync()}
 }
 fun refresh(){graph.sync()}
}