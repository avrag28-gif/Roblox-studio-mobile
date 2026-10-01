package com.roblox.studiomobile.editor

import com.roblox.studiomobile.core.Instance
import kotlin.math.abs
import kotlin.math.sqrt

data class Ray(val origin:Vec3,val direction:Vec3)
data class PickHit(val instance:Instance,val distance:Float)

class ViewportPicker(private val graph:SceneGraph){
 fun pick(ray:Ray):Instance?{
  var best:PickHit?=null
  for(node in graph.renderable()){
   val p=node.transform.position
   val s=node.transform.scale
   val hit=intersectAabb(ray,p,s) ?: continue
   if(best==null || hit<best!!.distance) best=PickHit(node.instance,hit)
  }
  return best?.instance
 }
 private fun intersectAabb(ray:Ray,c:Vec3,h:Vec3):Float?{
  var tMin=0f
  var tMax=Float.POSITIVE_INFINITY
  fun slab(o:Float,d:Float,min:Float,max:Float):Boolean{
   if(abs(d)<0.000001f)return o in min..max
   var a=(min-o)/d
   var b=(max-o)/d
   if(a>b){val t=a;a=b;b=t}
   tMin=maxOf(tMin,a)
   tMax=minOf(tMax,b)
   return tMin<=tMax
  }
  if(!slab(ray.origin.x,ray.direction.x,c.x-h.x,c.x+h.x))return null
  if(!slab(ray.origin.y,ray.direction.y,c.y-h.y,c.y+h.y))return null
  if(!slab(ray.origin.z,ray.direction.z,c.z-h.z,c.z+h.z))return null
  return if(tMax>=0f)maxOf(0f,tMin) else null
 }
}
