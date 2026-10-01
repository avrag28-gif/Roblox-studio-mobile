package com.roblox.studiomobile.editor
import kotlin.math.cos
import kotlin.math.sin
import kotlin.math.sqrt

class ViewportCamera{
 var target=Vec3()
 var distance=18f
 var yaw=0.65f
 var pitch=0.45f
 fun orbit(dx:Float,dy:Float){yaw+=dx;pitch=(pitch+dy).coerceIn(-1.45f,1.45f)}
 fun zoom(delta:Float){distance=(distance+delta).coerceIn(2f,200f)}
 fun pan(dx:Float,dy:Float){val s=distance*0.0025f;target=Vec3(target.x-dx*s,target.y+dy*s,target.z)}
 fun position():Vec3{val cp=cos(pitch);return Vec3(target.x+distance*cp*cos(yaw),target.y+distance*sin(pitch),target.z+distance*cp*sin(yaw))}
 fun ray(screenX:Float,screenY:Float,width:Int,height:Int):Ray{
  val nx=2f*screenX/width-1f
  val ny=1f-2f*screenY/height
  val aspect=width.toFloat()/height.coerceAtLeast(1)
  val fov=60f
  val tanHalf=kotlin.math.tan(Math.toRadians((fov/2f).toDouble())).toFloat()
  val forward=normalize(target-position())
  val right=normalize(cross(forward,Vec3(0f,1f,0f)))
  val up=normalize(cross(right,forward))
  val dir=normalize(forward+right*(nx*aspect*tanHalf)+up*(ny*tanHalf))
  return Ray(position(),dir)
 }
 private fun normalize(v:Vec3):Vec3{val l=sqrt(v.x*v.x+v.y*v.y+v.z*v.z).coerceAtLeast(0.000001f);return Vec3(v.x/l,v.y/l,v.z/l)}
 private fun cross(a:Vec3,b:Vec3)=Vec3(a.y*b.z-a.z*b.y,a.z*b.x-a.x*b.z,a.x*b.y-a.y*b.x)
 private operator fun Vec3.plus(v:Vec3)=Vec3(x+v.x,y+v.y,z+v.z)
 private operator fun Vec3.times(s:Float)=Vec3(x*s,y*s,z*s)
}
