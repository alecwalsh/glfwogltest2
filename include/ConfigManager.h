#pragma once

#include "ShaderManager.h"

#include <stdexcept>

class ConfigManager {
  public:
    ConfigManager() = default;

    // TODO: Read values from file
    ConfigManager(const char* fileName) : ConfigManager{} {
        throw std::runtime_error{"Not implemented yet"};
    }

    int width = 800;
    int height = 600;
    const gl_version_t gl_version = {3, 3, false};
};
