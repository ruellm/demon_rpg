#include "Plane.h"

void Plane::Normalize()
{
	float distance = sqrtf(A * A + 
		B * B + C * C);

    A = A / distance;
    B = B / distance;
    C = C / distance;
    D = D / distance;
}

void Plane::FromPointNormal(const glm::vec3& point, 
	const glm::vec3& norm)
{
	glm::vec3 normalizeNormal = glm::normalize(norm);

	A = normalizeNormal.x;
    B = normalizeNormal.y;
    C = normalizeNormal.z;
    D = -glm::dot(point,normalizeNormal);
}

void Plane::FromPoints(const glm::vec3& v0, 
		const glm::vec3& v1, const glm::vec3& v2)
{
	glm::vec3 normal = glm::normalize(glm::cross(v1-v0, v2-v0));
	FromPointNormal(v0, normal);
}

float Plane::DotCoord( const glm::vec3& pt )
{
	// N . P + D = 0 Coplanar to the plane
	//			 = 1 in front of the plane
	//			 = <0 (negative) behind the plane
	return (glm::dot( glm::vec3(A,B,C), pt) + D);
}
