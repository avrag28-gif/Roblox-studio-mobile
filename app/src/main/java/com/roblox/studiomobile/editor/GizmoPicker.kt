package com.roblox.studiomobile.editor

import kotlin.math.abs
import kotlin.math.sqrt

enum class GizmoAxis { X, Y, Z, None }

class GizmoPicker {
    fun axisParameter(ray: Ray, origin: Vec3, axis: GizmoAxis): Float? {
        val v = when (axis) {
            GizmoAxis.X -> Vec3(1f, 0f, 0f)
            GizmoAxis.Y -> Vec3(0f, 1f, 0f)
            GizmoAxis.Z -> Vec3(0f, 0f, 1f)
            GizmoAxis.None -> return null
        }
        val d = ray.direction
        val r = ray.origin - origin
        val a = dot(d, d)
        val b = dot(d, v)
        val c = dot(v, v)
        val e = dot(v, r)
        val f = dot(d, r)
        val denom = a * c - b * b
        return if (denom < 0.000001f) e / c else (a * e - b * f) / denom
    }

    fun pick(ray: Ray, origin: Vec3, length: Float): GizmoAxis {
        val axes = listOf(
            GizmoAxis.X to Vec3(1f, 0f, 0f),
            GizmoAxis.Y to Vec3(0f, 1f, 0f),
            GizmoAxis.Z to Vec3(0f, 0f, 1f)
        )
        var best = GizmoAxis.None
        var bestDistance = Float.POSITIVE_INFINITY
        val threshold = (length * 0.12f).coerceAtLeast(0.12f)
        for ((axis, direction) in axes) {
            val d = distanceRayToSegment(ray, origin, origin + direction * length)
            if (d < threshold && d < bestDistance) {
                bestDistance = d
                best = axis
            }
        }
        return best
    }

    private fun distanceRayToSegment(ray: Ray, a: Vec3, b: Vec3): Float {
        val u = ray.direction
        val v = b - a
        val w = ray.origin - a
        val aDot = dot(u, u)
        val bDot = dot(u, v)
        val cDot = dot(v, v)
        val dDot = dot(u, w)
        val eDot = dot(v, w)
        val denom = aDot * cDot - bDot * bDot
        var s: Float
        var t: Float
        if (denom < 0.000001f) {
            s = 0f
            t = (eDot / cDot).coerceIn(0f, 1f)
        } else {
            s = ((bDot * eDot - cDot * dDot) / denom).coerceAtLeast(0f)
            t = ((aDot * eDot - bDot * dDot) / denom).coerceIn(0f, 1f)
        }
        val p = ray.origin + u * s
        val q = a + v * t
        return distance(p, q)
    }

    private fun dot(a: Vec3, b: Vec3) = a.x*b.x + a.y*b.y + a.z*b.z
    private fun distance(a: Vec3, b: Vec3): Float {
        val d = a - b
        return sqrt(d.x*d.x + d.y*d.y + d.z*d.z)
    }

    private operator fun Vec3.plus(v: Vec3) = Vec3(x+v.x, y+v.y, z+v.z)
    private operator fun Vec3.minus(v: Vec3) = Vec3(x-v.x, y-v.y, z-v.z)
    private operator fun Vec3.times(s: Float) = Vec3(x*s, y*s, z*s)
}
