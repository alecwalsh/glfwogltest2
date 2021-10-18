#pragma once

#include <cstdint>

#include <string>

namespace GameEngine {

struct GLVersion {
    std::uint8_t major;
    std::uint8_t minor;
    bool is_gles; // OpenGL or OpenGL ES

    friend constexpr bool operator==(const GLVersion&, const GLVersion&) noexcept = default;
    
    std::string toString() const {
        using std::to_string;
    
        std::string versionString;
        
        versionString.reserve(18);
        
        versionString += "#version ";

        versionString += to_string(major) + to_string(minor) + '0';
        versionString += is_gles ? " es" : " core";
        versionString += '\n';
        
        return versionString;
    }
};

} // namespace GameEngine
