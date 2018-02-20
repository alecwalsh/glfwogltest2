#include "ConfigManager.h"

#include <iostream>

// TODO: add autoexec.lua with default values
ConfigManager::ConfigManager() : LuaScript("default.lua") { LoadVars(); }

ConfigManager::ConfigManager(const char* fileName) : LuaScript(fileName) { LoadVars(); }

ConfigManager::~ConfigManager() {}

int ConfigManager::getWidth() const {
    lua_getglobal(L, "width");
    // Check for incorrect type
    if (!lua_isinteger(L, -1)) {
        std::cout << "Error: Width must be an integer, using default value" << std::endl;
        // Clean up the stack
        lua_pop(L, 1);
        return width;
    }
    auto width = lua_tointeger(L, -1);

    // Clean up the stack
    lua_pop(L, 1);
    if (width < 1) {
        std::cout << "Error: Invalid width, using default value" << std::endl;
        width = this->width;
    }
    return width;
}

int ConfigManager::getHeight() const {
    lua_getglobal(L, "height");
    // Check for incorrect type
    if (!lua_isinteger(L, -1)) {
        std::cout << "Error: Height must be an integer, using default value" << std::endl;
        // Clean up the stack
        lua_pop(L, 1);
        return height;
    }
    auto height = lua_tointeger(L, -1);

    // Clean up the stack
    lua_pop(L, 1);
    if (height < 1) {
        std::cout << "Error: Invalid height, using default value" << std::endl;
        height = this->height;
    }
    return height;
}

void ConfigManager::LoadVars() {
    table_t table = get_lua_table(*this, "gl_version");
    
    auto get_int_index = [&](int i){
        return table[LuaValue{LUA_TNUMBER, static_cast<double>(i+1)}].value;
    };
    gl_version = {any_cast<LUA_NUMBER>(get_int_index(0)), any_cast<LUA_NUMBER>(get_int_index(1)), any_cast<bool>(get_int_index(2))};
//     std::cout << std::get<0>(gl_version) << std::get<1>(gl_version) << std::get<2>(gl_version) << std::endl;
    
    width = getWidth();
    height = getHeight();
}
