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
  reflection.register(ClassDescriptor("DataModel"))
  reflection.register(ClassDescriptor("Workspace").property(PropertyDescriptor("Gravity",PropertyType.Vector3,"196.2",true)))
  reflection.register(ClassDescriptor("Part").property(PropertyDescriptor("Anchored",PropertyType.Bool,false)).property(PropertyDescriptor("Name",PropertyType.String,"Part")))
  reflection.register(ClassDescriptor("Folder"))
  reflection.register(ClassDescriptor("Script"))
  val workspace=Instance("Workspace");workspace.rename("Workspace");workspace.setParent(dataModel);services.register(workspace)
 }
}
