package com.roblox.studiomobile.serialization
import com.roblox.studiomobile.core.*
import org.xmlpull.v1.XmlPullParser
import android.util.Xml
data class SerializedInstance(val className:String,val name:String,val properties:Map<String,String>,val children:List<SerializedInstance>)
class DocumentSerializer(private val reflection:ReflectionRegistry,private val properties:PropertyStore){
 fun snapshot(root:Instance):SerializedInstance=serialize(root)
 private fun serialize(i:Instance):SerializedInstance{
  val props=reflection.get(i.className)?.all()?.associate{d->d.name to (properties.get<Any>(i,d.name)?.toString()?:"")}?:emptyMap()
  return SerializedInstance(i.className,i.name,props,i.children.map(::serialize))
 }
 fun toXml(root:Instance):String{
  val out=StringBuilder().append("""<?xml version="1.0" encoding="utf-8"?>\n<roblox version="1">\n""")
  fun esc(v:String)=v.replace("&","&amp;").replace("<","&lt;").replace(">","&gt;").replace(""","&quot;")
  fun write(i:Instance,depth:Int){
   val pad="  ".repeat(depth)
   out.append(pad).append("<Item class="").append(esc(i.className)).append("" name="").append(esc(i.name)).append("">\n")
   reflection.get(i.className)?.all()?.forEach{d->
    val value=properties.get<Any>(i,d.name)?.toString()?:""
    out.append(pad).append("  <Property name="").append(esc(d.name)).append("">").append(esc(value)).append("</Property>\n")
   }
   i.children.forEach{write(it,depth+1)}
   out.append(pad).append("</Item>\n")
  }
  root.children.forEach{write(it,1)}
  return out.append("</roblox>\n").toString()
 }
 fun fromXml(xml:String):DataModel{
  val parser=Xml.newPullParser();parser.setInput(xml.reader())
  val model=DataModel();val stack=ArrayDeque<Instance>()
  var event=parser.eventType
  while(event!=XmlPullParser.END_DOCUMENT){
   if(event==XmlPullParser.START_TAG&&parser.name=="Item"){
    val i=Instance(parser.getAttributeValue(null,"class")?:"Folder")
    parser.getAttributeValue(null,"name")?.let{if(it.isNotBlank())i.rename(it)}
    (stack.lastOrNull()?:model).let{i.setParent(it)};stack.addLast(i)
   }else if(event==XmlPullParser.START_TAG&&parser.name=="Property"&&stack.isNotEmpty()){
    val name=parser.getAttributeValue(null,"name")?:""
    val value=parser.nextText()
    // Property values are applied by the caller because conversion is descriptor-specific.
   }else if(event==XmlPullParser.END_TAG&&parser.name=="Item"&&stack.isNotEmpty())stack.removeLast()
   event=parser.next()
  }
  return model
 }
}
