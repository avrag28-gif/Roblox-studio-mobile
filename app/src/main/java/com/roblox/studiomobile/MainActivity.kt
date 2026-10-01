package com.roblox.studiomobile
import android.app.Activity
import android.os.Bundle
import android.graphics.Color
import android.view.Gravity
import android.widget.*
import com.roblox.studiomobile.core.*
class MainActivity:Activity(){override fun onCreate(b:Bundle?){super.onCreate(b);val dm=DataModel();val w=Instance("Workspace");w.setParent(dm);val root=LinearLayout(this).apply{orientation=LinearLayout.VERTICAL;setBackgroundColor(Color.rgb(24,24,27))};val bar=TextView(this).apply{text="  ROBLOX STUDIO MOBILE   |   Core 0.1";setTextColor(Color.WHITE);setGravity(Gravity.CENTER_VERTICAL);setBackgroundColor(Color.rgb(35,35,40));textSize=16f};root.addView(bar,LinearLayout.LayoutParams(-1,56));val body=LinearLayout(this).apply{orientation=LinearLayout.HORIZONTAL};val ex=TextView(this).apply{text="EXPLORER\n\n▾ Workspace";setTextColor(Color.WHITE);setPadding(18,18,18,18)};body.addView(ex,LinearLayout.LayoutParams(320,-1));val vp=TextView(this).apply{text="3D VIEWPORT\n\nDataModel: "+dm.name;setTextColor(Color.LTGRAY);gravity=Gravity.CENTER;textSize=18f};body.addView(vp,LinearLayout.LayoutParams(0,-1,1f));root.addView(body,LinearLayout.LayoutParams(-1,0,1f));setContentView(root)}}
