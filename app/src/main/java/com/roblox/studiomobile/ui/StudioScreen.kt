package com.roblox.studiomobile.ui
import android.content.Context
import android.graphics.Color
import android.view.Gravity
import android.widget.*
import com.roblox.studiomobile.core.*
import com.roblox.studiomobile.editor.*
class StudioScreen(private val context:Context,private val runtime:CoreRuntime){
 private val explorer=ExplorerModel(runtime.dataModel)
 private val controller=ExplorerController(runtime)
 private val transform=TransformController(runtime)
 private val scene=SceneSynchronizer(runtime)
 private val list=LinearLayout(context).apply{orientation=LinearLayout.VERTICAL}
 private val properties=LinearLayout(context).apply{orientation=LinearLayout.VERTICAL}
 private var selected:Instance?=null
 fun view():LinearLayout{
  val root=LinearLayout(context).apply{orientation=LinearLayout.VERTICAL;setBackgroundColor(Color.rgb(24,24,27))}
  val toolbar=LinearLayout(context).apply{orientation=LinearLayout.HORIZONTAL}
  toolbar.addView(Button(context).apply{text="Folder";setOnClickListener{create("Folder")}},LinearLayout.LayoutParams(0,52,1f))
  toolbar.addView(Button(context).apply{text="Part";setOnClickListener{create("Part")}},LinearLayout.LayoutParams(0,52,1f))
  toolbar.addView(Button(context).apply{text="Undo";setOnClickListener{runtime.transactions.undo();refresh()}},LinearLayout.LayoutParams(0,52,1f))
  toolbar.addView(Button(context).apply{text="Redo";setOnClickListener{runtime.transactions.redo();refresh()}},LinearLayout.LayoutParams(0,52,1f))
  toolbar.addView(Button(context).apply{text="Move";setOnClickListener{transform.tool=TransformTool.Move}},LinearLayout.LayoutParams(0,52,1f))
  toolbar.addView(Button(context).apply{text="Scale";setOnClickListener{transform.tool=TransformTool.Scale}},LinearLayout.LayoutParams(0,52,1f))
  toolbar.addView(Button(context).apply{text="Select";setOnClickListener{transform.tool=TransformTool.Select}},LinearLayout.LayoutParams(0,52,1f))
  toolbar.addView(Button(context).apply{text="Delete";setOnClickListener{selected?.let{controller.delete(it);selected=null;refresh()}}},LinearLayout.LayoutParams(0,52,1f))
  root.addView(toolbar,LinearLayout.LayoutParams(-1,60))
  val body=LinearLayout(context).apply{orientation=LinearLayout.HORIZONTAL}
  body.addView(explorerView(),LinearLayout.LayoutParams(0,-1,0.30f))
  body.addView(viewport(),LinearLayout.LayoutParams(0,-1,0.45f))
  body.addView(propertyView(),LinearLayout.LayoutParams(0,-1,0.25f))
  root.addView(body,LinearLayout.LayoutParams(-1,0,1f))
  return root
 }
 private fun viewport():FrameLayout{
  val frame=FrameLayout(context)
  val camera=ViewportCamera()
  val surface=ViewportSurface(context,scene.graph,camera)
  surface.onPicked={select(it)}
  surface.onTransformGestureStart={transform.beginGesture()}
  surface.onTransformGestureEnd={commit->transform.endGesture(commit)}
  surface.onTransformDrag={dx,dy->selected?.let{transform.applyScreenDelta(it,dx,dy)} ?: false}
  frame.addView(surface,FrameLayout.LayoutParams(-1,-1))
  return frame
 }
 private fun targetParent():Instance=selected?:runtime.services.get<Instance>("Workspace")?:runtime.dataModel
 private fun create(className:String){val i=controller.create(className,targetParent());refresh();select(i)}
 private fun select(i:Instance){selected=i;controller.select(i);showProperties(i)}
 private fun refresh(){list.removeAllViews();explorer.flatten().forEach{n->list.addView(Button(context).apply{text=("  ".repeat(n.depth))+n.instance.name;gravity=Gravity.START;setOnClickListener{select(n.instance)}},LinearLayout.LayoutParams(-1,48))}}
 private fun explorerView():ScrollView=ScrollView(context).apply{addView(list);post{refresh()}}
 private fun propertyView():ScrollView=ScrollView(context).apply{addView(properties)}
 private fun showProperties(i:Instance){
  properties.removeAllViews()
  PropertiesModel(runtime.reflection,runtime.properties).rows(i).forEach{row->
   properties.addView(TextView(context).apply{text=row.name;setTextColor(Color.WHITE);setPadding(8,8,8,2)},LinearLayout.LayoutParams(-1,34))
   val edit=EditText(context).apply{setText(row.value?.toString()?:"");setSingleLine();setTextColor(Color.WHITE)}
   edit.setOnEditorActionListener{_,_,_->commit(i,row,edit.text.toString());true}
   properties.addView(edit,LinearLayout.LayoutParams(-1,52))
  }
 }
 private fun commit(i:Instance,row:PropertyRow,text:String){
  try{when(row.type){
   PropertyType.Bool->runtime.properties.set(i,row.name,text.equals("true",true)||text=="1")
   PropertyType.Float->runtime.properties.set(i,row.name,text.toFloat())
   PropertyType.Int->runtime.properties.set(i,row.name,text.toInt())
   PropertyType.Double->runtime.properties.set(i,row.name,text.toDouble())
   PropertyType.String->runtime.properties.set(i,row.name,text)
   else->runtime.properties.set(i,row.name,text)
  };refresh();showProperties(i)}catch(_:Exception){}
 }
}