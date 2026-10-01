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
 private var selected:Instance?=null
 fun view():LinearLayout{
  val root=LinearLayout(context).apply{orientation=LinearLayout.VERTICAL;setBackgroundColor(Color.rgb(24,24,27))}
  val toolbar=LinearLayout(context).apply{orientation=LinearLayout.HORIZONTAL}
  toolbar.addView(Button(context).apply{text="Folder";setOnClickListener{create("Folder")}},LinearLayout.LayoutParams(0,52,1f))
  toolbar.addView(Button(context).apply{text="Part";setOnClickListener{create("Part")}},LinearLayout.LayoutParams(0,52,1f))
  toolbar.addView(Button(context).apply{text="Delete";setOnClickListener{selected?.let{delete(it)}}},LinearLayout.LayoutParams(0,52,1f))
  root.addView(toolbar,LinearLayout.LayoutParams(-1,60))
  val body=LinearLayout(context).apply{orientation=LinearLayout.HORIZONTAL}
  body.addView(explorerView(),LinearLayout.LayoutParams(0,-1,0.30f))
  body.addView(TextView(context).apply{text="3D VIEWPORT\n\nSceneGraph pending";gravity=Gravity.CENTER;textSize=18f;setTextColor(Color.LTGRAY)},LinearLayout.LayoutParams(0,-1,0.45f))
  body.addView(propertyView(),LinearLayout.LayoutParams(0,-1,0.25f))
  root.addView(body,LinearLayout.LayoutParams(-1,0,1f))
  return root
 }
 private fun targetParent():Instance=selected?:runtime.services.get<Instance>("Workspace")?:runtime.dataModel
 private fun create(className:String){val i=runtime.factory.create(className);i.setParent(targetParent());refresh();select(i)}
 private fun delete(i:Instance){if(i.className=="Workspace")return;i.setParent(null);runtime.selection.remove(i);selected=null;refresh()}
 private fun select(i:Instance){selected=i;runtime.selection.set(listOf(i));showProperties(i)}
 private fun refresh(){list.removeAllViews();explorer.flatten().forEach{n->list.addView(Button(context).apply{text=("  ".repeat(n.depth))+n.instance.name;gravity=Gravity.START;setOnClickListener{select(n.instance)}},LinearLayout.LayoutParams(-1,48))}}
 private fun explorerView():ScrollView=ScrollView(context).apply{addView(list);post{refresh()}}
 private fun propertyView():ScrollView=ScrollView(context).apply{addView(properties)}
 private fun showProperties(i:Instance){
  properties.removeAllViews()
  PropertiesModel(runtime.reflection,runtime.properties).rows(i).forEach{row->
   val label=TextView(context).apply{text=row.name;setTextColor(Color.WHITE);setPadding(8,8,8,2)}
   properties.addView(label,LinearLayout.LayoutParams(-1,34))
   if(row.type==PropertyType.Bool){
    properties.addView(CheckBox(context).apply{text="Enabled";isChecked=(row.value as? Boolean)==true;setOnCheckedChangeListener{_,v->runtime.properties.set(i,row.name,v)}},LinearLayout.LayoutParams(-1,52))
   }else{
    properties.addView(EditText(context).apply{setText(row.value?.toString()?:"");setSingleLine();setTextColor(Color.WHITE);setOnEditorActionListener{_,_,_->commit(i,row);true}},LinearLayout.LayoutParams(-1,52))
   }
  }
 }
 private fun commit(i:Instance,row:PropertyRow){val raw=(properties.getChildAt(properties.indexOfChild(properties.findViewWithTag("none"))));}
}