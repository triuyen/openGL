cmake_minimum_required(VERSION 3.16)
project(OpenGL3DScene)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Set build type if not specified
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

# Add compiler flags
set(CMAKE_CXX_FLAGS_DEBUG "-g -O0 -Wall -Wextra")
set(CMAKE_CXX_FLAGS_RELEASE "-O3 -DNDEBUG")

# Find required packages
find_package(OpenGL REQUIRED)
find_package(glfw3 REQUIRED)
find_package(GLEW REQUIRED)
find_package(glm REQUIRED)

# Alternative package finding for different systems
if(NOT TARGET glfw)
    find_package(PkgConfig REQUIRED)
    pkg_check_modules(GLFW3 REQUIRED glfw3)
endif()

# Source files
set(SOURCES
        src/main.cpp
        src/Shader.cpp
)

# Header files
set(HEADERS
        src/Shader.hpp
)

# Create executable
add_executable(${PROJECT_NAME} ${SOURCES} ${HEADERS})

# Include directories
target_include_directories(${PROJECT_NAME} PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src
        ${OPENGL_INCLUDE_DIRS}
)

# Link libraries
target_link_libraries(${PROJECT_NAME}
        OpenGL::GL
        glfw
        GLEW::GLEW
        glm::glm
)

# Platform-specific settings
if(WIN32)
    # Windows specific settings
    target_link_libraries(${PROJECT_NAME} opengl32)

    # Copy DLLs to output directory for Windows
    if(TARGET glfw)
        add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
                COMMAND ${CMAKE_COMMAND} -E copy_if_different
                $<TARGET_FILE:glfw> $<TARGET_FILE_DIR:${PROJECT_NAME}>)
    endif()

elseif(APPLE)
    # macOS specific settings
    find_library(COCOA_LIBRARY Cocoa)
    find_library(IOKIT_LIBRARY IOKit)
    find_library(COREVIDEO_LIBRARY CoreVideo)

    target_link_libraries(${PROJECT_NAME}
            ${COCOA_LIBRARY}
            ${IOKIT_LIBRARY}
            ${COREVIDEO_LIBRARY}
    )

elseif(UNIX)
    # Linux specific settings
    target_link_libraries(${PROJECT_NAME}
            ${CMAKE_DL_LIBS}
            pthread
            X11
            Xxf86vm
            Xrandr
            Xi
            Xinerama
            Xcursor
    )
endif()

# Create shaders directory in build folder
file(MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/shaders)

# Copy shader files to build directory
file(GLOB SHADER_FILES "${CMAKE_SOURCE_DIR}/shaders/*")
foreach(SHADER ${SHADER_FILES})
    file(COPY ${SHADER} DESTINATION ${CMAKE_BINARY_DIR}/shaders)
endforeach()

# Custom target to copy shaders during build
add_custom_target(copy_shaders ALL
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_SOURCE_DIR}/shaders
        ${CMAKE_BINARY_DIR}/shaders
        COMMENT "Copying shader files"
)
add_dependencies(${PROJECT_NAME} copy_shaders)

# Set working directory for Visual Studio
if(MSVC)
    set_property(TARGET ${PROJECT_NAME} PROPERTY VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_BINARY_DIR}")
endif()

# CLion-specific settings
if(CMAKE_GENERATOR STREQUAL "Ninja")
    # CLion uses Ninja by default, set working directory
    set_target_properties(${PROJECT_NAME} PROPERTIES
            RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}
    )
endif()

# Print some information
message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")
message(STATUS "C++ standard: ${CMAKE_CXX_STANDARD}")
message(STATUS "OpenGL found: ${OPENGL_FOUND}")
message(STATUS "GLFW found: ${glfw3_FOUND}")
message(STATUS "GLEW found: ${GLEW_FOUND}")

# Optional: Add installation rules
install(TARGETS ${PROJECT_NAME}
        RUNTIME DESTINATION bin
)

install(DIRECTORY shaders/
        DESTINATION bin/shaders
)