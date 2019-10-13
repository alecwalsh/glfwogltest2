#pragma once

#include "ShaderManager.h"

class ConfigManager {
  private:
    void LoadVars();
    int getWidth() const;
    int getHeight() const;

  public:
    ConfigManager();
    ConfigManager(const char* fileName);

    int width = 800, height = 600;
    gl_version_t gl_version;
};
