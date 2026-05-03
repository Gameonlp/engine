cmake_minimum_required(VERSION 3.20)
project(MyEngine)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# SDL (vendored)
if (NOT TARGET SDL3::SDL3)
    add_subdirectory(vendored/SDL EXCLUDE_FROM_ALL)
endif()

# ----------------------
# Engine (library)
# ----------------------
add_library(engine
        src/engine/Game.h
        src/engine/GameConfig.h)
add_library(MyEngine)

target_sources(MyEngine
        PRIVATE
        src/engine/SDLMain.cpp
        src/engine/GameObject.cpp
        src/engine/Utils.cpp
        src/engine/Root.cpp
        src/engine/Timer.cpp
)

target_include_directories(MyEngine
        PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/src
)

target_link_libraries(MyEngine
        PUBLIC SDL3::SDL3
)

add_library(MyEngine::MyEngine ALIAS MyEngine)