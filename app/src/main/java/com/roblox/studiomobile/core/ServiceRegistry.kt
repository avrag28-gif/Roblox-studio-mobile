package com.roblox.studiomobile.core
class ServiceRegistry{private val services=linkedMapOf<String,Instance>();fun register(i:Instance){services[i.className]=i};fun <T:Instance>get(n:String):T?{ @Suppress("UNCHECKED_CAST") return services[n] as T? };fun all():List<Instance>=services.values.toList()}
