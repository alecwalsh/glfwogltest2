#pragma once

#include "LuaScript.h"
#include "LuaTable.h"
#include "LuaValue.h"

#include <tuple>

class ConfigManager : public LuaScript {
  private:
    void LoadVars();
    int getWidth() const;
    int getHeight() const;

  public:
    ConfigManager();
    ConfigManager(const char* fileName);
    virtual ~ConfigManager() override;

    int width = 800, height = 600;
    std::tuple<int, int, bool> gl_version;
};
