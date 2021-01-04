#ifndef __PHOENIX_COMMON_H__
#define __PHOENIX_COMMON_H__

#ifdef _WIN32
#include "stdafx.h"
#endif

#include <MMSystem.h>
#pragma comment(lib,"Winmm.lib")

// WGL + GLEW header files
#include <glew/include/GL/glew.h>
#include <glew/include/GL/wglew.h>

// GLM!!
// vec3, vec4, ivec4, mat4 
#include <glm/glm.hpp> 
// translate, rotate, scale, perspective 
#include <glm/gtc/matrix_transform.hpp>
// value_ptr
#include <glm/gtc/type_ptr.hpp>
// Matrix elements access
#include <glm/gtc/matrix_access.hpp>
// rotate vector
#include <glm/gtx/rotate_vector.hpp>
#include <glm/gtx/transform.hpp>

#include <glm/gtc/quaternion.hpp> 
#include <glm/gtx/quaternion.hpp>

#pragma comment(lib, "opengl32.lib")

// STL includes
#include <vector>
#include <map>
#include <string>


//SOIL -- lightweight OpenGL texture library
#include "soil\SOIL.h"

#ifdef _WIN64
#pragma comment(lib, "../../glew-1.10.0-win32/glew/lib/Release/x64/glew32.lib")
#else
#pragma comment(lib, "glew32.lib") 
#endif

#define SAFE_DELETE( obj ) { if( obj ) { delete obj; obj = NULL; } }
#define SAFE_DELETE_ARRAY( obj ) { if( obj ) { delete[] obj; obj = NULL; } }

#endif
