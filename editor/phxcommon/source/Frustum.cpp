// Frustum.h
// Created: February 21, 2014
// Author: Ruell Magpayo <ruellm@yahoo.com>
// Detail implementation of Frustum culling using GLM Matrix,
// Please check notes on extracting frustum plane

#include "Frustum.h"

Frustum::Frustum()
{
	//...
}

Frustum::~Frustum()
{
	//...
}

void Frustum::Update(const glm::mat4& vp)
{
	// NOTE:
	// using GLM (column major) matrix should be accessed as
	// !!! mat[col][row] !!

	//Left Plane is X normal + W Normal in Clip Space
	// in row major matrix it is accessed as Column_1 + Column 4
	// in column major matrix it is access as Row_1 + Row_4
	// therefore vp[1][0] is row 0 column 1.
	m_planes[FRUSTUM_LEFT].A = vp[0][0] + vp[0][3];
	m_planes[FRUSTUM_LEFT].B = vp[1][0] + vp[1][3];
	m_planes[FRUSTUM_LEFT].C = vp[2][0] + vp[2][3];
	m_planes[FRUSTUM_LEFT].D = vp[3][0] + vp[3][3];

	// Right Plane is -X Normal + W Normal 
	m_planes[FRUSTUM_RIGHT].A = -vp[0][0] + vp[0][3];
	m_planes[FRUSTUM_RIGHT].B = -vp[1][0] + vp[1][3];
	m_planes[FRUSTUM_RIGHT].C = -vp[2][0] + vp[2][3];
	m_planes[FRUSTUM_RIGHT].D = -vp[3][0] + vp[3][3];

	// BOTTOM Plane is Y Normal + W Normal, Row_2 + Row_4
	m_planes[FRUSTUM_BOTTOM].A = vp[0][1] + vp[0][3];
	m_planes[FRUSTUM_BOTTOM].B = vp[1][1] + vp[1][3];
	m_planes[FRUSTUM_BOTTOM].C = vp[2][1] + vp[2][3];
	m_planes[FRUSTUM_BOTTOM].D = vp[3][1] + vp[3][3];

	// Top Plane is -Y Normal + W Normal
	m_planes[FRUSTUM_TOP].A = -vp[0][1] + vp[0][3];
	m_planes[FRUSTUM_TOP].B = -vp[1][1] + vp[1][3];
	m_planes[FRUSTUM_TOP].C = -vp[2][1] + vp[2][3];
	m_planes[FRUSTUM_TOP].D = -vp[3][1] + vp[3][3];

	// Near Plane is Z + W Normal, Row_3, + Row_4
	m_planes[FRUSTUM_NEAR].A = vp[0][2] + vp[0][3];
	m_planes[FRUSTUM_NEAR].B = vp[1][2] + vp[1][3];
	m_planes[FRUSTUM_NEAR].C = vp[2][2] + vp[2][3];
	m_planes[FRUSTUM_NEAR].D = vp[3][2] + vp[3][3];

	// Back is -Z + W Normal
	m_planes[FRUSTUM_FAR].A = -vp[0][2] + vp[0][3];
	m_planes[FRUSTUM_FAR].B = -vp[1][2] + vp[1][3];
	m_planes[FRUSTUM_FAR].C = -vp[2][2] + vp[2][3];
	m_planes[FRUSTUM_FAR].D = -vp[3][2] + vp[3][3];

	for( int i = 0; i < 6; i++ ) {
		m_planes[i].Normalize();
	}
}

bool Frustum::CheckOOBB(const glm::vec3& min, 
		const glm::vec3& max)
{
	// Get the corners of the Box -- manually computed by Ruell 02/21/2014
	glm::vec3 corners[] = {

		// The bottom corners
		glm::vec3(min.x, min.y, min.z),
		glm::vec3(max.x, min.y, min.z),
		glm::vec3(min.x, min.y, max.z),
		glm::vec3(max.x, min.y, max.z),

		// Top Corners
		glm::vec3(min.x, max.y, min.z),
		glm::vec3(max.x, max.y, min.z),
		glm::vec3(min.x, max.y, max.z),
		glm::vec3(max.x, max.y, max.z),
	
	};

	// Test the corners to our planes
	for(int i = 0; i < 6; i++ ){
		int not_visible = 0;
		for(int c = 0; c < 8; c++ ){ 
			if( m_planes[i].DotCoord( corners[c] ) >= 0 ){
				continue;
			}
			not_visible++;
		}
		
		if( not_visible >= 8 ) {
			return false;
		}
	}
	return true;
}

bool Frustum::CheckPoint( const glm::vec3& pt )
{
	for(int i = 0; i < 6; i++ ) {
		if( m_planes[i].DotCoord( pt ) < 0 ) {
			return false;
		}
	}
	return true;
}

bool Frustum::CheckSphere( const glm::vec3& center, const float radius )
{
	for(int i = 0; i < 6; i++ ) {
		if( m_planes[i].DotCoord( center ) < -radius ) {
			return false;
		}
	}
	return true;
}
