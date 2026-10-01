package com.roblox.studiomobile.core
class Signal<T>{private val ls=mutableListOf<(T)->Unit>();fun connect(l:(T)->Unit):()->Unit{ls+=l;return{ls-=l}};fun fire(v:T){ls.toList().forEach{it(v)}}}
