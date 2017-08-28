#pragma once
#include <lua.hpp>

class LuaScript {
protected:
    lua_State* L;
public:
    LuaScript();
    LuaScript(const char* fileName);
    
    virtual ~LuaScript();
};
