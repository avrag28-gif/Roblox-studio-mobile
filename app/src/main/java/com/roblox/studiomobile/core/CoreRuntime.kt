package com.roblox.studiomobile.core
class CoreRuntime{
 val dataModel=DataModel()
 val reflection=ReflectionRegistry()
 val transactions=TransactionManager()
 val properties=PropertyStore(reflection,transactions)
 val selection=SelectionService()
 val services=ServiceRegistry()
 val factory=InstanceFactory(reflection)
 init{bootstrap()}
 private fun bootstrap(){
  reflection.register(ClassDescriptor("DataModel").property(PropertyDescriptor("Name",PropertyType.String,"DataModel",false)))
  reflection.register(ClassDescriptor("Workspace").property(PropertyDescriptor("Name",PropertyType.String,"Workspace",true)).property(PropertyDescriptor("Gravity",PropertyType.Float,196.2f,true)))
  reflection.register(ClassDescriptor("Folder").property(PropertyDescriptor("Name",PropertyType.String,"Folder",true)))
  reflection.register(ClassDescriptor("Model").property(PropertyDescriptor("Name",PropertyType.String,"Model",true)))
  reflection.register(ClassDescriptor("Part")
   .property(PropertyDescriptor("Name",PropertyType.String,"Part",true))
   .property(PropertyDescriptor("Anchored",PropertyType.Bool,false,true))
   .property(PropertyDescriptor("Transparency",PropertyType.Float,0f,true)))
  reflection.register(ClassDescriptor("Script").property(PropertyDescriptor("Name",PropertyType.String,"Script",true)))
  reflection.register(ClassDescriptor("LocalScript").property(PropertyDescriptor("Name",PropertyType.String,"LocalScript",true)))
  reflection.register(ClassDescriptor("ModuleScript").property(PropertyDescriptor("Name",PropertyType.String,"ModuleScript",true)))
  val workspace=Instance("Workspace");workspace.rename("Workspace");workspace.setParent(dataModel);services.register(workspace)
 }
}