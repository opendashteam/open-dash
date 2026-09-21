file(GLOB_RECURSE GAME_SOURCES CONFIGURE_DEPENDS "*.cpp")

add_executable(OpenDash ${GAME_SOURCES})

target_link_libraries(OpenDash PRIVATE
    opendash_engine
)

add_custom_command(TARGET OpenDash POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        $<TARGET_RUNTIME_DLLS:OpenDash>
        $<TARGET_FILE_DIR:OpenDash>
    COMMAND_EXPAND_LISTS
)

target_include_directories(OpenDash PRIVATE
    ${CMAKE_SOURCE_DIR}/src/third_party
)