#pragma once

#include "glad/glad.h"

#include "ShaderManager.hpp"
#include "Window.hpp"

// Used for postprocessing effects
// Renders the main scene to a texture and applies that texture to a fullscreen quad
class PostProcess {
    ShaderProgram* shaderProgram;
    GLuint vao;
    struct buffers {
        GLuint vbo;
        GLuint ebo;
    } buffers;
    GLuint fbo;
    GLuint rbo;       // Renderbuffer for depth attachment
    GLuint fbTexture; // Texture for color attachment
    void SetupFramebuffer();

    // gl_version needs to be set before insantiating this struct with the default constructor
    [[nodiscard]] PostProcess();

    // Deleted to prevent copies
    PostProcess(const PostProcess&) = delete;
    void operator=(const PostProcess&) = delete;

    ~PostProcess();
  public:
    [[nodiscard]] static PostProcess& GetInstance();
    void BindFramebuffer();
    void UnbindFramebuffer();
    void Draw();

    // TODO: Support multiple postprocess effects
    // Replaces the current postprocess shader with a different shader
    void ReloadShader(const char* vertShader, const char* fragShader, gl_version_t version);
    // Resize framebuffer texture and renderbuffer to match the current window size
    void Resize();
};
