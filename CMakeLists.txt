cmake_minimum_required(VERSION 3.25)
project(OpenDash LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(GLM_BUILD_LIBRARY OFF CACHE BOOL "" FORCE)

include(FetchContent)

FetchContent_Declare(
    fmt
    GIT_REPOSITORY https://github.com/fmtlib/fmt
    GIT_TAG        1be298e1bd68957e4cd352e1f676f00e07dcfb57 # 12.2.0
)
FetchContent_MakeAvailable(fmt)

FetchContent_Declare(
    glm
    GIT_REPOSITORY https://github.com/g-truc/glm.git
    GIT_TAG 1.0.3
)
FetchContent_MakeAvailable(glm)

set(COMMON_LIBRARIES glm::glm-header-only fmt::fmt)

if(${CMAKE_SYSTEM_NAME} STREQUAL "Windows")
    set(TARGET_PLATFORM "WINDOWS")
elseif(${CMAKE_SYSTEM_NAME} STREQUAL "Linux")
    set(TARGET_PLATFORM "LINUX")
elseif(${CMAKE_SYSTEM_NAME} STREQUAL "Darwin")
    set(TARGET_PLATFORM "MAC")
endif()

if (NOT DEFINED MSVC)
    add_compile_options(-Wno-deprecated-declarations)
endif()

set(COMMON_INCLUDE
    "${CMAKE_CURRENT_SOURCE_DIR}/src"
    ${CMAKE_CURRENT_SOURCE_DIR}
)

add_subdirectory(src/platform)
add_subdirectory(src/engine)
add_subdirectory(src/game)

add_custom_target(copy_assets ALL
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_SOURCE_DIR}/assets
        $<TARGET_FILE_DIR:OpenDash>/assets
)
add_dependencies(OpenDash copy_assets)