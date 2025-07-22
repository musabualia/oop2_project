cmake_minimum_required (VERSION 3.26)

project (oop2_ex04)
set (MY_AUTHORS "Musa_AbuAlia-Ellen_Habash")

include (cmake/CompilerSettings.cmake)

add_executable (${CMAKE_PROJECT_NAME})

target_compile_options(${CMAKE_PROJECT_NAME} PRIVATE $<$<CONFIG:DEBUG>:-fsanitize=address>)
if (NOT MSVC)
    target_link_options(${CMAKE_PROJECT_NAME} PRIVATE $<$<CONFIG:DEBUG>:-fsanitize=address>)
endif()

# Add your subdirectories
add_subdirectory (include)
add_subdirectory (src)
add_subdirectory (resources)


# === FFmpeg Setup ===
set(FFMPEG_INCLUDE_DIR "${CMAKE_SOURCE_DIR}/external/ffmpeg/include")
set(FFMPEG_LIB_DIR "${CMAKE_SOURCE_DIR}/external/ffmpeg/lib")

include_directories(${FFMPEG_INCLUDE_DIR})
link_directories(${FFMPEG_LIB_DIR})

target_link_libraries(${CMAKE_PROJECT_NAME}
    PRIVATE
    ${CMAKE_SOURCE_DIR}/external/ffmpeg/lib/avformat.lib
    ${CMAKE_SOURCE_DIR}/external/ffmpeg/lib/avcodec.lib
    ${CMAKE_SOURCE_DIR}/external/ffmpeg/lib/avutil.lib
    ${CMAKE_SOURCE_DIR}/external/ffmpeg/lib/swscale.lib
)

# Copy FFmpeg DLLs to the binary directory
file(GLOB FFMPEG_DLLS
    "${CMAKE_SOURCE_DIR}/external/ffmpeg/bin/avformat-*.dll"
    "${CMAKE_SOURCE_DIR}/external/ffmpeg/bin/avcodec-*.dll"
    "${CMAKE_SOURCE_DIR}/external/ffmpeg/bin/avutil-*.dll"
    "${CMAKE_SOURCE_DIR}/external/ffmpeg/bin/swscale-*.dll"
    "${CMAKE_SOURCE_DIR}/external/ffmpeg/bin/swresample-*.dll"
    "${CMAKE_SOURCE_DIR}/external/ffmpeg/bin/avdevice-*.dll"
    "${CMAKE_SOURCE_DIR}/external/ffmpeg/bin/avfilter-*.dll"
)

foreach(dll ${FFMPEG_DLLS})
    configure_file(${dll} ${CMAKE_CURRENT_BINARY_DIR} COPYONLY)
endforeach()


foreach(dll ${FFMPEG_DLLS})
    configure_file(${dll} ${CMAKE_BINARY_DIR} COPYONLY)
endforeach()


# Box2D Setup
add_subdirectory (external/box2d/src)
target_include_directories(${CMAKE_PROJECT_NAME} PRIVATE external/box2d/include)
target_link_libraries(${CMAKE_PROJECT_NAME} PRIVATE box2d)

# SFML Setup - ADD AUDIO COMPONENT
set (SFML_LOCATION "C:/SFML/SFML-2.6.1")
set (SFML_DIR "${SFML_LOCATION}/lib/cmake/SFML")

# Fix: Add 'audio' component
find_package (SFML 2.6 COMPONENTS graphics audio REQUIRED)

set_target_properties (
    sfml-main sfml-system sfml-window sfml-graphics sfml-audio sfml-network 
    PROPERTIES MAP_IMPORTED_CONFIG_RELWITHDEBINFO RELEASE
)

# Fix: Link audio library
target_link_libraries (${CMAKE_PROJECT_NAME} PRIVATE sfml-graphics sfml-audio sfml-main)

include (cmake/SFML.cmake)
include (cmake/Zip.cmake)