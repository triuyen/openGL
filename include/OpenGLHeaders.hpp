#pragma once

// Forcer GLEW_STATIC
#ifndef GLEW_STATIC
#define GLEW_STATIC
#endif

#define GL_H
#include "OpenGLHeaders.hpp"


// Corriger les définitions manquantes
#ifndef GLchar
typedef char GLchar;
#endif
#ifndef GLuint64
typedef unsigned long long GLuint64;
#endif
#ifndef GLint64
typedef long long GLint64;
#endif
#ifndef GLsync
typedef struct __GLsync *GLsync;
#endif