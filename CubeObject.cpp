#include "CubeObject.h"

// Runs every frame
void CubeObject::Tick() {
    // TODO: move glGetUniformLocation calls outside of tick function; they only need to be called once per shader
    GLint uniTime = glGetUniformLocation(shaderProgram.shaderProgram, "time");

    float time = elapsedTime;

    bool ascending = ((int)time / 5) % 2 == 0;

    time = time / 5;
    if (ascending) {
        glUniform1f(uniTime, time - floor(time));
    } else {
        glUniform1f(uniTime, 1 - (time - floor(time)));
    }

    // std::cout << "Elapsed time:" << elapsedTime << std::endl;
}

void CubeObject::TickLua() {
    int starttop = lua_gettop(L);
    
    lua_getglobal(L, "cube");
    lua_getfield(L, -1, "tick");
    lua_pcall(L, 0, 1, 0);
    float i = (float)lua_tonumber(L, -1);
    
    glm::mat4 rotation, translation, scaling;

    rotation = glm::rotate(rotation, deltaTime * i * glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f));

    this->ModTransform(translation * rotation * scaling);
    
    Tick();
    
    lua_pop(L, 2);
    
    assert(lua_gettop(L) - starttop == 0);
}

void CubeObject::Draw(Camera camera) const {
    // Set material properties
    GLint matAmbientLoc = glGetUniformLocation(shaderProgram.shaderProgram, "material.ambient");
    GLint matDiffuseLoc = glGetUniformLocation(shaderProgram.shaderProgram, "material.diffuse");
    GLint matSpecularLoc = glGetUniformLocation(shaderProgram.shaderProgram, "material.specular");
    GLint matShineLoc = glGetUniformLocation(shaderProgram.shaderProgram, "material.shininess");

    glUniform3f(matAmbientLoc, material.ambient.r, material.ambient.g, material.ambient.b);
    glUniform3f(matDiffuseLoc, material.diffuse.r, material.diffuse.g, material.diffuse.b);
    glUniform3f(matSpecularLoc, material.specular.r, material.specular.g, material.specular.b);
    glUniform1f(matShineLoc, material.shininess);

    // Sets up cameraPos uniform then calls base class method
    GLint uniCameraPos = glGetUniformLocation(shaderProgram.shaderProgram, "cameraPos");
    glUniform3f(uniCameraPos, camera.position.x, camera.position.y, camera.position.z);

    GameObject::Draw(camera);
}

CubeObject::CubeObject(Mesh &_mesh, ShaderProgram &_shaderProgram, glm::mat4 _transform, float &_elapsedTime,
                       float &_deltaTime, TextureManager &_texman)
    : GameObject(_mesh, _shaderProgram, _transform, _elapsedTime, _deltaTime, _texman) {
    L = luaL_newstate();
    luaL_dofile(L, "cube.lua");
    
    // Sets up material properties for the cube
    material.ambient = glm::vec3(1.0f, 0.5f, 0.31f);
    material.diffuse = glm::vec3(1.0f, 0.5f, 0.31f);
    material.specular = glm::vec3(1.0f, 1.0f, 1.0f);
    material.shininess = 32.0f;
}

CubeObject::~CubeObject() {
    lua_close(L);
}
