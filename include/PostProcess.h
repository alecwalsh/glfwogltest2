#pragma once

#include "glad/glad.h"

#include "ShaderProgram.h"
#include "Window.h"

// Used for postprocessing effects
// Renders the main scene to a texture and applies that texture to a fullscreen quad
struct PostProcess {
    static PostProcess& GetInstance();
    void BindFramebuffer();
    void UnbindFramebuffer();
    void Draw();

    // Deleted to prevent copies
    PostProcess(const PostProcess&) = delete;
    void operator=(const PostProcess&) = delete;

  private:
    ShaderProgram shaderProgram;
    GLuint vao;
    struct buffers {
        GLuint vbo;
        GLuint ebo;
    } buffers;
    GLuint fbo;
    GLuint rbo;        // Renderbuffer for depth attachment
    GLuint fb_texture; // Texture for color attachment
    void SetupFramebuffer();

    // gl_version needs to be set before insantiating this struct with the default constructor
    PostProcess();
    ~PostProcess();

  public:
    // TODO: Support multiple postprocess effects
    // TODO: Don't immediately delete shaders, they might be used later
    // Replaces the current postprocess shader with a different shader
    void ReloadShader(const char* vertShader, const char* fragShader, gl_version_t version);
    // Resize framebuffer texture and renderbuffer to match the current window size
    void Resize();
};
