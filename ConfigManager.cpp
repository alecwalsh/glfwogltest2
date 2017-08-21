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
    //TODO: Use lua_isinteger to detect type errors
    auto width = lua_tointeger(L, 1);
    lua_pop(L, 1);
    if(width == 0) {
        throw std::runtime_error("Invalid width");
    }
    return width;
}

int ConfigManager::getHeight() {
    lua_getglobal(L, "height");
    //TODO: Use lua_isinteger to detect type errors
    auto height = lua_tointeger(L, 1);
    lua_pop(L, 1);
    if(height == 0) {
        throw std::runtime_error("Invalid height");
    }
    return height;
}

void ConfigManager::LoadVars() {
    bool validWidth = true, validHeight = true;
    try {
        width = getWidth();
    } catch(std::runtime_error& e) {
        validWidth = false;
    }
    try {
        height = getHeight();
    } catch(std::runtime_error& e) {
        validHeight = false;
    }
    
    //Construct appropriate error message
    if(!validWidth && !validHeight) {
        std::cout << "Error: Invalid width and height, using default values" << std::endl;
    } else if(!validWidth) {
        std::cout << "Error: Invalid width, using default value" << std::endl;
    } else if(!validHeight) {
        std::cout << "Error: Invalid height, using default value" << std::endl;
    }
}
