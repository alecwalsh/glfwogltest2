#pragma once

#include "ShaderManager.h"

#include <stdexcept>
#include <cstdint>

class ConfigManager {
  public:
    ConfigManager() = default;

    // TODO: Read values from file
    ConfigManager(const char* fileName) : ConfigManager{} {
        throw std::runtime_error{"Not implemented yet"};
    }

    std::uint16_t width = 800;
    std::uint16_t height = 600;
    const gl_version_t gl_version = {3, 3, false};
};
