file(GLOB_RECURSE GAME_SOURCES CONFIGURE_DEPENDS "*.cpp")

set(GAME_EXECUTABLE OpenDash)

if(TARGET_PLATFORM STREQUAL "MAC" AND CREATE_MAC_BUNDLE)
    file(GLOB_RECURSE MAC_APP_ASSETS CMAKE_CONFIGURE_DEPENDS
        assets/*.png
        assets/*.fnt
        assets/*.plist
        assets/*.mp3
        assets/*.ogg
    )
    
    file(GLOB_RECURSE MAC_APP_ICON_ASSETS CMAKE_CONFIGURE_DEPENDS assets/icons/*.plist assets/icons/*.png)
    file(GLOB_RECURSE MAC_APP_LEVEL_ASSETS CMAKE_CONFIGURE_DEPENDS assets/levels/*.txt)
    file(GLOB_RECURSE MAC_APP_SHADER_ASSETS CMAKE_CONFIGURE_DEPENDS assets/shaders/msl/*.msl)

    add_executable(${GAME_EXECUTABLE} MACOSX_BUNDLE 
        ${GAME_SOURCES}
        ${MAC_APP_ASSETS}
        ${MAC_APP_ICON_ASSETS}
        ${MAC_APP_LEVEL_ASSETS}
        ${MAC_APP_SHADER_ASSETS}
    )

    set_target_properties(${GAME_EXECUTABLE} PROPERTIES
        BUNDLE True
        MACOSX_BUNDLE_BUNDLE_NAME ${GAME_EXECUTABLE}
        MACOSX_BUNDLE_BUNDLE_VERSION ${CMAKE_PROJECT_VERSION}
        MACOSX_BUNDLE_GUI_IDENTIFIER app.teamopendash.open-dash
#        MACOSX_BUNDLE_ICON_FILE AppIcon # NO APP ICON YET
        MACOSX_BUNDLE_SHORT_VERSION_STRING ${CMAKE_PROJECT_VERSION}
    )

    set_source_files_properties(${MAC_APP_ASSETS} PROPERTIES MACOSX_PACKAGE_LOCATION "Resources/assets")
    set_source_files_properties(${MAC_APP_ICON_ASSETS} PROPERTIES MACOSX_PACKAGE_LOCATION "Resources/assets/icons")
    set_source_files_properties(${MAC_APP_LEVEL_ASSETS} PROPERTIES MACOSX_PACKAGE_LOCATION "Resources/assets/levels")
    set_source_files_properties(${MAC_APP_SHADER_ASSETS} PROPERTIES MACOSX_PACKAGE_LOCATION "Resources/assets/shaders")
else()
    add_executable(${GAME_EXECUTABLE} ${GAME_SOURCES})
endif()

target_link_libraries(OpenDash PRIVATE
    opendash_engine
)

if(TARGET_PLATFORM STREQUAL "WINDOWS")
    add_custom_command(TARGET ${GAME_EXECUTABLE} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
            $<TARGET_RUNTIME_DLLS:OpenDash>
            $<TARGET_FILE_DIR:OpenDash>
        COMMAND_EXPAND_LISTS
    )
endif()

target_include_directories(${GAME_EXECUTABLE} PRIVATE
    ${CMAKE_SOURCE_DIR}/src/third_party
)