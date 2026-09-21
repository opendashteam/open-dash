file(GLOB_RECURSE ENGINE_SOURCES CONFIGURE_DEPENDS "*.cpp")

add_library(opendash_engine STATIC ${ENGINE_SOURCES})

target_include_directories(opendash_engine PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(opendash_engine PUBLIC
    SDL3::SDL3
    SDL3_mixer::SDL3_mixer
    glm::glm-header-only
)