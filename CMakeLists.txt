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
add_library(engine)

target_sources(engine
        PRIVATE
        src/engine/GameObject.cpp
        src/engine/Utils.cpp
        src/engine/Root.cpp
        src/engine/Timer.cpp
)

target_include_directories(engine
        PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/src
)

target_link_libraries(engine
        PUBLIC SDL3::SDL3
)