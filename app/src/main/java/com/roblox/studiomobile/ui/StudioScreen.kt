package com.roblox.studiomobile.ui
import android.content.Context
import android.graphics.Color
import android.view.Gravity
import android.widget.*
import com.roblox.studiomobile.core.*
import com.roblox.studiomobile.editor.*
class StudioScreen(private val context:Context,private val runtime:CoreRuntime){
 private val explorer=ExplorerModel(runtime.dataModel)
 private val list=LinearLayout(context).apply{orientation=LinearLayout.VERTICAL}
 private val properties=LinearLayout(context).apply{orientation=LinearLayout.VERTICAL}
 fun view():LinearLayout{
  val root=LinearLayout(context).apply{orientation=LinearLayout.VERTICAL;setBackgroundColor(Color.rgb(24,24,27))}
  val body=LinearLayout(context).apply{orientation=LinearLayout.HORIZONTAL}
  body.addView(explorerView(),LinearLayout.LayoutParams(0,-1,0.30f))
  val center=TextView(context).apply{text="3D VIEWPORT\n\nRuntime connected";gravity=Gravity.CENTER;textSize=18f;setTextColor(Color.LTGRAY)}
  body.addView(center,LinearLayout.LayoutParams(0,-1,0.45f))
  body.addView(propertyView(),LinearLayout.LayoutParams(0,-1,0.25f))
  root.addView(body,LinearLayout.LayoutParams(-1,0,1f));return root
 }
 private fun explorerView():ScrollView{
  list.removeAllViews()
  explorer.flatten().forEach{n->
   val b=Button(context).apply{text=("  ".repeat(n.depth))+n.instance.name;gravity=Gravity.START;setOnClickListener{runtime.selection.set(listOf(n.instance));showProperties(n.instance)}}
   list.addView(b,LinearLayout.LayoutParams(-1,48))
  }
  return ScrollView(context).apply{addView(list)}
 }
 private fun propertyView():ScrollView{properties.removeAllViews();return ScrollView(context).apply{addView(properties)}}
 private fun showProperties(i:Instance){
  properties.removeAllViews()
  PropertiesModel(runtime.reflection,runtime.properties).rows(i).forEach{row->
   val b=Button(context).apply{text=row.name+": "+(row.value ?: "");gravity=Gravity.START}
   properties.addView(b,LinearLayout.LayoutParams(-1,52))
  }
 }
}
