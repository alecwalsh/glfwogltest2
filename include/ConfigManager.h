#pragma once

#include <tuple>

class ConfigManager {
  private:
    void LoadVars();
    int getWidth() const;
    int getHeight() const;

  public:
    ConfigManager() = default;
    ConfigManager(const char* fileName);

    int width = 800;
    int height = 600;
    std::tuple<int, int, bool> gl_version = {3, 3, false};
};
