#pragma once

#include <cstdint>

namespace GameEngine {

struct GLVersion {
    std::uint8_t major;
    std::uint8_t minor;
    bool is_gles; // OpenGL or OpenGL ES

#ifdef __cpp_impl_three_way_comparison
    friend constexpr bool operator==(const GLVersion& lhs, const GLVersion& rhs) noexcept = default;
#else
    friend constexpr bool operator==(const GLVersion& lhs, const GLVersion& rhs) noexcept {
        return lhs.major == rhs.major && lhs.minor == rhs.minor && lhs.is_gles == rhs.is_gles;
    }
#endif // __cpp_impl_three_way_comparison
};

} // namespace GameEngine