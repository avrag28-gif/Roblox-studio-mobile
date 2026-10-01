package com.roblox.studiomobile.editor

import com.roblox.studiomobile.core.*

enum class TransformTool { Select, Move, Scale }

class TransformController(private val runtime:CoreRuntime){
 var tool=TransformTool.Select
 fun commitMove(instance:Instance,start:Vec3,end:Vec3){
  if(start==end)return
  runtime.properties.set(instance,"Position",end)
 }
 fun commitScale(instance:Instance,start:Vec3,end:Vec3){
  if(start==end)return
  runtime.properties.set(instance,"Size",end)
 }
}
