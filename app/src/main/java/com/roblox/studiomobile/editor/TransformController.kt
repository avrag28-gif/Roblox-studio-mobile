package com.roblox.studiomobile.editor

import com.roblox.studiomobile.core.*

enum class TransformTool { Select, Move, Scale }

class TransformController(private val runtime: CoreRuntime) {
    var tool = TransformTool.Select

    fun commitMove(instance: Instance, start: Vec3, end: Vec3) {
        if (start == end) return
        runtime.properties.set(instance, "Position", end)
    }

    fun commitScale(instance: Instance, start: Vec3, end: Vec3) {
        if (start == end) return
        runtime.properties.set(instance, "Size", end)
    }

    fun applyScreenDelta(instance: Instance, dx: Float, dy: Float): Boolean {
        if (instance.className != "Part") return false
        return when (tool) {
            TransformTool.Move -> {
                val p = runtime.properties.get<Vec3>(instance, "Position") ?: Vec3()
                runtime.properties.set(instance, "Position", Vec3(p.x + dx * 0.025f, p.y - dy * 0.025f, p.z))
                true
            }
            TransformTool.Scale -> {
                val s = runtime.properties.get<Vec3>(instance, "Size") ?: Vec3(4f, 1f, 2f)
                runtime.properties.set(instance, "Size", Vec3((s.x + dx * 0.025f).coerceAtLeast(0.1f), (s.y - dy * 0.025f).coerceAtLeast(0.1f), s.z))
                true
            }
            TransformTool.Select -> false
        }
    }
}
