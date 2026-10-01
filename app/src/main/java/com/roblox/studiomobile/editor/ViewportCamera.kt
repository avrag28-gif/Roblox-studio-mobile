package com.roblox.studiomobile.editor
import kotlin.math.cos
import kotlin.math.sin
class ViewportCamera{
 var target=Vec3()
 var distance=18f
 var yaw=0.65f
 var pitch=0.45f
 fun orbit(dx:Float,dy:Float){yaw+=dx;pitch=(pitch+dy).coerceIn(-1.45f,1.45f)}
 fun zoom(delta:Float){distance=(distance+delta).coerceIn(2f,200f)}
 fun pan(dx:Float,dy:Float){val s=distance*0.0025f;target=Vec3(target.x-dx*s,target.y+dy*s,target.z)}
 fun position():Vec3{val cp=cos(pitch);return Vec3(target.x+distance*cp*cos(yaw),target.y+distance*sin(pitch),target.z+distance*cp*sin(yaw))}
}