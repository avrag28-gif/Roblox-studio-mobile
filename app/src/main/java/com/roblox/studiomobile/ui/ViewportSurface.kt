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
                    camera.orbit(-dx * 0.008f, -dy * 0.008f)
                    lastX = event.x
                    lastY = event.y
                }
                return true
            }
            MotionEvent.ACTION_POINTER_UP -> {
                gesture = if (event.pointerCount > 2) 2 else 1
                return true
            }
            MotionEvent.ACTION_UP, MotionEvent.ACTION_CANCEL -> {
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
