#pragma once

#include "GLVersion.hpp"

#include <stdexcept>
#include <cstdint>

#include <spdlog/spdlog.h>

class ConfigManager {
  public:
    ConfigManager() = default;

    // TODO: Read values from file
    ConfigManager([[maybe_unused]] const char* fileName) : ConfigManager{} {
        throw std::runtime_error{"Not implemented yet"};
    }

    std::uint16_t width = 800;
    std::uint16_t height = 600;
    GameEngine::GLVersion glVersion = {3, 3, false};

    spdlog::level::level_enum logLevel = spdlog::level::info;
};
