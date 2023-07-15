cmake_minimum_required(VERSION 3.23)
project(Island)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -O0")

add_executable(
        Island
        src/glad.c
        src/util/shaders.cpp
        src/util/Camera.cpp
        src/util/stb_image.cpp
        src/util/game_util.cpp
        src/util/debug.cpp
        src/util/materials.cpp
        src/util/Mesh.cpp
        src/util/Model.cpp
        src/util/Model.cpp
        src/util/BuiltinMesh.cpp
        src/util/shapes.cpp
        src/util/texture_util.cpp
        src/util/Screen.cpp
        src/util/SkyBox.cpp
        src/util/buffer_util.cpp
        src/util/light_util.cpp
        src/util/render_util.cpp
        src/util/ClientWorld.cpp
        src/main.cpp
)

INCLUDE_DIRECTORIES(include)
target_link_libraries(Island
        ${PROJECT_SOURCE_DIR}/lib/glfw3.dll
        ${PROJECT_SOURCE_DIR}/lib/libassimp.dll
        ${PROJECT_SOURCE_DIR}/lib/libassimp-5.dll
)


