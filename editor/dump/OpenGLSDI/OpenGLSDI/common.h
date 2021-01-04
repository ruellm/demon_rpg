#ifndef __PHOENIX_COMMON_H__
#define __PHOENIX_COMMON_H__

#ifdef _WIN32
//#include "stdafx.h"
#include <Windows.h>
#pragma comment(lib,"Winmm.lib")
#pragma comment(lib, "opengl32.lib")
#endif

#include <stdio.h>
#include <stdarg.h>
#include <MMSystem.h>


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
#include <glm\gtx\intersect.hpp>

// STL includes
#include <vector>
#include <map>
#include <string>

//SOIL -- lightweight OpenGL texture library
#if 0
#include "soil\SOIL.h"

// Phoenix Common
#include "Log.h"
#include "TextureLoader.h"
#include "PathUtility.h"
#endif

#pragma comment(lib, "glew32.lib") 

#define SAFE_DELETE( obj ) { if( obj ) { delete obj; obj = NULL; } }
#define SAFE_DELETE_ARRAY( obj ) { if( obj ) { delete[] obj; obj = NULL; } }

#endif
