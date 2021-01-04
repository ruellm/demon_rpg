#pragma once

#include "common.h"

typedef struct DirectionalLight
{
	glm::vec3 color;
	float intensity;
	glm::vec3 direction;

} DirectionalLight;

typedef struct AmbientLight
{
	glm::vec3 color;
	float intensity;

} AmbientLight;