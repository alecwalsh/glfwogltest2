#pragma once
#include <unordered_map>
#include <functional>
#include <utility>

#include <lua.hpp>

#include "LuaScriptSet.h"

class LuaScript {
public:
    //Pointer to a member function
    template<typename R, typename T> using mptr_t = R(T::*);
    
    template<typename T>
    void Register(std::string name, T ptr, int type);
    
    template<typename R, typename T, typename... Args>
    void Register(std::string name, mptr_t<R,T> mptr, T* obj, Args&&... args);
    
    template<typename F>
    void Register(std::string name, F&& f);
    
    void exec(std::string code);
    
    LuaScript();
    LuaScript(std::string fileName);
    virtual ~LuaScript();
//TODO: make private again
public:
    void SetupBinding();
    lua_State* L;
    std::unordered_map<std::string, std::pair<void*, int>> propertyMap;
    std::unordered_map<std::string, std::function<void()>> methodMap;
};

template<typename T>
void LuaScript::Register(std::string name, T ptr, int type) {
    propertyMap.insert({name, {ptr, type}});
}

template<typename R, typename T, typename... Args>
void LuaScript::Register(std::string name, mptr_t<R,T> mptr, T* obj, Args&&... args) {
    //Creates a lambda that calls mptr on obj with args
    //Args is passed by reference, so you can change them after calling Register
    methodMap.insert({name, [=, &args...](){return (obj->*mptr)(std::forward<Args>(args)...);}});
    
    lua_pushstring(L, name.c_str());
    lua_pushlightuserdata(L, &methodMap);
    lua_pushcclosure(L, call_cpp, 2);
    lua_setglobal(L, name.c_str());
}

template<typename F>
void LuaScript::Register(std::string name, F&& f) {
    //Creates a lambda that calls mptr on obj with args
    //Args is passed by reference, so you can change them after calling Register
    methodMap.insert({name, std::function<void()>(f)});
    
    lua_pushstring(L, name.c_str());
    lua_pushlightuserdata(L, &methodMap);
    lua_pushcclosure(L, call_cpp, 2);
    lua_setglobal(L, name.c_str());
}
