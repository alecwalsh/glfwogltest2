#pragma once

#include <cstdint>

class GLBuffer final {
    std::uint32_t id = 0;
    
    friend void swap(GLBuffer& b1, GLBuffer& b2) noexcept;

    [[nodiscard]] GLBuffer(GLBuffer&& b) noexcept;
    GLBuffer& operator=(GLBuffer&& b) noexcept;

    void Delete() noexcept;
  public:
    GLBuffer() = default;

    // Copy constructor and assignment are deleted because you can't copy OpenGL objects
    GLBuffer(const GLBuffer&) = delete;
    GLBuffer& operator=(const GLBuffer&) = delete;

    ~GLBuffer();

    void GenBuffer();
    void Bind(std::uint32_t target);
};