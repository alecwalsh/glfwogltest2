#pragma once

#include <cstdint>

namespace GameEngine {

struct GLVersion {
    std::uint8_t major;
    std::uint8_t minor;
    bool is_gles; // OpenGL or OpenGL ES

    friend constexpr bool operator==(const GLVersion&, const GLVersion&) noexcept = default;
};

} // namespace GameEngine
