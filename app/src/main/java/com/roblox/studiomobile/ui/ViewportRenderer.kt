package com.roblox.studiomobile.ui

import android.content.Context
import android.opengl.GLSurfaceView
import android.view.MotionEvent
import com.roblox.studiomobile.editor.ViewportCamera
import com.roblox.studiomobile.editor.SceneGraph
import com.roblox.studiomobile.editor.Vec3
import javax.microedition.khronos.egl.EGLConfig
import javax.microedition.khronos.opengles.GL10
import kotlin.math.abs

class ViewportRenderer(private val graph: SceneGraph, private val camera: ViewportCamera) : GLSurfaceView.Renderer {
    override fun onSurfaceCreated(gl: GL10?, config: EGLConfig?) {
        gl?.glClearColor(0.08f, 0.09f, 0.11f, 1f)
        gl?.glEnable(GL10.GL_DEPTH_TEST)
    }

    override fun onSurfaceChanged(gl: GL10?, width: Int, height: Int) {
        gl?.glViewport(0, 0, width, height)
    }

    override fun onDrawFrame(gl: GL10?) {
        gl?.glClear(GL10.GL_COLOR_BUFFER_BIT or GL10.GL_DEPTH_BUFFER_BIT)
    }
}

class ViewportSurface(
    context: Context,
    graph: SceneGraph,
    camera: ViewportCamera
) : GLSurfaceView(context) {
    private var lastX = 0f
    private var lastY = 0f
    private var mode = 0

    init {
        setEGLContextClientVersion(2)
        setRenderer(ViewportRenderer(graph, camera))
        renderMode = RENDERMODE_CONTINUOUSLY
    }

    override fun onTouchEvent(event: MotionEvent): Boolean {
        when (event.actionMasked) {
            MotionEvent.ACTION_DOWN -> {
                lastX = event.x
                lastY = event.y
                mode = 1
                return true
            }
            MotionEvent.ACTION_MOVE -> {
                if (mode == 1) {
                    val dx = event.x - lastX
                    val dy = event.y - lastY
                    lastX = event.x
                    lastY = event.y
                }
                return true
            }
            MotionEvent.ACTION_UP, MotionEvent.ACTION_CANCEL -> {
                mode = 0
                return true
            }
        }
        return true
    }
}
