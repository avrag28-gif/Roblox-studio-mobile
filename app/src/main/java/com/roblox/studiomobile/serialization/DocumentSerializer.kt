package com.roblox.studiomobile.serialization
import com.roblox.studiomobile.core.*
data class SerializedInstance(val className:String,val name:String,val properties:Map<String,String>,val children:List<SerializedInstance>)
class DocumentSerializer(private val reflection:ReflectionRegistry,private val properties:PropertyStore){
 fun snapshot(root:Instance):SerializedInstance=serialize(root)
 private fun serialize(i:Instance):SerializedInstance{
  val props=reflection.get(i.className)?.all()?.associate{d->d.name to (properties.get<Any>(i,d.name)?.toString() ?: "")}?:emptyMap()
  return SerializedInstance(i.className,i.name,props,i.children.map(::serialize))
 }
 fun toXml(root:Instance):String{
  val out=StringBuilder()
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
  write(root,0);return out.toString()
 }
 fun fromSnapshot(snapshot:SerializedInstance):Instance{
  val i=Instance(snapshot.className);i.rename(snapshot.name)
  snapshot.children.forEach{child->fromSnapshot(child).setParent(i)}
  return i
 }
}
