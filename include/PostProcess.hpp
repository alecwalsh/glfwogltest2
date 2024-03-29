#pragma once

#include "ShaderManager.hpp"
#include "GLBuffer.hpp"

#include <cstdint>

// Used for postprocessing effects
// Renders the main scene to a texture and applies that texture to a fullscreen quad
class PostProcess {
    ShaderProgram* shaderProgram;
    std::uint32_t vao;
    struct {
        GLBuffer vbo;
        GLBuffer ebo;
    } buffers;
    std::uint32_t fbo;
    std::uint32_t rbo; // Renderbuffer for depth attachment
    std::uint32_t fbTexture; // Texture for color attachment

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
    void ReloadShader(std::string_view name);
    
    // Resize framebuffer texture and renderbuffer to match the current window size
    void Resize();
};
