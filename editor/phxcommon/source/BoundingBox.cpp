#include "BoundingBox.h"

BoundingBox::BoundingBox() 
	: vao (0), vbo(0), ibo(0)
{
	transformMatx = glm::mat4(1);
}

void BoundingBox::Destory()
{
	glDeleteBuffers(1, &vbo);
	glDeleteBuffers(1, &ibo);

	glDeleteVertexArrays(1, &vao);
}

void BoundingBox::Generate( OBJModel* obj )
{
	min = obj->GetMesh(0)->vertexBuffer[0];
	max = min;
	 
	for(int x=0; x < obj->GetMeshCount(); x++) {
			OBJMesh* mesh = obj->GetMesh(x);
	 
			for(int v=0; v < mesh->vertexBuffer.size(); v++ ){
				if( mesh->vertexBuffer[v].x < min.x ) {
					min.x =  mesh->vertexBuffer[v].x;
				} 
				if( mesh->vertexBuffer[v].y < min.y ) {
					min.y =  mesh->vertexBuffer[v].y;
				}
				if( mesh->vertexBuffer[v].z < min.z ) {
					min.z =  mesh->vertexBuffer[v].z;
				}

				if( mesh->vertexBuffer[v].x > max.x ) {
					max.x =  mesh->vertexBuffer[v].x;
				} 
				if( mesh->vertexBuffer[v].y > max.y ) {
					max.y =  mesh->vertexBuffer[v].y;
				}
				if( mesh->vertexBuffer[v].z > max.z ) {
					max.z =  mesh->vertexBuffer[v].z;
				}
			}
	 }

	GenerateBuffers();
}

void  BoundingBox::Generate( glm::vec3* pVertex, int count)
{
	min = pVertex[0];
	max = min ;
	 
	for(int x=0; x < count; x++) {
		if( pVertex[x].x < min.x ) {
			min.x =  pVertex[x].x;
		} 
		if( pVertex[x].y < min.y ) {
			min.y =  pVertex[x].y;
		}
		if( pVertex[x].z < min.z ) {
			min.z =  pVertex[x].z;
		}

		if( pVertex[x].x > max.x ) {
			max.x =  pVertex[x].x;
		} 
		if( pVertex[x].y > max.y ) {
			max.y =  pVertex[x].y;
		}
		if( pVertex[x].z > max.z ) {
			max.z =  pVertex[x].z;
		}
	 }

	GenerateBuffers();
}

void BoundingBox::Draw()
{
	glBindVertexArray(vao);
	glEnableVertexAttribArray(0);
	
#if 1
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(
    0,  // attribute
    4,                  // number of elements per vertex, here (x,y,z,w)
    GL_FLOAT,           // the type of each element
    GL_FALSE,           // take our values as-is
    0,                  // no extra data between each position
    0                   // offset of first element
  );
 
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);

   glDrawElements(GL_LINE_LOOP, 4, GL_UNSIGNED_SHORT, 0);
  glDrawElements(GL_LINE_LOOP, 4, GL_UNSIGNED_SHORT, (GLvoid*)(4*sizeof(GLushort)));
  glDrawElements(GL_LINES, 8, GL_UNSIGNED_SHORT, (GLvoid*)(8*sizeof(GLushort)));
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
#else

	glDrawArrays(GL_LINE_LOOP,0,4);
	glDrawArrays(GL_LINE_LOOP, 4*sizeof(GLushort), 4);
	glDrawArrays(GL_LINES, 8*sizeof(GLushort),4);
#endif

	glBindVertexArray(0);
	glDisableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void BoundingBox::GenerateBuffers()
{
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);
	
	// source: http://en.wikibooks.org/wiki/OpenGL_Programming/Bounding_box
	// Cube 1x1x1, centered on origin
	GLfloat vertices[] = {
		-0.5, -0.5, -0.5, 1.0,
		 0.5, -0.5, -0.5, 1.0,
		 0.5,  0.5, -0.5, 1.0,
		-0.5,  0.5, -0.5, 1.0,
		-0.5, -0.5,  0.5, 1.0,
		 0.5, -0.5,  0.5, 1.0,
		 0.5,  0.5,  0.5, 1.0,
		-0.5,  0.5,  0.5, 1.0,
	  };


	  glGenBuffers(1, &vbo);
	  glBindBuffer(GL_ARRAY_BUFFER, vbo);
	  glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
 
	  GLushort elements[] = {
		0, 1, 2, 3,
		4, 5, 6, 7,
		0, 4, 1, 5, 2, 6, 3, 7
	  };

	  glGenBuffers(1, &ibo);
	  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
	  glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(elements), elements, GL_STATIC_DRAW);
 
	  glm::vec3 size = glm::vec3(max.x-min.x, max.y-min.y, max.z-min.z);
	  glm::vec3 center = glm::vec3((min.x+max.x)/2, (min.y+max.y)/2, (min.z+max.z)/2);
	  transformMatx =  glm::translate(glm::mat4(1), center) * glm::scale(glm::mat4(1), size);
#if 0
	  glBindBuffer(GL_ARRAY_BUFFER, vbo);
	  glEnableVertexAttribArray(0);
	  glVertexAttribPointer(
		0,  // attribute
		4,                  // number of elements per vertex, here (x,y,z,w)
		GL_FLOAT,           // the type of each element
		GL_FALSE,           // take our values as-is
		0,                  // no extra data between each position
		0                   // offset of first element
	  );
#endif

	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER,0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}


glm::mat4& BoundingBox::GetTransformMatrix() {
	return transformMatx;
}
