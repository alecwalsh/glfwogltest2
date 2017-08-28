#include "ConfigManager.h"

#include <iostream>

ConfigManager::ConfigManager() {
    L = luaL_newstate();
    //TODO: add autoexec.lua with default values
    luaL_dofile(L, "autoexec.lua");
    LoadVars();
}

ConfigManager::ConfigManager(const char* fileName) {
    L = luaL_newstate();
    luaL_dofile(L, fileName);
    LoadVars();
}

ConfigManager::~ConfigManager() {
    lua_close(L);
}

int ConfigManager::getWidth() {
    lua_getglobal(L, "width");
    if(!lua_isinteger(L, 1)) {
        std::cout << "Error: Width must be an integer, using default value" << std::endl;
        return width;
    }
    auto width = lua_tointeger(L, 1);
    lua_pop(L, 1);
    if(width < 1) {
        std::cout << "Error: Invalid width, using default value" << std::endl;
        return this->width;
    }
    return width;
}

int ConfigManager::getHeight() {
    lua_getglobal(L, "height");
    if(!lua_isinteger(L, 1)) {
        std::cout << "Error: Height must be an integer, using default value" << std::endl;
        return height;
    }
    auto height = lua_tointeger(L, 1);
    lua_pop(L, 1);
    if(height < 1) {
        std::cout << "Error: Invalid height, using default value" << std::endl;
        return this->height;
    }
    return height;
}

void ConfigManager::LoadVars() {
    width = getWidth();
    height = getHeight();
}
