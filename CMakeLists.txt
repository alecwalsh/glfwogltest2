cmake_minimum_required(VERSION 3.7.0)

project(glfwogltest2 VERSION 0.0.0.1)

set(GLM_PATH "${CMAKE_CURRENT_SOURCE_DIR}/../gl_libs/glm-0.9.9.5" CACHE PATH "")
set(GLFW_PATH "${CMAKE_CURRENT_SOURCE_DIR}/../gl_libs/glfw-3.3" CACHE PATH "")
set(ASSIMP_PATH "${CMAKE_CURRENT_SOURCE_DIR}/../gl_libs/assimp-4.1.0" CACHE PATH "")

set(BASE "/build")

file(GLOB SOURCES "*.cpp")
file(GLOB_RECURSE HEADERS "include/*.h")
set(SOURCES ${SOURCES} ${HEADERS} glad/glad.c)

link_directories(${BASE}/lib)

add_executable(glfwogltest2 ${SOURCES})

target_compile_features(glfwogltest2 PUBLIC cxx_std_17)
set_target_properties(glfwogltest2 PROPERTIES CXX_STANDARD_REQUIRED true)
set_target_properties(glfwogltest2 PROPERTIES CXX_EXTENSIONS false)

target_compile_options(glfwogltest2 PRIVATE "-Wall")
target_include_directories(glfwogltest2 PUBLIC "include" ".")

if(UNIX)
    target_link_libraries(glfwogltest2 GL glfw assimp dl)
elseif(MINGW)
    target_link_libraries(glfwogltest2 opengl32 glfw3 assimp IrrXML z)
elseif(WIN32)
    add_library(glfw3 STATIC IMPORTED)
    set_target_properties(glfw3 PROPERTIES IMPORTED_LOCATION ${GLFW_PATH}/build/src/debug/glfw3.lib)
    target_include_directories(glfw3 INTERFACE ${GLFW_PATH}/include)

    add_library(assimp STATIC IMPORTED)
    set_target_properties(assimp PROPERTIES IMPORTED_LOCATION ${ASSIMP_PATH}/build/code/debug/assimp-vc140-mt.lib)
    target_include_directories(assimp INTERFACE ${ASSIMP_PATH}/include ${ASSIMP_PATH}/build/include)

    set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT glfwogltest2)
    target_include_directories(glfwogltest2 PUBLIC ${GLM_PATH}/glm)
    target_link_libraries(glfwogltest2 opengl32 glfw3 assimp)
	
	add_custom_command(TARGET glfwogltest2 POST_BUILD 
               COMMAND ${CMAKE_COMMAND} -E copy_if_different
               ${ASSIMP_PATH}/build/code/debug/assimp-vc140-mt.dll
               $<TARGET_FILE_DIR:glfwogltest2>)
endif()

configure_file(include/version.h.in version.h)
target_include_directories(glfwogltest2 PUBLIC ${CMAKE_CURRENT_BINARY_DIR})

#Debug flags
if (CMAKE_BUILD_TYPE STREQUAL "Debug")
    target_compile_definitions(glfwogltest2 PUBLIC DEBUG)
endif()
