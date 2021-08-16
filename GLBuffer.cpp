#include "GLBuffer.hpp"

#include "glad/glad.h"

#include <iostream>
#include <utility>
#include <stdexcept>

void swap(GLBuffer& b1, GLBuffer& b2) noexcept {
    using std::swap;

    swap(b1.id, b2.id);
}

[[nodiscard]] GLBuffer::GLBuffer(GLBuffer&& b) noexcept { *this = std::move(b); }

GLBuffer& GLBuffer::operator=(GLBuffer&& b) noexcept {
    if (this != &b) {
        Delete();

        swap(*this, b);
    }

    return *this;
}

GLBuffer::~GLBuffer() { Delete(); }

void GLBuffer::Delete() noexcept {
    if (id != 0) {
        std::cout << "Deleting buffer " << id << std::endl;

        glDeleteBuffers(1, &id);
        id = 0;
    }
}

void GLBuffer::Bind(std::uint32_t target) { glBindBuffer(target, id); }

void GLBuffer::GenBuffer() {
    if (id != 0) {
        throw std::runtime_error{"Buffer has already been generated"};
    }
    glGenBuffers(1, &id);

    std::cout << "Created buffer " << id << std::endl;
}