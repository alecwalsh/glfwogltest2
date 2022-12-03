#include "GLBuffer.hpp"

#include "glad/glad.h"

#include <utility>
#include <stdexcept>

#include <cassert>

#include <spdlog/spdlog.h>

void swap(GLBuffer& b1, GLBuffer& b2) noexcept {
    using std::swap;

    swap(b1.id, b2.id);
}

[[nodiscard]] GLBuffer::GLBuffer(GLBuffer&& b) noexcept { *this = std::move(b); }

GLBuffer& GLBuffer::operator=(GLBuffer&& b) noexcept {
    if (this != &b) {
        Delete();

        using std::swap;
        swap(*this, b);

        assert(b.id == 0);
    }

    return *this;
}

void GLBuffer::Delete() noexcept {
    if (id != 0) {
        spdlog::info("Deleting buffer {}", id);

        glDeleteBuffers(1, &id);
        id = 0;
    }
}

GLBuffer::~GLBuffer() { Delete(); }

void GLBuffer::Bind(std::uint32_t target) { glBindBuffer(target, id); }

void GLBuffer::GenBuffer() {
    if (id != 0) {
        throw std::runtime_error{"Buffer has already been generated"};
    }
    glGenBuffers(1, &id);

    spdlog::info("Created buffer {}", id);
}