package com.roblox.studiomobile.editor

import kotlin.math.abs

object TransformMath {
    fun axisScreenScale(axis: GizmoAxis, camera: ViewportCamera, origin: Vec3, width: Int, height: Int): Float {
        val base = camera.ray(width * 0.5f, height * 0.5f, width, height).direction
        val point = when (axis) {
            GizmoAxis.X -> origin + Vec3(1f, 0f, 0f)
            GizmoAxis.Y -> origin + Vec3(0f, 1f, 0f)
            GizmoAxis.Z -> origin + Vec3(0f, 0f, 1f)
            GizmoAxis.None -> origin
        }
        val p0 = camera.ray(width * 0.5f, height * 0.5f, width, height).origin
        val depth = ((point.x-p0.x)*base.x + (point.y-p0.y)*base.y + (point.z-p0.z)*base.z).coerceAtLeast(0.01f)
        return (height.toFloat() / (depth * 2f)).coerceAtLeast(0.01f)
    }

    fun projectAxisDelta(axis: GizmoAxis, dx: Float, dy: Float, camera: ViewportCamera, origin: Vec3, width: Int, height: Int): Float {
        val center = camera.ray(width * 0.5f, height * 0.5f, width, height).direction
        val right = normalize(cross(center, Vec3(0f,1f,0f)))
        val up = normalize(cross(right, center))
        val axisVector = when(axis) {
            GizmoAxis.X -> Vec3(1f,0f,0f)
            GizmoAxis.Y -> Vec3(0f,1f,0f)
            GizmoAxis.Z -> Vec3(0f,0f,1f)
            GizmoAxis.None -> Vec3()
        }
        val sx = dot(axisVector, right)
        val sy = dot(axisVector, up)
        val denom = sx*sx + sy*sy
        if (denom < 0.0001f) return 0f
        return (dx*sx + dy*sy) / kotlin.math.sqrt(denom)
    }

    private fun dot(a: Vec3,b:Vec3)=a.x*b.x+a.y*b.y+a.z*b.z
    private fun cross(a:Vec3,b:Vec3)=Vec3(a.y*b.z-a.z*b.y,a.z*b.x-a.x*b.z,a.x*b.y-a.y*b.x)
    private fun normalize(v:Vec3):Vec3 { val l=kotlin.math.sqrt(v.x*v.x+v.y*v.y+v.z*v.z).coerceAtLeast(0.000001f); return Vec3(v.x/l,v.y/l,v.z/l) }
}
