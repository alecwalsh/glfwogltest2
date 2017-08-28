#include "ConfigManager.h"

#include <iostream>

//TODO: add autoexec.lua with default values
ConfigManager::ConfigManager() : LuaScript("autoexec.lua") {
    LoadVars();
}

ConfigManager::ConfigManager(const char* fileName) : LuaScript(fileName) {
    LoadVars();
}

ConfigManager::~ConfigManager() {
    
}

int ConfigManager::getWidth() const {
    lua_getglobal(L, "width");
    //Check for incorrect type
    if(!lua_isinteger(L, 1)) {
        std::cout << "Error: Width must be an integer, using default value" << std::endl;
        //Clean up the stack
        lua_pop(L, 1);
        return width;
    }
    auto width = lua_tointeger(L, 1);
    
    //Clean up the stack
    lua_pop(L, 1);
    if(width < 1) {
        std::cout << "Error: Invalid width, using default value" << std::endl;
        width = this->width;
    }
    return width;
}

int ConfigManager::getHeight() const {
    lua_getglobal(L, "height");
    //Check for incorrect type
    if(!lua_isinteger(L, -1)) {
        std::cout << "Error: Height must be an integer, using default value" << std::endl;
        //Clean up the stack
        lua_pop(L, 1);
        return height;
    }
    auto height = lua_tointeger(L, 1);
    
    //Clean up the stack
    lua_pop(L, 1);
    if(height < 1) {
        std::cout << "Error: Invalid height, using default value" << std::endl;
        height = this->height;
    }
    return height;
}

void ConfigManager::LoadVars() {
    width = getWidth();
    height = getHeight();
}
