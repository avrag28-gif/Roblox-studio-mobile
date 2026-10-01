package com.roblox.studiomobile.ui

import android.opengl.GLES20
import android.opengl.GLSurfaceView
import android.opengl.Matrix
import com.roblox.studiomobile.editor.SceneGraph
import com.roblox.studiomobile.core.SelectionService
import com.roblox.studiomobile.editor.ViewportCamera
import java.nio.ByteBuffer
import java.nio.ByteOrder
import java.nio.FloatBuffer
import javax.microedition.khronos.egl.EGLConfig
import javax.microedition.khronos.opengles.GL10

class ViewportRenderer(
    private val graph: SceneGraph,
    private val camera: ViewportCamera,
    selection: SelectionService
) : GLSurfaceView.Renderer {
    private var program = 0
    private var positionHandle = 0
    private var mvpHandle = 0
    private var colorHandle = 0
    private var lineProgram = 0
    private var linePositionHandle = 0
    private var lineColorHandle = 0
    private var lineMvpHandle = 0
    @Volatile private var selectedIds: Set<String> = emptySet()
    private var width = 1
    private var height = 1

    private val projection = FloatArray(16)
    private val view = FloatArray(16)
    private val model = FloatArray(16)
    private val viewProjection = FloatArray(16)
    private val mvp = FloatArray(16)

    private val gizmoLines: FloatBuffer = ByteBuffer.allocateDirect(GIZMO_LINES.size * 4).order(ByteOrder.nativeOrder()).asFloatBuffer().apply { put(GIZMO_LINES); position(0) }

    private var runtimeRotationX = 0f
    private var runtimeRotationY = 0f
    private var runtimeRotationZ = 0f

    private val cube: FloatBuffer = ByteBuffer.allocateDirect(CUBE.size * 4)
        .order(ByteOrder.nativeOrder())
        .asFloatBuffer()
        .apply { put(CUBE); position(0) }

    init { selection.changed.connect { selectedIds = it.map { instance -> instance.id }.toSet() } }

    override fun onSurfaceCreated(gl: GL10?, config: EGLConfig?) {
        program = createProgram(VERTEX_SHADER, FRAGMENT_SHADER)
        lineProgram = createProgram(LINE_VERTEX_SHADER, LINE_FRAGMENT_SHADER)
        positionHandle = GLES20.glGetAttribLocation(program, "aPosition")
        mvpHandle = GLES20.glGetUniformLocation(program, "uMvp")
        colorHandle = GLES20.glGetUniformLocation(program, "uColor")
        linePositionHandle = GLES20.glGetAttribLocation(lineProgram, "aPosition")
        lineMvpHandle = GLES20.glGetUniformLocation(lineProgram, "uMvp")
        lineColorHandle = GLES20.glGetUniformLocation(lineProgram, "uColor")
        GLES20.glClearColor(0.08f, 0.09f, 0.11f, 1f)
        GLES20.glEnable(GLES20.GL_DEPTH_TEST)
    }

    override fun onSurfaceChanged(gl: GL10?, width: Int, height: Int) {
        this.width = width.coerceAtLeast(1)
        this.height = height.coerceAtLeast(1)
        GLES20.glViewport(0, 0, this.width, this.height)
        Matrix.setIdentityM(projection, 0)
        Matrix.perspectiveM(projection, 0, 60f, this.width.toFloat() / this.height.toFloat(), 0.1f, 500f)
    }

    override fun onDrawFrame(gl: GL10?) {
        GLES20.glClear(GLES20.GL_COLOR_BUFFER_BIT or GLES20.GL_DEPTH_BUFFER_BIT)
        GLES20.glUseProgram(program)

        val eye = camera.position()
        Matrix.setLookAtM(
            view, 0,
            eye.x, eye.y, eye.z,
            camera.target.x, camera.target.y, camera.target.z,
            0f, 1f, 0f
        )
        Matrix.multiplyMM(viewProjection, 0, projection, 0, view, 0)

        GLES20.glEnableVertexAttribArray(positionHandle)
        GLES20.glVertexAttribPointer(positionHandle, 3, GLES20.GL_FLOAT, false, 12, cube)

        val parts = graph.renderable()
        if (parts.isEmpty()) {
            Matrix.setIdentityM(model, 0)
            drawModel(model, false)
        } else {
            parts.forEach { node ->
                val p = node.transform.position
                val s = node.transform.scale
                Matrix.setIdentityM(model, 0)
                Matrix.translateM(model, 0, p.x, p.y, p.z)
                Matrix.rotateM(model, 0, runtimeRotationX, 1f, 0f, 0f)
                Matrix.rotateM(model, 0, runtimeRotationY, 0f, 1f, 0f)
                Matrix.rotateM(model, 0, runtimeRotationZ, 0f, 0f, 1f)
                Matrix.scaleM(model, 0, s.x, s.y, s.z)
                drawModel(model, node.instance.id in selectedIds)
            }
        }

        GLES20.glDisableVertexAttribArray(positionHandle)
        drawGizmo(parts)
    }

    private fun drawGizmo(parts: List<com.roblox.studiomobile.editor.SceneNode>) {
        val selected = parts.firstOrNull { it.instance.id in selectedIds } ?: return
        val p = selected.transform.position
        val length = (selected.transform.scale.x + selected.transform.scale.y + selected.transform.scale.z).coerceAtLeast(1f) * 1.4f
        Matrix.setIdentityM(model, 0)
        Matrix.translateM(model, 0, p.x, p.y, p.z)
        Matrix.scaleM(model, 0, length, length, length)
        Matrix.multiplyMM(mvp, 0, viewProjection, 0, model, 0)
        GLES20.glUseProgram(lineProgram)
        GLES20.glEnableVertexAttribArray(linePositionHandle)
        GLES20.glVertexAttribPointer(linePositionHandle, 3, GLES20.GL_FLOAT, false, 12, gizmoLines)
        GLES20.glUniformMatrix4fv(lineMvpHandle, 1, false, mvp, 0)
        GLES20.glLineWidth(5f)
        drawGizmoAxis(0, 1f, 0.2f, 0.2f)
        drawGizmoAxis(1, 0.2f, 1f, 0.2f)
        drawGizmoAxis(2, 0.2f, 0.5f, 1f)
        GLES20.glDisableVertexAttribArray(linePositionHandle)
        GLES20.glUseProgram(program)
    }

    private fun drawGizmoAxis(axis: Int, r: Float, g: Float, b: Float) {
        GLES20.glUniform4f(lineColorHandle, r, g, b, 1f)
        val offset = axis * 2
        gizmoLines.position(offset * 3)
        GLES20.glDrawArrays(GLES20.GL_LINES, 0, 2)
        gizmoLines.position(0)
    }

    private fun drawModel(modelMatrix: FloatArray, selected: Boolean) {
        Matrix.multiplyMM(mvp, 0, viewProjection, 0, modelMatrix, 0)
        GLES20.glUniformMatrix4fv(mvpHandle, 1, false, mvp, 0)
        GLES20.glUniform4f(colorHandle, if (selected) 0.20f else 0.55f, if (selected) 0.75f else 0.62f, if (selected) 1.0f else 0.72f, 1f)
        cube.position(0)
        GLES20.glDrawArrays(GLES20.GL_TRIANGLES, 0, CUBE.size / 3)
    }

    private fun createProgram(vs: String, fs: String): Int {
        fun compile(type: Int, source: String): Int {
            val shader = GLES20.glCreateShader(type)
            GLES20.glShaderSource(shader, source)
            GLES20.glCompileShader(shader)
            check(GLES20.glGetShaderiv(shader, GLES20.GL_COMPILE_STATUS, IntArray(1), 0) != 0) {
                GLES20.glGetShaderInfoLog(shader)
            }
            return shader
        }

        val v = compile(GLES20.GL_VERTEX_SHADER, vs)
        val f = compile(GLES20.GL_FRAGMENT_SHADER, fs)
        return GLES20.glCreateProgram().also {
            GLES20.glAttachShader(it, v)
            GLES20.glAttachShader(it, f)
            GLES20.glLinkProgram(it)
            val status = IntArray(1)
            GLES20.glGetProgramiv(it, GLES20.GL_LINK_STATUS, status, 0)
            check(status[0] != 0) { GLES20.glGetProgramInfoLog(it) }
            GLES20.glDeleteShader(v)
            GLES20.glDeleteShader(f)
        }
    }

    companion object {
        private val GIZMO_LINES = floatArrayOf(0f,0f,0f, 1f,0f,0f, 0f,0f,0f, 0f,1f,0f, 0f,0f,0f, 0f,0f,1f)
        private val CUBE = floatArrayOf(
            -1f,-1f,1f, 1f,-1f,1f, 1f,1f,1f, -1f,-1f,1f, 1f,1f,1f, -1f,1f,1f,
            -1f,-1f,-1f, -1f,1f,-1f, 1f,1f,-1f, -1f,-1f,-1f, 1f,1f,-1f, 1f,-1f,-1f,
            -1f,1f,-1f, -1f,1f,1f, 1f,1f,1f, -1f,1f,-1f, 1f,1f,1f, 1f,1f,-1f,
            -1f,-1f,-1f, 1f,-1f,-1f, 1f,-1f,1f, -1f,-1f,-1f, 1f,-1f,1f, -1f,-1f,1f,
            1f,-1f,-1f, 1f,1f,-1f, 1f,1f,1f, 1f,-1f,-1f, 1f,1f,1f, 1f,-1f,1f,
            -1f,-1f,-1f, -1f,-1f,1f, -1f,1f,1f, -1f,-1f,-1f, -1f,1f,1f, -1f,1f,-1f
        )
        private const val LINE_VERTEX_SHADER = "attribute vec3 aPosition; uniform mat4 uMvp; void main(){gl_Position=uMvp*vec4(aPosition,1.0);}"
        private const val LINE_FRAGMENT_SHADER = "precision mediump float; uniform vec4 uColor; void main(){gl_FragColor=uColor;}"
        private const val VERTEX_SHADER =
            "attribute vec3 aPosition; uniform mat4 uMvp; void main(){gl_Position=uMvp*vec4(aPosition,1.0);}"
        private const val FRAGMENT_SHADER =
            "precision mediump float; uniform vec4 uColor; void main(){gl_FragColor=uColor;}"
    }
}
