// Plane.h
// Created: Feb 21, 2014
// Author: Ruell Magpayo <ruellm@yahoo.com>
// Plane definition
// a plane can be defined with equation Ax+By+Cz+D = 0	 where
// ABC is the Normal and D is the Distance to origin it can be 
// expressed as (N . P) + D = 0
//
#pragma once

#include "common.h"

typedef struct Plane
{
	float A;
	float B;
	float C;
	float D;	// Distance to Origin

	void FromPointNormal(const glm::vec3& point, 
		const glm::vec3& normal);

	void FromPoints(const glm::vec3& v0, 
		const glm::vec3& v1, const glm::vec3& v2);

	float DotCoord( const glm::vec3& pt );
	void Normalize();

	
}Plane;
