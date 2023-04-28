cmake_minimum_required(VERSION 3.23)
project(Island)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -O0")

add_executable(
        Island
        src/glad.c
        src/util/Shader.cpp
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
        src/main.cpp
)

#add_executable(
#        examples
#        src/glad.c
#        util/Shader.cpp
#        shape/Cube.cpp
#        shape/Polyhedron.cpp
#        shape/Polygon.cpp
#        util/Camera.cpp
#        util/game_util.cpp
#        examples/1.colors/colors.cpp
#)


INCLUDE_DIRECTORIES(include)
link_directories(lib)

target_link_libraries(Island
        ${PROJECT_SOURCE_DIR}/lib/glfw3.dll
        ${PROJECT_SOURCE_DIR}/lib/libassimp.dll
        ${PROJECT_SOURCE_DIR}/lib/libassimp-5.dll
)

