package com.roblox.studiomobile.editor

import com.roblox.studiomobile.core.*

enum class TransformTool { Select, Move, Scale, Rotate }

enum class TransformAxis { Screen, X, Y, Z }

class TransformController(private val runtime: CoreRuntime) {
    var tool = TransformTool.Select
    var axis = TransformAxis.Screen

    fun beginGesture() {
        if (tool != TransformTool.Select) runtime.transactions.beginBatch()
    }

    fun endGesture(commit: Boolean = true) {
        if (tool != TransformTool.Select) runtime.transactions.endBatch(commit)
    }

    fun commitMove(instance: Instance, start: Vec3, end: Vec3) {
        if (start == end) return
        runtime.properties.set(instance, "Position", end)
    }

    fun commitScale(instance: Instance, start: Vec3, end: Vec3) {
        if (start == end) return
        runtime.properties.set(instance, "Size", end)
    }

    fun applyAxisDelta(instances: Collection<Instance>, gizmoAxis: GizmoAxis, delta: Float): Boolean {
        val targets = instances.filter { it.className == "Part" }
        if (targets.isEmpty() || gizmoAxis == GizmoAxis.None) return false
        targets.forEach { applyAxisDelta(it, gizmoAxis, delta) }
        return true
    }

    fun applyAxisDelta(instance: Instance, gizmoAxis: GizmoAxis, delta: Float): Boolean {
        if (instance.className != "Part" || gizmoAxis == GizmoAxis.None) return false
        val axis = when (gizmoAxis) {
            GizmoAxis.X -> TransformAxis.X
            GizmoAxis.Y -> TransformAxis.Y
            GizmoAxis.Z -> TransformAxis.Z
            GizmoAxis.None -> TransformAxis.Screen
        }
        return when (tool) {
            TransformTool.Move -> {
                val p = runtime.properties.get<Vec3>(instance, "Position") ?: Vec3()
                val next = when (axis) {
                    TransformAxis.X -> Vec3(p.x + delta, p.y, p.z)
                    TransformAxis.Y -> Vec3(p.x, p.y + delta, p.z)
                    TransformAxis.Z -> Vec3(p.x, p.y, p.z + delta)
                    TransformAxis.Screen -> p
                }
                runtime.properties.set(instance, "Position", next)
                true
            }
            TransformTool.Scale -> {
                val s = runtime.properties.get<Vec3>(instance, "Size") ?: Vec3(4f, 1f, 2f)
                val amount = delta * 2f
                val next = when (axis) {
                    TransformAxis.X -> Vec3((s.x + amount).coerceAtLeast(0.1f), s.y, s.z)
                    TransformAxis.Y -> Vec3(s.x, (s.y + amount).coerceAtLeast(0.1f), s.z)
                    TransformAxis.Z -> Vec3(s.x, s.y, (s.z + amount).coerceAtLeast(0.1f))
                    TransformAxis.Screen -> s
                }
                runtime.properties.set(instance, "Size", next)
                true
            }
            TransformTool.Rotate -> {
                val r = runtime.properties.get<Vec3>(instance, "Orientation") ?: Vec3()
                val next = when (axis) {
                    TransformAxis.X -> Vec3(r.x + delta * 35f, r.y, r.z)
                    TransformAxis.Y -> Vec3(r.x, r.y + delta * 35f, r.z)
                    TransformAxis.Z -> Vec3(r.x, r.y, r.z + delta * 35f)
                    TransformAxis.Screen -> r
                }
                runtime.properties.set(instance, "Orientation", next)
                true
            }
            TransformTool.Select -> false
        }
    }

    fun applyScreenDelta(instance: Instance, dx: Float, dy: Float): Boolean {
        if (instance.className != "Part") return false
        return when (tool) {
            TransformTool.Move -> {
                val p = runtime.properties.get<Vec3>(instance, "Position") ?: Vec3()
                runtime.properties.set(instance, "Position", moveDelta(p, dx, dy))
                true
            }
            TransformTool.Scale -> {
                val s = runtime.properties.get<Vec3>(instance, "Size") ?: Vec3(4f, 1f, 2f)
                runtime.properties.set(instance, "Size", scaleDelta(s, dx, dy))
                true
            }
            TransformTool.Rotate -> {
                val r = runtime.properties.get<Vec3>(instance, "Orientation") ?: Vec3()
                val amount = (dx - dy) * 0.5f
                val next = when (axis) {
                    TransformAxis.X -> Vec3(r.x + amount, r.y, r.z)
                    TransformAxis.Y -> Vec3(r.x, r.y + amount, r.z)
                    TransformAxis.Z -> Vec3(r.x, r.y, r.z + amount)
                    TransformAxis.Screen -> Vec3(r.x, r.y + amount, r.z)
                }
                runtime.properties.set(instance, "Orientation", next)
                true
            }
            TransformTool.Select -> false
        }
    }
    private fun moveDelta(p: Vec3, dx: Float, dy: Float): Vec3 {
        val amount = dx * 0.025f - dy * 0.025f
        return when (axis) {
            TransformAxis.X -> Vec3(p.x + dx * 0.025f, p.y, p.z)
            TransformAxis.Y -> Vec3(p.x, p.y - dy * 0.025f, p.z)
            TransformAxis.Z -> Vec3(p.x, p.y, p.z + amount)
            TransformAxis.Screen -> Vec3(p.x + dx * 0.025f, p.y - dy * 0.025f, p.z)
        }
    }

    private fun scaleDelta(s: Vec3, dx: Float, dy: Float): Vec3 {
        return when (axis) {
            TransformAxis.X -> Vec3((s.x + dx * 0.025f).coerceAtLeast(0.1f), s.y, s.z)
            TransformAxis.Y -> Vec3(s.x, (s.y - dy * 0.025f).coerceAtLeast(0.1f), s.z)
            TransformAxis.Z -> Vec3(s.x, s.y, (s.z + (dx - dy) * 0.0125f).coerceAtLeast(0.1f))
            TransformAxis.Screen -> Vec3((s.x + dx * 0.025f).coerceAtLeast(0.1f), (s.y - dy * 0.025f).coerceAtLeast(0.1f), s.z)
        }
    }
}

