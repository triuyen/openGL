#pragma once

// IMPORTANT: GLEW doit être inclus AVANT GLFW et tout autre header OpenGL
#ifndef GLEW_STATIC
#define GLEW_STATIC
#endif

// S'assurer que gl.h n'est pas inclus avant glew.h
#ifndef __gl_h_
#include <GL/glew.h>
#endif

#include <GLFW/glfw3.h>

// Headers GLM communs
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

// Headers standard communs
#include <iostream>
#include <string>
#include <vector>
#include <memory>
#include <fstream>
#include <sstream>
#include <cmath>