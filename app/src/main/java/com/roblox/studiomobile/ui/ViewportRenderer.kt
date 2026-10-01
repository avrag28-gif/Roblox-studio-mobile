package com.roblox.studiomobile.ui

import android.opengl.GLES20
import android.opengl.GLSurfaceView
import com.roblox.studiomobile.editor.SceneGraph
import com.roblox.studiomobile.editor.Vec3
import com.roblox.studiomobile.editor.ViewportCamera
import java.nio.ByteBuffer
import java.nio.ByteOrder
import java.nio.FloatBuffer
import javax.microedition.khronos.egl.EGLConfig
import javax.microedition.khronos.opengles.GL10
import kotlin.math.tan

class ViewportRenderer(private val graph: SceneGraph, private val camera: ViewportCamera) : GLSurfaceView.Renderer {
    private var program = 0
    private var positionHandle = 0
    private var mvpHandle = 0
    private var width = 1
    private var height = 1
    private val cube: FloatBuffer = ByteBuffer.allocateDirect(CUBE.size * 4)
        .order(ByteOrder.nativeOrder()).asFloatBuffer().apply { put(CUBE); position(0) }

    override fun onSurfaceCreated(gl: GL10?, config: EGLConfig?) {
        program = createProgram(VERTEX_SHADER, FRAGMENT_SHADER)
        positionHandle = GLES20.glGetAttribLocation(program, "aPosition")
        mvpHandle = GLES20.glGetUniformLocation(program, "uMvp")
        GLES20.glClearColor(0.08f, 0.09f, 0.11f, 1f)
        GLES20.glEnable(GLES20.GL_DEPTH_TEST)
    }

    override fun onSurfaceChanged(gl: GL10?, width: Int, height: Int) {
        this.width = width.coerceAtLeast(1)
        this.height = height.coerceAtLeast(1)
        GLES20.glViewport(0, 0, this.width, this.height)
    }

    override fun onDrawFrame(gl: GL10?) {
        GLES20.glClear(GLES20.GL_COLOR_BUFFER_BIT or GLES20.GL_DEPTH_BUFFER_BIT)
        GLES20.glUseProgram(program)
        val aspect = width.toFloat() / height.toFloat()
        val projection = perspective(60f, aspect, 0.1f, 500f)
        val view = lookAt(camera.position().x, camera.position().y, camera.position().z, camera.target.x, camera.target.y, camera.target.z)
        GLES20.glEnableVertexAttribArray(positionHandle)
        GLES20.glVertexAttribPointer(positionHandle, 3, GLES20.GL_FLOAT, false, 12, cube)
        val parts = graph.renderable()
        if (parts.isEmpty()) {
            drawCube(multiply(projection, view))
        } else {
            parts.forEach { node ->
                val position = node.transform.position
                val size = node.transform.scale
                val model = translation(position.x, position.y, position.z)
                val scaled = multiply(model, scale(size.x, size.y, size.z))
                GLES20.glUniformMatrix4fv(mvpHandle, 1, false, multiply(multiply(projection, view), scaled), 0)
                cube.position(0)
                GLES20.glDrawArrays(GLES20.GL_TRIANGLES, 0, CUBE.size / 3)
            }
        }
        GLES20.glDisableVertexAttribArray(positionHandle)
    }

    private fun createProgram(vs: String, fs: String): Int {
        fun compile(type: Int, source: String): Int {
            val shader = GLES20.glCreateShader(type)
            GLES20.glShaderSource(shader, source)
            GLES20.glCompileShader(shader)
            return shader
        }
        val v = compile(GLES20.GL_VERTEX_SHADER, vs)
        val f = compile(GLES20.GL_FRAGMENT_SHADER, fs)
        return GLES20.glCreateProgram().also {
            GLES20.glAttachShader(it, v)
            GLES20.glAttachShader(it, f)
            GLES20.glLinkProgram(it)
            GLES20.glDeleteShader(v)
            GLES20.glDeleteShader(f)
        }
    }

    private fun perspective(fov: Float, aspect: Float, near: Float, far: Float): FloatArray {
        val f = 1f / tan(Math.toRadians((fov / 2f).toDouble())).toFloat()
        val nf = 1f / (near - far)
        return floatArrayOf(f / aspect, 0f, 0f, 0f, 0f, f, 0f, 0f, 0f, 0f, (far + near) * nf, -1f, 0f, 0f, 2f * far * near * nf, 0f)
    }

    private fun lookAt(ex: Float, ey: Float, ez: Float, tx: Float, ty: Float, tz: Float): FloatArray {
        val fx = tx-ex; val fy = ty-ey; val fz = tz-ez
        val fl = kotlin.math.sqrt(fx*fx+fy*fy+fz*fz)
        val nx = fx/fl; val ny = fy/fl; val nz = fz/fl
        val sx = ny*0f - nz*1f; val sy = nz*0f - nx*0f; val sz = nx*1f - ny*0f
        val sl = kotlin.math.sqrt(sx*sx+sy*sy+sz*sz).coerceAtLeast(0.0001f)
        val rx=sx/sl; val ry=sy/sl; val rz=sz/sl
        val ux=ry*nz-rz*ny; val uy=rz*nx-rx*nz; val uz=rx*ny-ry*nx
        return floatArrayOf(rx,ux,-nx,0f, ry,uy,-ny,0f, rz,uz,-nz,0f, -(rx*ex+ry*ey+rz*ez), -(ux*ex+uy*ey+uz*ez), nx*ex+ny*ey+nz*ez,1f)
    }

    private fun drawCube(mvp: FloatArray) {
        GLES20.glUniformMatrix4fv(mvpHandle, 1, false, mvp, 0)
        cube.position(0)
        GLES20.glDrawArrays(GLES20.GL_TRIANGLES, 0, CUBE.size / 3)
    }

    private fun translation(x: Float, y: Float, z: Float): FloatArray =
        floatArrayOf(1f,0f,0f,0f, 0f,1f,0f,0f, 0f,0f,1f,0f, x,y,z,1f)

    private fun scale(x: Float, y: Float, z: Float): FloatArray =
        floatArrayOf(x,0f,0f,0f, 0f,y,0f,0f, 0f,0f,z,0f, 0f,0f,0f,1f)

    private fun multiply(a: FloatArray, b: FloatArray): FloatArray {
        val r=FloatArray(16)
        for(i in 0..3) for(j in 0..3) for(k in 0..3) r[i*4+j]+=a[i*4+k]*b[k*4+j]
        return r
    }

    companion object {
        private val CUBE = floatArrayOf(
            -1f,-1f,1f, 1f,-1f,1f, 1f,1f,1f, -1f,-1f,1f, 1f,1f,1f, -1f,1f,1f,
            -1f,-1f,-1f, -1f,1f,-1f, 1f,1f,-1f, -1f,-1f,-1f, 1f,1f,-1f, 1f,-1f,-1f,
            -1f,1f,-1f, -1f,1f,1f, 1f,1f,1f, -1f,1f,-1f, 1f,1f,1f, 1f,1f,-1f,
            -1f,-1f,-1f, 1f,-1f,-1f, 1f,-1f,1f, -1f,-1f,-1f, 1f,-1f,1f, -1f,-1f,1f,
            1f,-1f,-1f, 1f,1f,-1f, 1f,1f,1f, 1f,-1f,-1f, 1f,1f,1f, 1f,-1f,1f,
            -1f,-1f,-1f, -1f,-1f,1f, -1f,1f,1f, -1f,-1f,-1f, -1f,1f,1f, -1f,1f,-1f
        )
        private const val VERTEX_SHADER = "attribute vec3 aPosition; uniform mat4 uMvp; void main(){gl_Position=uMvp*vec4(aPosition,1.0);}"
        private const val FRAGMENT_SHADER = "precision mediump float; void main(){gl_FragColor=vec4(0.55,0.62,0.72,1.0);}"
    }
}
