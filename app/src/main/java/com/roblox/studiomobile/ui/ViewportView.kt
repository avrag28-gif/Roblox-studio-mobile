package com.roblox.studiomobile.ui
import android.content.Context
import android.opengl.GLES20
import android.opengl.GLSurfaceView
import android.view.MotionEvent
import com.roblox.studiomobile.editor.ViewportCamera
import javax.microedition.khronos.egl.EGLConfig
import javax.microedition.khronos.opengles.GL10
class ViewportView(context:Context):GLSurfaceView(context){
 private val camera=ViewportCamera()
 private var lastX=0f
 private var lastY=0f
 private var active=false
 init{setEGLContextClientVersion(2);setRenderer(Renderer());renderMode=RENDERMODE_CONTINUOUSLY}
 override fun onTouchEvent(e:MotionEvent):Boolean{
  when(e.actionMasked){
   MotionEvent.ACTION_DOWN->{lastX=e.x;lastY=e.y;active=true}
   MotionEvent.ACTION_MOVE->if(active){val dx=e.x-lastX;val dy=e.y-lastY;camera.orbit(-dx*0.008f,-dy*0.008f);lastX=e.x;lastY=e.y}
   MotionEvent.ACTION_UP,MotionEvent.ACTION_CANCEL->active=false
  };return true
 }
 private inner class Renderer:GLSurfaceView.Renderer{
  override fun onSurfaceCreated(gl:GL10?,config:EGLConfig?){GLES20.glClearColor(.08f,.08f,.1f,1f);GLES20.glEnable(GLES20.GL_DEPTH_TEST)}
  override fun onSurfaceChanged(gl:GL10?,w:Int,h:Int){GLES20.glViewport(0,0,w,h)}
  override fun onDrawFrame(gl:GL10?){GLES20.glClear(GLES20.GL_COLOR_BUFFER_BIT or GLES20.GL_DEPTH_BUFFER_BIT)}
 }
}