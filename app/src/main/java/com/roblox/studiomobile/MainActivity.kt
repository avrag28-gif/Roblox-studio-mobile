package com.roblox.studiomobile
import android.app.Activity
import android.os.Bundle
import com.roblox.studiomobile.core.CoreRuntime
import com.roblox.studiomobile.ui.StudioScreen
class MainActivity:Activity(){
 private lateinit var runtime:CoreRuntime
 override fun onCreate(state:Bundle?){super.onCreate(state);runtime=CoreRuntime();setContentView(StudioScreen(this,runtime).view())}
}
