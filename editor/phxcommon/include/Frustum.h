// Frustum.h
// Created: February 21, 2014
// Author: Ruell Magpayo <ruellm@yahoo.com>
// Frustum culling implementation using CLIP Space extraction
// For algorithm, please refer to gameinstitute book chpater 4
// also refer to: http://www.lighthouse3d.com/tutorials/view-frustum-culling/clip-space-approach-extracting-the-planes/

#pragma once

#include "common.h"
#include "Plane.h"

typedef enum FRUSTUM_PLANE {
	FRUSTUM_LEFT = 0,
	FRUSTUM_RIGHT,
	FRUSTUM_TOP,
	FRUSTUM_BOTTOM,
	FRUSTUM_NEAR,
	FRUSTUM_FAR
}FRUSTUM_PLANE;


class Frustum
{
public:
	Frustum();
	~Frustum();

	void Update(const glm::mat4& vp);
	bool CheckOOBB(const glm::vec3& min, 
		const glm::vec3& max);
	
	bool CheckPoint( const glm::vec3& pt );
	bool CheckSphere( const glm::vec3& center, const float radius );
private:
	Plane m_planes[6];

};