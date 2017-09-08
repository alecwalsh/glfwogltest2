#include <unordered_map>
#include <functional>

#include <cassert>

#include "LuaScriptSet.h"

int call_cpp(lua_State* L) {
    int name_idx = lua_upvalueindex(1);
    int methodMap_idx = lua_upvalueindex(2);
    std::string name = lua_tostring(L, name_idx);
    
    auto methodMap = static_cast<std::unordered_map<std::string, std::function<void()>>*> (lua_touserdata(L, methodMap_idx));
    (*methodMap)[name]();
    return 0;
}

//TODO: add error handling
int set_cpp(lua_State* L) {
    int propertyMap_idx = lua_upvalueindex(1);
    std::string name = lua_tostring(L, 1);
    
    auto propertyMap = static_cast<std::unordered_map<std::string, std::pair<void*, int>>*>(lua_touserdata(L, propertyMap_idx));
    
    if(!propertyMap->count(name)) {
        printf("Error: Name has not been registered\n");
        return 0;
    }
    //A std::pair containg a pointer to the value and the type of the value
    auto propertyValueType = (*propertyMap)[name];
    
    //TODO: add more types
    switch(propertyValueType.second) {
        //TODO: handle floats
        case LUA_TNUMBER:
            *((int*)(*propertyMap)[name].first) = lua_tointeger(L, 2);
            break;
        case LUA_TSTRING:
            *((const char**)(*propertyMap)[name].first) = lua_tostring(L, 2);
            break;
        case LUA_TBOOLEAN:
            *((bool*)(*propertyMap)[name].first) = lua_toboolean(L, 2);
            break;
        default:
            break;
    }
    
    return 0;
}

//TODO: implement this
int get_cpp(lua_State* L) {
    int propertyMap_idx = lua_upvalueindex(1);
    std::string name = lua_tostring(L, 1);
    
    auto propertyMap = static_cast<std::unordered_map<std::string, std::pair<void*, int>>*>(lua_touserdata(L, propertyMap_idx));
    
    if(!propertyMap->count(name)) {
        printf("Error: Name has not been registered\n");
        return 0;
    }
    //A std::pair containg a pointer to the value and the type of the value
    auto propertyValueType = (*propertyMap)[name];
    
    //TODO: add more types
    switch(propertyValueType.second) {
        //TODO: handle floats
        case LUA_TNUMBER:
            lua_pushinteger(L, *((int*)(*propertyMap)[name].first));
            break;
        case LUA_TSTRING:
            lua_pushstring(L, *((const char**)(*propertyMap)[name].first));
            break;
        case LUA_TBOOLEAN:
            lua_pushboolean(L, *((bool*)(*propertyMap)[name].first));
            break;
        default:
            break;
    }
    
    return 1;
}
