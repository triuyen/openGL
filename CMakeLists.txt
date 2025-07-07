cmake_minimum_required(VERSION 3.16)
project(OpenGL3DScene)

# Set C++ standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Set build type if not specified
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

# Add compiler flags
set(CMAKE_CXX_FLAGS_DEBUG "-g -O0 -Wall -Wextra")
set(CMAKE_CXX_FLAGS_RELEASE "-O3 -DNDEBUG")

# Find OpenGL (system package)
find_package(OpenGL REQUIRED)

# Try to find system packages first
find_package(glfw3 QUIET)
find_package(GLEW QUIET)
find_package(glm QUIET)
find_package(imgui QUIET)

# If system packages not found, set up local libraries
if(NOT TARGET glfw)
    message(STATUS "Using local GLFW library")
    add_library(glfw3_local STATIC IMPORTED)
    set_target_properties(glfw3_local PROPERTIES
            IMPORTED_LOCATION "${CMAKE_SOURCE_DIR}/libs/glfw/lib-static-ucrt/glfw3.lib"
            INTERFACE_INCLUDE_DIRECTORIES "${CMAKE_SOURCE_DIR}/libs/glfw/include"
    )
    add_library(glfw ALIAS glfw3_local)
endif()

if(NOT TARGET GLEW::GLEW)
    message(STATUS "Using local GLEW library")
    add_library(glew_local STATIC IMPORTED)
    set_target_properties(glew_local PROPERTIES
            IMPORTED_LOCATION "${CMAKE_SOURCE_DIR}/libs/glew/lib/release/x64/glew32s.lib"
            INTERFACE_INCLUDE_DIRECTORIES "${CMAKE_SOURCE_DIR}/libs/glew/include"
            INTERFACE_COMPILE_DEFINITIONS "GLEW_STATIC"
    )
    add_library(GLEW::GLEW ALIAS glew_local)
endif()

if(NOT TARGET glm::glm)
    message(STATUS "Looking for local GLM library")
    find_path(GLM_INCLUDE_DIR glm/glm.hpp
            PATHS
            "${CMAKE_SOURCE_DIR}/libs/glm"
            "${CMAKE_SOURCE_DIR}/libs"
            "C:/vcpkg/installed/x64-windows/include"
            NO_DEFAULT_PATH
    )

    if(GLM_INCLUDE_DIR)
        add_library(glm_local INTERFACE)
        target_include_directories(glm_local INTERFACE ${GLM_INCLUDE_DIR})
        add_library(glm::glm ALIAS glm_local)
        message(STATUS "Found GLM: ${GLM_INCLUDE_DIR}")
    else()
        message(STATUS "GLM search paths tried:")
        message(STATUS "  ${CMAKE_SOURCE_DIR}/libs/glm")
        message(STATUS "  ${CMAKE_SOURCE_DIR}/libs")
        message(FATAL_ERROR "GLM not found! Please install GLM or place it in libs/glm/")
    endif()
endif()

# ImGui setup
if(NOT TARGET imgui::imgui)
    message(STATUS "Setting up local ImGui")
    set(IMGUI_DIR "${CMAKE_SOURCE_DIR}/libs/imgui")

    if(EXISTS ${IMGUI_DIR})
        add_library(imgui STATIC
                ${IMGUI_DIR}/imgui.cpp
                ${IMGUI_DIR}/imgui_demo.cpp
                ${IMGUI_DIR}/imgui_draw.cpp
                ${IMGUI_DIR}/imgui_tables.cpp
                ${IMGUI_DIR}/imgui_widgets.cpp
                ${IMGUI_DIR}/backends/imgui_impl_glfw.cpp
                ${IMGUI_DIR}/backends/imgui_impl_opengl3.cpp
        )

        target_include_directories(imgui PUBLIC
                ${IMGUI_DIR}
                ${IMGUI_DIR}/backends
        )

        target_link_libraries(imgui glfw OpenGL::GL)
        add_library(imgui::imgui ALIAS imgui)
        message(STATUS "Found ImGui: ${IMGUI_DIR}")
    else()
        message(WARNING "ImGui not found at ${IMGUI_DIR}")
    endif()
endif()

# Source files
set(SOURCES
        src/main.cpp
        src/Shader.cpp
        src/GUI.cpp
)

# Header files (optional, for IDE organization)
set(HEADERS
        src/Shader.hpp
        src/GUI.hpp
)

# Create executable
add_executable(${PROJECT_NAME} ${SOURCES} ${HEADERS})

# Include directories
target_include_directories(${PROJECT_NAME} PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src
)

# Link libraries
target_link_libraries(${PROJECT_NAME}
        OpenGL::GL
        glfw
        GLEW::GLEW
        glm::glm
)

# Link ImGui if available
if(TARGET imgui::imgui)
    target_link_libraries(${PROJECT_NAME} imgui::imgui)
endif()

# Platform-specific settings
if(WIN32)
    # Windows specific libraries
    target_link_libraries(${PROJECT_NAME}
            gdi32
            user32
            kernel32
    )

    # Define GLEW_STATIC if using static GLEW
    target_compile_definitions(${PROJECT_NAME} PRIVATE GLEW_STATIC)

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
if(EXISTS "${CMAKE_SOURCE_DIR}/shaders")
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
else()
    message(WARNING "Shaders directory not found: ${CMAKE_SOURCE_DIR}/shaders")
endif()

# Set working directory for Visual Studio
if(MSVC)
    set_property(TARGET ${PROJECT_NAME} PROPERTY VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_BINARY_DIR}")
endif()

# CLion-specific settings
if(CMAKE_GENERATOR STREQUAL "Ninja")
    set_target_properties(${PROJECT_NAME} PROPERTIES
            RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}
    )
endif()

# Print some information
message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")
message(STATUS "C++ standard: ${CMAKE_CXX_STANDARD}")
message(STATUS "OpenGL found: ${OPENGL_FOUND}")

# Check which targets are available
if(TARGET glfw)
    message(STATUS "Using GLFW target: glfw")
elseif(TARGET glfw3_local)
    message(STATUS "Using GLFW target: glfw3_local")
endif()

if(TARGET GLEW::GLEW)
    message(STATUS "Using GLEW target: GLEW::GLEW")
elseif(TARGET glew_local)
    message(STATUS "Using GLEW target: glew_local")
endif()

if(TARGET glm::glm)
    message(STATUS "Using GLM target: glm::glm")
elseif(TARGET glm_local)
    message(STATUS "Using GLM target: glm_local")
endif()

# Optional: Add installation rules
install(TARGETS ${PROJECT_NAME}
        RUNTIME DESTINATION bin
)

if(EXISTS "${CMAKE_SOURCE_DIR}/shaders")
    install(DIRECTORY shaders/
            DESTINATION bin/shaders
    )
endif()