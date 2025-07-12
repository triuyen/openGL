cmake_minimum_required(VERSION 3.20)
project(OpenGL_TD)

set(CMAKE_CXX_STANDARD 20)

# Define directories
set(THIRD_PARTY_DIR ${CMAKE_SOURCE_DIR}/third_party)

# ============================================================================
# Include directories
# ============================================================================
include_directories(
        ${CMAKE_SOURCE_DIR}/include
        ${THIRD_PARTY_DIR}  # This allows #include <glm/glm.hpp> to work
        ${THIRD_PARTY_DIR}/glfw/include
        ${THIRD_PARTY_DIR}/glew/include
        ${THIRD_PARTY_DIR}/imgui
        ${THIRD_PARTY_DIR}/imgui/backends
        ${THIRD_PARTY_DIR}/stb
)

# ============================================================================
# Compiler definitions
# ============================================================================
# Force GLEW_STATIC definition pour tous les fichiers
add_compile_definitions(GLEW_STATIC)

# ============================================================================
# Platform-specific library setup
# ============================================================================
if(WIN32)
    # GLFW
    if(MINGW)
        set(GLFW_LIBRARY ${THIRD_PARTY_DIR}/glfw/lib-mingw-w64/libglfw3.a)
    else()  # MSVC
        # Simplified: just use VS2019 libs as they're generally compatible
        set(GLFW_LIBRARY ${THIRD_PARTY_DIR}/glfw/lib-vc2019/glfw3.lib)
    endif()

    # GLEW
    if(CMAKE_SIZEOF_VOID_P EQUAL 8)
        set(GLEW_LIBRARY ${THIRD_PARTY_DIR}/glew/lib/Release/x64/glew32s.lib)
    else()
        set(GLEW_LIBRARY ${THIRD_PARTY_DIR}/glew/lib/Release/Win32/glew32s.lib)
    endif()

    # OpenGL
    set(OPENGL_LIBRARY opengl32)
else()
    # Linux/Mac - use system packages
    find_package(glfw3 REQUIRED)
    find_package(GLEW REQUIRED)
    find_package(OpenGL REQUIRED)

    set(GLFW_LIBRARY glfw)
    set(GLEW_LIBRARY GLEW::GLEW)
    set(OPENGL_LIBRARY ${OPENGL_LIBRARIES})
endif()

# ============================================================================
# ImGui sources
# ============================================================================
set(IMGUI_SOURCES
        ${THIRD_PARTY_DIR}/imgui/imgui.cpp
        ${THIRD_PARTY_DIR}/imgui/imgui_demo.cpp
        ${THIRD_PARTY_DIR}/imgui/imgui_draw.cpp
        ${THIRD_PARTY_DIR}/imgui/imgui_tables.cpp
        ${THIRD_PARTY_DIR}/imgui/imgui_widgets.cpp
        ${THIRD_PARTY_DIR}/imgui/backends/imgui_impl_glfw.cpp
        ${THIRD_PARTY_DIR}/imgui/backends/imgui_impl_opengl3.cpp
)

# ============================================================================
# Project sources
# ============================================================================
set(PROJECT_SOURCES
        src/main.cpp
        src/Camera.cpp
        src/GUI.cpp
        src/Light.cpp
        src/Shader.cpp
        src/Material.cpp
        src/Geometry.cpp
        src/Skybox.cpp
        src/stb_image_impl.cpp
)

# ============================================================================
# Create executable and link libraries
# ============================================================================
add_executable(${PROJECT_NAME} ${PROJECT_SOURCES} ${IMGUI_SOURCES})

target_link_libraries(${PROJECT_NAME}
        ${GLFW_LIBRARY}
        ${GLEW_LIBRARY}
        ${OPENGL_LIBRARY}
)

# Platform-specific additional libraries
if(APPLE)
    target_link_libraries(${PROJECT_NAME}
            "-framework Cocoa"
            "-framework IOKit"
            "-framework CoreVideo"
    )
elseif(UNIX AND NOT APPLE)  # Linux
    find_package(X11 REQUIRED)
    target_link_libraries(${PROJECT_NAME}
            ${X11_LIBRARIES}
            dl
            pthread
    )
endif()

# ============================================================================
# Copy resources
# ============================================================================
# Copy shaders to build directory (for all configurations)
add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_SOURCE_DIR}/assets $<TARGET_FILE_DIR:${PROJECT_NAME}>/assets
)