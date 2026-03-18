cmake_minimum_required(VERSION 3.10)
project(SundownDrive
    VERSION 0.1.0
    DESCRIPTION "A minimalist 3D driving simulator with atmospheric sunset rendering."
    LANGUAGES CXX
)
set(CMAKE_CXX_STANDARD 17)

file(GLOB_RECURSE SRC_FILES
    ${CMAKE_SOURCE_DIR}/src/*.cpp
)
add_executable(SundownDrive
    main.cpp
    ${SRC_FILES}
)

target_include_directories(SundownDrive PRIVATE
    ${CMAKE_SOURCE_DIR}/src
    ${CMAKE_SOURCE_DIR}/external/freeglut/include
    ${CMAKE_SOURCE_DIR}/external/glew/include
    ${CMAKE_SOURCE_DIR}/external/glm
    ${CMAKE_SOURCE_DIR}/external/stb
    ${CMAKE_SOURCE_DIR}/external/tinyobjloader
)

target_link_libraries(SundownDrive PRIVATE
    ${CMAKE_SOURCE_DIR}/external/freeglut/lib/x64/freeglut.lib
    ${CMAKE_SOURCE_DIR}/external/glew/lib/Release/x64/glew32.lib
    opengl32
    glu32
)

target_compile_definitions(SundownDrive PRIVATE
    PROJECT_VERSION="${PROJECT_VERSION}"
)

# copy necessary DLLs to output directory after the build
add_custom_command(TARGET SundownDrive POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "${CMAKE_SOURCE_DIR}/external/freeglut/bin/x64/freeglut.dll"
        $<TARGET_FILE_DIR:SundownDrive>
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "${CMAKE_SOURCE_DIR}/external/glew/bin/Release/x64/glew32.dll"
        $<TARGET_FILE_DIR:SundownDrive>
)

# copy shaders and assets to output directory after the build
add_custom_command(TARGET SundownDrive POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${CMAKE_SOURCE_DIR}/shaders"
        "$<TARGET_FILE_DIR:SundownDrive>/shaders"
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${CMAKE_SOURCE_DIR}/assets"
        "$<TARGET_FILE_DIR:SundownDrive>/assets"
)