#include "LuaScript.h"

LuaScript::LuaScript() {
    L = luaL_newstate();
}

LuaScript::LuaScript(const char* fileName) {
    L = luaL_newstate();
    luaL_dofile(L, fileName);
}

LuaScript::~LuaScript() {
    lua_close(L);
}
