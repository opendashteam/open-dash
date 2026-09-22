# PC targets will use SDL
if(
    ${TARGET_PLATFORM} STREQUAL "WINDOWS" OR
    ${TARGET_PLATFORM} STREQUAL "LINUX" OR
    ${TARGET_PLATFORM} STREQUAL "MAX"
)
    set(PLATFORM_SOURCES
        "${CMAKE_CURRENT_SOURCE_DIR}/sdl/Window.cpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/sdl/SDLGPUGraphics.cpp"
    )

    FetchContent_Declare(
        SDL3
        GIT_REPOSITORY https://github.com/libsdl-org/SDL.git
        GIT_TAG release-3.4.16
    )
    FetchContent_MakeAvailable(SDL3)

    FetchContent_Declare(
        SDL3_mixer
        GIT_REPOSITORY https://github.com/libsdl-org/SDL_mixer.git
        GIT_TAG release-3.2.4
    )
    FetchContent_MakeAvailable(SDL3_mixer)

    set(
        PLATFORM_LIBRARIES 
        SDL3::SDL3
        SDL3_mixer::SDL3_mixer
    )
endif()

add_library(opendash_platform STATIC ${PLATFORM_SOURCES})

target_include_directories(opendash_platform PUBLIC
    ${COMMON_INCLUDE}
)

target_link_libraries(opendash_platform PUBLIC
    ${COMMON_LIBRARIES}
    ${PLATFORM_LIBRARIES}
)