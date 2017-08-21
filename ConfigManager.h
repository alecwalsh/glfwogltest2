#pragma once
#include <lua.hpp>

#include <string>
#include <stdexcept>

class ConfigManager {
private:
    lua_State* L;
    void LoadVars();
    int getWidth();
    int getHeight();
public:
    ConfigManager();
    ConfigManager(const char* fileName);
    ~ConfigManager();
    
    int width = 800, height = 600;
};
