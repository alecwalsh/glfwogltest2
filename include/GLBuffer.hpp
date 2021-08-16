#pragma once

#include <cstdint>

class GLBuffer {
    std::uint32_t id = 0;
    void Delete() noexcept;

  public:
    friend void swap(GLBuffer& b1, GLBuffer& b2) noexcept;

    GLBuffer() = default;

    // OpenGL objects don't support copying
    GLBuffer(const GLBuffer&) = delete;
    GLBuffer& operator=(const GLBuffer&) = delete;

    [[nodiscard]] GLBuffer(GLBuffer&& b) noexcept;
    GLBuffer& operator=(GLBuffer&& b) noexcept;

    ~GLBuffer();

    void GenBuffer();
    void Bind(std::uint32_t target);
};