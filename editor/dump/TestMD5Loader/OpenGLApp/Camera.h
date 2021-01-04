#pragma once

#include "common.h"

class Camera
{
public:
	Camera(void);
	~Camera(void);

	void Update();

	void LookAt(const glm::vec3& pos, 
		const glm::vec3& up, 
		const glm::vec3& look);		//NOTE: The lookup in camera is NOT normalize vector

	void Walk( float amount );
	void Strafe (float amount);
	void Pitch(float amount);
	void Yaw(float amount);

	inline glm::mat4 GetViewMatrix() const {
		return m_viewmat;
	}

private:
	glm::vec3 m_position;
	glm::vec3 m_up;
	glm::vec3 m_look;
	glm::vec3 m_right;
	glm::mat4 m_viewmat;

	void UpdateViewMtx();

};

