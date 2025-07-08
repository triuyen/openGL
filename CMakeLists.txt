cmake_minimum_required(VERSION 3.10)
project(TP_dragon)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find packages
find_package(OpenGL REQUIRED)
find_package(GLEW REQUIRED)
find_package(glfw3 REQUIRED)
# Remove this line: find_package(glm REQUIRED)

# Copy texture to build directory (add this line)
configure_file(${CMAKE_SOURCE_DIR}/dragon_texture.png ${CMAKE_BINARY_DIR}/dragon_texture.png COPYONLY)

# Add executable
add_executable(TP_dragon main.cpp)

# Include directories
target_include_directories(TP_dragon PRIVATE
        ${CMAKE_SOURCE_DIR}         # For dataDragon.h and stb_image.h
        ${CMAKE_SOURCE_DIR}/libs    # For GLM headers
        ${CMAKE_SOURCE_DIR}/src
        ${OPENGL_INCLUDE_DIR}
        ${GLEW_INCLUDE_DIRS}
)

# Link libraries
target_link_libraries(TP_dragon PRIVATE
        ${OPENGL_LIBRARIES}
        GLEW::GLEW
        glfw
        # Remove this line: ${GLM_LIBRARIES}
)