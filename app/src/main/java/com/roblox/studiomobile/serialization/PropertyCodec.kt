package com.roblox.studiomobile.serialization
import com.roblox.studiomobile.core.*
object PropertyCodec{
 fun encode(type:PropertyType,value:Any?):String=when(type){
  PropertyType.Bool->((value as? Boolean)?:false).toString()
  PropertyType.Int->((value as? Number)?.toInt()?:0).toString()
  PropertyType.Float->((value as? Number)?.toFloat()?:0f).toString()
  PropertyType.Double->((value as? Number)?.toDouble()?:0.0).toString()
  PropertyType.String,PropertyType.Enum->value?.toString()?:""
  else->value?.toString()?:""
 }
 fun decode(type:PropertyType,text:String):Any=when(type){
  PropertyType.Bool->text.equals("true",true)||text=="1"
  PropertyType.Int->text.toInt()
  PropertyType.Float->text.toFloat()
  PropertyType.Double->text.toDouble()
  PropertyType.String,PropertyType.Enum->text
  else->text
 }
}