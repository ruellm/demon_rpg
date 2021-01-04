#pragma once

#include "common.h"
#include "OBJModel.h"

class BoundingBox{
	
public:
	BoundingBox();
	void Destory();

	void Generate( glm::vec3* pVertex, int count);
	void Generate( OBJModel* obj );
	void Draw();
	void GenerateBuffers();
    void Scale( const glm::vec3& scale );

	glm::mat4& GetTransformMatrix();
	glm::mat4& GetScaleMatrix();

	glm::vec3 min;
	glm::vec3 max;

private:
	
	glm::mat4 transformMatx;
	glm::mat4 scaleMatx;
	glm::mat4 finalTransformMatx;

	GLuint vao;
	GLuint vbo;
	GLuint ibo;

};

