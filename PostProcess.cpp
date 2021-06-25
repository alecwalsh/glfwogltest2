#include "PostProcess.hpp"

#include <stdexcept>

#include "Window.hpp"

#include "glad/glad.h"

//Vertices and indices for a full screen rectangle
static const GLfloat vertices[20]{1.0f,  1.0f,  0.0f, 1.0f, 1.0f, 1.0f,  -1.0f, 0.0f, 1.0f, 0.0f,
                           -1.0f, -1.0f, 0.0f, 0.0f, 0.0f, -1.0f, 1.0f,  0.0f, 0.0f, 1.0f};

static const GLubyte elements[6]{0, 1, 2, 0, 2, 3};

void PostProcess::Draw() {
    // Clear the screen to black
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    // Depth test is unnecessary here because we are rendering a single quad
    glDisable(GL_DEPTH_TEST);

    shaderProgram->UseProgram();
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, fbTexture);

    glBindVertexArray(vao);

    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_BYTE, 0);
}

PostProcess& PostProcess::GetInstance() {
    thread_local PostProcess f{};
    return f;
}

void PostProcess::BindFramebuffer() { glBindFramebuffer(GL_FRAMEBUFFER, fbo); }

void PostProcess::UnbindFramebuffer() { glBindFramebuffer(GL_FRAMEBUFFER, 0); }

void PostProcess::SetupFramebuffer() {
    glGenFramebuffers(1, &fbo);

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        throw std::runtime_error{"Error generating framebuffer"};
    }
    glBindFramebuffer(GL_FRAMEBUFFER, fbo);

    glGenTextures(1, &fbTexture);
    glBindTexture(GL_TEXTURE_2D, fbTexture);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    auto& window = Window::GetInstance();

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, window.width, window.height, 0, GL_RGB, GL_UNSIGNED_BYTE, nullptr);

    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, fbTexture, 0);

    glGenRenderbuffers(1, &rbo);
    glBindRenderbuffer(GL_RENDERBUFFER, rbo);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, window.width, window.height);
    glBindRenderbuffer(GL_RENDERBUFFER, 0);

    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, rbo);

    shaderProgram->UseProgram();
    glUniform1i(glGetUniformLocation(shaderProgram->shaderProgram, "texFramebuffer"), 0);
}

PostProcess::PostProcess() {
    shaderProgram =
        &shaderManager.AddShader({"shaders/vert_postprocess.glsl", "shaders/frag_postprocess_passthrough.glsl",
                                  Window::GetInstance().glVersion});
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    glGenBuffers(2, (GLuint*)&buffers);

    glBindBuffer(GL_ARRAY_BUFFER, buffers.vbo);
    glBufferData(GL_ARRAY_BUFFER, 20 * sizeof(vertices[0]), vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buffers.ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, 6 * sizeof(elements[0]), elements, GL_STATIC_DRAW);

    GLint posAttrib = VertexShaderAttribute::position;
    GLint texAttrib = VertexShaderAttribute::texcoord;

    glEnableVertexAttribArray(posAttrib);
    glVertexAttribPointer(posAttrib, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);

    glEnableVertexAttribArray(texAttrib);
    glVertexAttribPointer(texAttrib, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    glBindVertexArray(0);

    SetupFramebuffer();
}

PostProcess::~PostProcess() {
    glDeleteVertexArrays(1, &vao);
    glDeleteBuffers(2, (GLuint*)&buffers);
    glDeleteFramebuffers(1, &fbo);
}

void PostProcess::ReloadShader(ShaderIdentifier id) {
    shaderProgram = &shaderManager.AddShader(id);
}

void PostProcess::ReloadShader(const char* vertShader, const char* fragShader, GameEngine::GLVersion version) {
    ReloadShader({vertShader, fragShader, version});
}

void PostProcess::ReloadShader(const char* vertShader, const char* fragShader) {
    ReloadShader(vertShader, fragShader, Window::GetInstance().glVersion);
}

void PostProcess::Resize() {
    auto& window = Window::GetInstance();

    glBindTexture(GL_TEXTURE_2D, fbTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, window.width, window.height, 0, GL_RGB, GL_UNSIGNED_BYTE, nullptr);

    glBindRenderbuffer(GL_RENDERBUFFER, rbo);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, window.width, window.height);
}
