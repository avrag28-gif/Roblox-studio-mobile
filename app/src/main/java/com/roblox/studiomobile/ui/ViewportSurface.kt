package com.roblox.studiomobile.ui

import android.content.Context
import android.opengl.GLSurfaceView
import android.view.MotionEvent
import com.roblox.studiomobile.editor.SceneGraph
import com.roblox.studiomobile.editor.ViewportCamera

class ViewportSurface(
    context: Context,
    graph: SceneGraph,
    private val camera: ViewportCamera
) : GLSurfaceView(context) {
    private var lastX = 0f
    private var lastY = 0f
    private var lastSpan = 0f
    private var gesture = 0
    var onPicked: ((com.roblox.studiomobile.core.Instance) -> Unit)? = null
    var onTransformDrag: ((Float, Float) -> Boolean)? = null
    private var downX = 0f
    private var downY = 0f

    init {
        setEGLContextClientVersion(2)
        setRenderer(ViewportRenderer(graph, camera))
        renderMode = RENDERMODE_CONTINUOUSLY
        isFocusable = true
        isFocusableInTouchMode = true
    }

    override fun onTouchEvent(event: MotionEvent): Boolean {
        when (event.actionMasked) {
            MotionEvent.ACTION_DOWN -> {
                lastX = event.x
                lastY = event.y
                downX = event.x
                downY = event.y
                gesture = 1
                return true
            }
            MotionEvent.ACTION_POINTER_DOWN -> {
                if (event.pointerCount >= 2) {
                    lastSpan = span(event)
                    gesture = 2
                }
                return true
            }
            MotionEvent.ACTION_MOVE -> {
                if (gesture == 2 && event.pointerCount >= 2) {
                    val current = span(event)
                    camera.zoom((lastSpan - current) * 0.035f)
                    lastSpan = current
                } else if (gesture == 1) {
                    val dx = event.x - lastX
                    val dy = event.y - lastY
                    val transformed = onTransformDrag?.invoke(dx, dy) == true
                    if (!transformed) camera.orbit(-dx * 0.008f, -dy * 0.008f)
                    lastX = event.x
                    lastY = event.y
                }
                return true
            }
            MotionEvent.ACTION_POINTER_UP -> {
                gesture = if (event.pointerCount > 2) 2 else 1
                if (gesture == 1) {
                    lastX = event.x
                    lastY = event.y
                }
                return true
            }
            MotionEvent.ACTION_UP -> {
                if (gesture == 1 && kotlin.math.abs(event.x - downX) < 12f && kotlin.math.abs(event.y - downY) < 12f) {
                    val ray = camera.ray(event.x, event.y, width, height)
                    val hit = com.roblox.studiomobile.editor.ViewportPicker(graph).pick(ray)
                    if (hit != null) onPicked?.invoke(hit)
                }
                gesture = 0
                return true
            }
            MotionEvent.ACTION_CANCEL -> {
                gesture = 0
                return true
            }
        }
        return true
    }

    private fun span(event: MotionEvent): Float {
        val dx = event.getX(0) - event.getX(1)
        val dy = event.getY(0) - event.getY(1)
        return kotlin.math.sqrt(dx * dx + dy * dy)
    }
}
