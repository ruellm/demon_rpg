#include <algorithm>    // std::max
#include "SceneObject.h"

SceneObject::SceneObject() 
	: m_bbox(0), m_angleX(0.0f), 
	m_angleY(0.0f), m_angleZ(0.0f),
	m_bIsVisible(true)
{
	m_position = glm::vec3(0.0f, 0.0f, 0.0f);
	m_up = glm::vec3(0.0f, 1.0f, 0.0f);
	m_look = glm::vec3(0.0f, 0.0f, 1.0f);
	m_right = glm::vec3(1.0f, 0.0f, 0.0f);

	m_worldMat = glm::mat4(1.0f);

	m_scale = glm::vec3(1.0f, 1.0f, 1.0f);
}

SceneObject::~SceneObject() 
{
	m_bbox->Destory();
	SAFE_DELETE(m_bbox);
}
	 
void SceneObject::Update(float elapsed)
{
#if 0
	m_worldMat[0][0] = m_right.x;
	m_worldMat[1][0] = m_up.x;
	m_worldMat[2][0] = m_look.x;

	m_worldMat[0][1] = m_right.y;
	m_worldMat[1][1] = m_up.y;
	m_worldMat[2][1] = m_look.y;
	
	m_worldMat[0][2] =m_right.z;
	m_worldMat[1][2] =m_up.z;
	m_worldMat[2][2] =m_look.z;

	m_worldMat[3][0] = m_position.x;
	m_worldMat[3][1] = m_position.y;
	m_worldMat[3][2] = m_position.z;
#else

	glm::mat4 matScale = glm::mat4(1.0f);
	matScale = glm::scale( m_scale );

	glm::mat4 matPos = glm::translate( m_position );
	
	glm::mat4 matRotX = glm::mat4(1.0f);
	matRotX = glm::rotate(matRotX, m_angleX, glm::vec3(1.0f, 0.0f, 0.0f));

	glm::mat4 matRotY = glm::mat4(1.0f);
	matRotY = glm::rotate(matRotY, m_angleY, glm::vec3(0.0f, 1.0f, 0.0f));

	glm::mat4 matRotZ = glm::mat4(1.0f);
	matRotZ = glm::rotate(matRotZ, m_angleZ, glm::vec3(0.0f, 0.0f, -1.0f));

	m_worldMat = glm::mat4(1.0f);
	m_worldMat= matPos * (matRotX * matRotY * matRotZ) * matScale;
#endif

}

void SceneObject::Draw()
{
	//...
}


void SceneObject::DrawBBox()
{
	if( m_bbox )
		m_bbox->Draw();
}

void SceneObject::Move( glm::vec3& dir, float value )
{
	dir = glm::normalize( dir );
	m_position += (dir * value);
}

void SceneObject::RotateX( float value )
{
	value = glm::radians( value );
	m_angleX += value;

}

void SceneObject::RotateY( float value )
{
	value = glm::radians( value );
	m_angleY += value;
}

void SceneObject::RotateZ( float value )
{
	value = glm::radians( value );
	m_angleZ += value;
}

void SceneObject::ScaleX( float value )
{
	m_scale.x += value;
}

void SceneObject::ScaleY( float value )
{
	m_scale.y += value;

}

void SceneObject::ScaleZ( float value )
{
	m_scale.z += value;
}


/*--------------------------------------------------------------*/
// Watch Gamedev.net line geometry testing for explanation
// We must decide to put this somehwere
// this is using SLAB testing
// ZBU_TODO: migrate/transfer to common Math handler class

static bool RaySlabIntersect(float slabmin, float slabmax, float raystart, float rayend, float& tbenter, float& tbexit)
{
	float raydir = rayend - raystart;

	// ray parallel to the slab
	if (fabs(raydir) < 1.0E-9f)
	{
		// ray parallel to the slab, but ray not inside the slab planes
		if(raystart < slabmin || raystart > slabmax)
		{
			return false;
		}
		// ray parallel to the slab, but ray inside the slab planes
		else
		{
			return true;
		}
	}

	// slab's enter and exit parameters
	float tsenter = (slabmin - raystart) / raydir;
	float tsexit = (slabmax - raystart) / raydir;

	// order the enter / exit values.
	if(tsenter > tsexit)
	{
		//swapf(tsenter, tsexit);
		float temp = tsenter;
		tsenter = tsexit;
		tsexit = temp;
	}

	// make sure the slab interval and the current box intersection interval overlap
	if (tbenter > tsexit || tsenter > tbexit)
	{
		// nope. Ray missed the box.
		return false;
	}
	// yep, the slab and current intersection interval overlap
	else
	{
		// update the intersection interval
		tbenter = std::max(tbenter, tsenter);
		tbexit = std::min(tbexit, tsexit);
		return true;
	}
}/*--------------------------------------------------------------*/

bool SceneObject::MousePick( const glm::vec3& rayPos, 
		const glm::vec3& rayDir, glm::vec3* hitPos )
{

	glm::mat4 worldMat = m_worldMat * m_bbox->GetTransformMatrix();
	glm::vec4 min = m_worldMat * glm::vec4(m_bbox->min, 1 );
	glm::vec4 max = m_worldMat * glm::vec4(m_bbox->max, 1 );
	glm::vec3 rayEnd = 	rayPos + (rayDir * 1000.0f);		   // Multiplied to Far clip plane

	// initialise to the segment's boundaries. 
	float tenter = 0.0f, texit = 1.0f; 

	// test X slab
	if (!RaySlabIntersect(min.x, max.x, rayPos.x, rayEnd.x, tenter, texit)) 
	{
		return false;
	}

	// test Y slab

	if (!RaySlabIntersect(min.y, max.y, rayPos.y, rayEnd.y, tenter, texit)) 
	{
		return false;
	}

	// test Z slab
	if (!RaySlabIntersect(min.z, max.z, rayPos.z, rayEnd.z, tenter, texit)) 
	{
		return false;
	}

	// all intersections in the green. Return the first time of intersection, tenter.
	//tinter = tenter;
	return  true;

#if 0
	// Intersection method from Real-Time Rendering and Essential Mathematics for Games
	// courtesy of 
	// http://www.opengl-tutorial.org/miscellaneous/clicking-on-objects/picking-with-custom-ray-obb-function/#Ray_OBB_intersection_algorithm
	float tMin = 0.0f;
	float tMax = 100000.0f;

	glm::mat4 worldMat = m_worldMat * m_bbox->GetTransformMatrix();
	glm::vec3 OBBposition_worldspace(worldMat[3].x, worldMat[3].y, worldMat[3].z);

	glm::vec3 delta = OBBposition_worldspace - rayPos;

	// Test intersection with the 2 planes perpendicular to the OBB's X axis
	{
		glm::vec3 xaxis(worldMat[0].x, worldMat[0].y, worldMat[0].z);
		float e = glm::dot(xaxis, delta);
		float f = glm::dot(rayDir, xaxis);

		if ( fabs(f) > 0.001f ){ // Standard case

			float t1 = (e+m_bbox->min.x)/f; // Intersection with the "left" plane
			float t2 = (e+m_bbox->max.x)/f; // Intersection with the "right" plane
			// t1 and t2 now contain distances betwen ray origin and ray-plane intersections

			// We want t1 to represent the nearest intersection, 
			// so if it's not the case, invert t1 and t2
			if (t1>t2){
				float w=t1;t1=t2;t2=w; // swap t1 and t2
			}

			// tMax is the nearest "far" intersection (amongst the X,Y and Z planes pairs)
			if ( t2 < tMax )
				tMax = t2;
			// tMin is the farthest "near" intersection (amongst the X,Y and Z planes pairs)
			if ( t1 > tMin )
				tMin = t1;

			// And here's the trick :
			// If "far" is closer than "near", then there is NO intersection.
			// See the images in the tutorials for the visual explanation.
			if (tMax < tMin )
				return false;

		}else{ // Rare case : the ray is almost parallel to the planes, so they don't have any "intersection"
			if(-e+m_bbox->min.x > 0.0f || -e+m_bbox->max.x < 0.0f)
				return false;
		}
	}


	// Test intersection with the 2 planes perpendicular to the OBB's Y axis
	// Exactly the same thing than above.
	{
		glm::vec3 yaxis(worldMat[1].x, worldMat[1].y, worldMat[1].z);
		float e = glm::dot(yaxis, delta);
		float f = glm::dot(rayDir, yaxis);

		if ( fabs(f) > 0.001f ){

			float t1 = (e+m_bbox->min.y)/f;
			float t2 = (e+m_bbox->max.y)/f;

			if (t1>t2){float w=t1;t1=t2;t2=w;}

			if ( t2 < tMax )
				tMax = t2;
			if ( t1 > tMin )
				tMin = t1;
			if (tMin > tMax)
				return false;

		}else{
			if(-e+m_bbox->min.y > 0.0f || -e+m_bbox->max.y < 0.0f)
				return false;
		}
	}


	// Test intersection with the 2 planes perpendicular to the OBB's Z axis
	// Exactly the same thing than above.
	{
		glm::vec3 zaxis(worldMat[2].x, worldMat[2].y, worldMat[2].z);
		float e = glm::dot(zaxis, delta);
		float f = glm::dot(rayDir, zaxis);

		if ( fabs(f) > 0.001f ){

			float t1 = (e+m_bbox->min.z)/f;
			float t2 = (e+m_bbox->max.z)/f;

			if (t1>t2){float w=t1;t1=t2;t2=w;}

			if ( t2 < tMax )
				tMax = t2;
			if ( t1 > tMin )
				tMin = t1;
			if (tMin > tMax)
				return false;

		}else{
			if(-e+m_bbox->min.z > 0.0f || -e+m_bbox->max.z < 0.0f)
				return false;
		}
	}

	float intersection_distance = tMin;
#endif
	return true;
}

//-------------------------------------------------------------------------------------------

Object3D::Object3D() 
	: SceneObject(), m_pModel(0) 
{
	
}

Object3D::~Object3D()
{
	
}


void Object3D::Update(float elapsed)
{
	SceneObject::Update(elapsed);
	if( m_pModel == NULL ) return;
	m_pModel->Update(elapsed);
}

void Object3D::Draw()
{
	if( m_pModel == NULL || !IsVisible() ) return;
	m_pModel->Draw( );
}

void Object3D::GenBoundingVolume()
{
	if( m_pModel == NULL ) return;

	// Delete old Bounding Volume
	if( m_bbox ) {
		m_bbox->Destory();
		SAFE_DELETE(m_bbox);
	}

	m_bbox = new BoundingBox();
	m_bbox->Generate((OBJModel*)m_pModel);
}
