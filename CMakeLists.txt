cmake_minimum_required(VERSION 3.20)
project(todoapp CXX)

set(SRC_DIR "${PROJECT_SOURCE_DIR}/src")

file(GLOB_RECURSE SRC_MAIN CONFIGURE_DEPENDS
    "${SRC_DIR}/**.hpp"
    "${SRC_DIR}/**.h"
    "${SRC_DIR}/**.cpp"
    "${SRC_DIR}/**.rc"
)

source_group(TREE ${SRC_DIR} PREFIX "src" FILES ${SRC_MAIN})

message("\nFetching ImGui module...\n")
include(libs/imgui.cmake)

message("\nFetching Json module...\n")
include(libs/json.cmake)

add_executable(todoapp "${SRC_MAIN}")

set_property(GLOBAL PROPERTY USE_FOLDERS ON)
set_property(TARGET todoapp PROPERTY CXX_STANDARD 23)
target_precompile_headers(todoapp PRIVATE "${SRC_DIR}/common.hpp")
set_target_properties(todoapp PROPERTIES WIN32_EXECUTABLE TRUE)

target_include_directories(todoapp PRIVATE
    "${SRC_DIR}"
    "${imgui_SOURCE_DIR}"
)

target_link_libraries(todoapp PRIVATE
    imgui
    d3d11
    dxgi
    nlohmann_json::nlohmann_json
)

set_property(TARGET todoapp PROPERTY COMPILE_WARNING_AS_ERROR ON)

target_compile_definitions(todoapp PRIVATE
    NOMINMAX
    WIN32_LEAN_AND_MEAN
)

target_link_options(todoapp PRIVATE  "/FI winapifamily.h")