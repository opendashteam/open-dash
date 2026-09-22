file(GLOB_RECURSE ENGINE_SOURCES CONFIGURE_DEPENDS "*.cpp")

add_library(opendash_engine STATIC ${ENGINE_SOURCES})

target_include_directories(opendash_engine PUBLIC
    ${COMMON_INCLUDE}
)

target_link_libraries(opendash_engine PUBLIC
    opendash_platform
    ${COMMON_LIBRARIES}
)