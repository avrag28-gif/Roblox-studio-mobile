package com.roblox.studiomobile.core

class PropertyStore(
    private val reflection: ReflectionRegistry,
    private val transactions: TransactionManager
) {
    private val values = mutableMapOf<String, MutableMap<String, Any?>>()
    val propertyChanged = Signal<Pair<Instance, String>>()

    fun <T> get(i: Instance, n: String): T? {
        if (n == "Name") {
            @Suppress("UNCHECKED_CAST")
            return i.name as T
        }
        val map = values[i.id]
        if (map != null && map.containsKey(n)) {
            @Suppress("UNCHECKED_CAST")
            return map[n] as T?
        }
        @Suppress("UNCHECKED_CAST")
        return reflection.get(i.className)?.get(n)?.defaultValue as T?
    }

    fun <T> set(i: Instance, n: String, v: T) {
        if (n == "Name") {
            val old = i.name
            require(v is String && v.isNotBlank()) { "Name must be a non-empty String" }
            if (old == v) return
            transactions.execute(object : Command {
                override fun execute() { i.rename(v) }
                override fun undo() { i.rename(old) }
            })
            return
        }
        val d = reflection.get(i.className)?.get(n) ?: error("Unknown property")
        require(d.writable) { "Property $n is read-only" }
        val old = get<T>(i, n)
        if (old == v) return
        transactions.execute(object : Command {
            override fun execute() { put(i, n, v) }
            override fun undo() { put(i, n, old) }
        })
    }

    private fun put(i: Instance, n: String, v: Any?) {
        values.getOrPut(i.id) { mutableMapOf() }[n] = v
        propertyChanged.fire(i to n)
        i.changed.fire(n)
    }
}
