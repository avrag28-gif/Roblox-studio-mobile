package com.roblox.studiomobile.ui
import android.content.Context
import android.opengl.GLES20
import android.opengl.GLSurfaceView
import android.opengl.Matrix
import android.view.MotionEvent
import com.roblox.studiomobile.editor.ViewportCamera
import java.nio.ByteBuffer
import java.nio.ByteOrder
import java.nio.FloatBuffer
import javax.microedition.khronos.egl.EGLConfig
import javax.microedition.khronos.opengles.GL10
class ViewportView(context:Context):GLSurfaceView(context){
 private val camera=ViewportCamera()
 private var lastX=0f
 private var lastY=0f
 private var shader=0
 private lateinit var cube:CubeRenderer
 init{setEGLContextClientVersion(2);setRenderer(Renderer());renderMode=RENDERMODE_CONTINUOUSLY}
 override fun onTouchEvent(e:MotionEvent):Boolean{
  when(e.actionMasked){
   MotionEvent.ACTION_DOWN->{lastX=e.x;lastY=e.y}
   MotionEvent.ACTION_MOVE->{val dx=e.x-lastX;val dy=e.y-lastY;camera.orbit(-dx*.008f,-dy*.008f);lastX=e.x;lastY=e.y}
  };return true
 }
 private inner class Renderer:GLSurfaceView.Renderer{
  private val view=FloatArray(16);private val proj=FloatArray(16);private val vp=FloatArray(16)
  override fun onSurfaceCreated(gl:GL10?,config:EGLConfig?){
   GLES20.glClearColor(.08f,.08f,.1f,1f);GLES20.glEnable(GLES20.GL_DEPTH_TEST)
   cube=CubeRenderer()
  }
  override fun onSurfaceChanged(gl:GL10?,w:Int,h:Int){GLES20.glViewport(0,0,w,h);Matrix.setLookAtM(view,0,8f,7f,8f,0f,0f,0f,0f,1f,0f);Matrix.perspectiveM(proj,0,55f,w.toFloat()/h.coerceAtLeast(1),.1f,500f);Matrix.multiplyMM(vp,0,proj,0,view,0)}
  override fun onDrawFrame(gl:GL10?){GLES20.glClear(GLES20.GL_COLOR_BUFFER_BIT or GLES20.GL_DEPTH_BUFFER_BIT);cube.draw(vp)}
 }
 private class CubeRenderer{
  private val vertices:FloatBuffer
  private val program:Int
  private val mvp=FloatArray(16)
  private val data=floatArrayOf(-1f,-1f,-1f,1f,-1f,-1f,1f,1f,-1f,-1f,1f,-1f,-1f,-1f,1f,1f,-1f,1f,1f,1f,1f,-1f,1f,1f)
  init{
   vertices=ByteBuffer.allocateDirect(data.size*4).order(ByteOrder.nativeOrder()).asFloatBuffer().apply{put(data).position(0)}
   val vs="attribute vec3 aPosition;uniform mat4 uMvp;void main(){gl_Position=uMvp*vec4(aPosition,1.0);}"
   val fs="precision mediump float;void main(){gl_FragColor=vec4(0.65,0.7,0.78,1.0);}"
   program=link(vs,fs)
  }
  fun draw(vp:FloatArray){
   Matrix.setIdentityM(mvp,0);Matrix.scaleM(mvp,0,1.5f,1.5f,1.5f);Matrix.multiplyMM(mvp,0,vp,0,mvp,0)
   GLES20.glUseProgram(program);val pos=GLES20.glGetAttribLocation(program,"aPosition");val mat=GLES20.glGetUniformLocation(program,"uMvp")
   vertices.position(0);GLES20.glEnableVertexAttribArray(pos);GLES20.glVertexAttribPointer(pos,3,GLES20.GL_FLOAT,false,0,vertices);GLES20.glUniformMatrix4fv(mat,1,false,mvp,0)
   GLES20.glDrawArrays(GLES20.GL_TRIANGLE_STRIP,0,8);GLES20.glDisableVertexAttribArray(pos)
  }
  private fun link(v:String,f:String):Int{fun compile(type:Int,s:String):Int{val id=GLES20.glCreateShader(type);GLES20.glShaderSource(id,s);GLES20.glCompileShader(id);return id};val p=GLES20.glCreateProgram();GLES20.glAttachShader(p,compile(GLES20.GL_VERTEX_SHADER,v));GLES20.glAttachShader(p,compile(GLES20.GL_FRAGMENT_SHADER,f));GLES20.glLinkProgram(p);return p}
 }
}