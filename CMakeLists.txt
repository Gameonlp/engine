cmake_minimum_required(VERSION 3.20)
project(MyEngine)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# SDL (vendored)
if (NOT TARGET SDL3::SDL3)
    add_subdirectory(vendored/SDL EXCLUDE_FROM_ALL)
endif()

# -----------------------------------------------------------------------------
# Engine Shader Embedding (GLSL -> SPIR-V -> C++ Header)
# -----------------------------------------------------------------------------
find_program(GLSLANG_VALIDATOR glslangValidator REQUIRED)

set(ENGINE_SHADER_SRC_DIR "${CMAKE_CURRENT_SOURCE_DIR}/src/engine/shaders")
set(GEN_SHADER_DIR "${CMAKE_CURRENT_BINARY_DIR}/generated_shaders")

file(GLOB RAW_SHADERS CONFIGURE_DEPENDS "${ENGINE_SHADER_SRC_DIR}/*.vert" "${ENGINE_SHADER_SRC_DIR}/*.frag")

set(SPV_BINARIES "")
foreach(SRC_FILE ${RAW_SHADERS})
    get_filename_component(FILE_NAME ${SRC_FILE} NAME)
    set(BIN_FILE "${GEN_SHADER_DIR}/${FILE_NAME}.spv")
    list(APPEND SPV_BINARIES ${BIN_FILE})

    add_custom_command(
            OUTPUT ${BIN_FILE}
            COMMAND ${CMAKE_COMMAND} -E make_directory ${GEN_SHADER_DIR}
            COMMAND ${GLSLANG_VALIDATOR} -V ${SRC_FILE} -o ${BIN_FILE}
            DEPENDS ${SRC_FILE}
            COMMENT "Compiling internal engine shader: ${FILE_NAME}"
    )
endforeach()

# 1. Convert the list to a space-separated string safely before the command runs
string(REPLACE ";" " " SHADER_LIST_STR "${SPV_BINARIES}")

set(EMBEDDED_HEADER "${GEN_SHADER_DIR}/EmbeddedShaders.h")
add_custom_command(
        OUTPUT ${EMBEDDED_HEADER}
        COMMAND ${CMAKE_COMMAND}
        -DINPUT_FILES=${SHADER_LIST_STR}
        -DOUTPUT_HEADER=${EMBEDDED_HEADER}
        -P "${CMAKE_CURRENT_SOURCE_DIR}/CMake/EmbedBinaries.cmake"
        DEPENDS ${SPV_BINARIES}
        COMMENT "Embedding SPIR-V binaries into C++ header"
)

# ----------------------
# Engine (library)
# ----------------------
add_library(MyEngine
        src/engine/Vertex.h
        src/engine/RenderItem.h
        src/engine/GPUBuffer.h
        src/engine/GraphicsCommand.h
        src/engine/GraphicsCommand.cpp
        src/engine/CopyPass.h
        src/engine/GPUSampler.h
        src/engine/utility/Movable.h
        src/engine/StringHash.h)

target_sources(MyEngine PRIVATE
        src/engine/Renderer.cpp
        # This is the anchor! It forces CMake to run the generator script
        # before trying to compile Renderer.cpp
        ${EMBEDDED_HEADER}
)

target_sources(MyEngine
        PRIVATE
        src/engine/Camera.cpp
        src/engine/SDLMain.cpp
        src/engine/GameObject.cpp
        src/engine/Utils.cpp
        src/engine/Root.cpp
        src/engine/Timer.cpp
        src/engine/Renderer.cpp
        src/engine/TileLayer.cpp
        src/engine/TileMap.cpp
)

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    target_compile_definitions(MyEngine PRIVATE DEBUG_BUILD=1)
endif()

target_include_directories(MyEngine
        PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src
        PRIVATE ${GEN_SHADER_DIR}
)

target_link_libraries(MyEngine
        PUBLIC SDL3::SDL3
)

add_library(MyEngine::MyEngine ALIAS MyEngine)