#pragma once

#include "common.h"

struct MeshSubset
{
	//ID3D11Buffer* vertBuffer;		//--> Convert to OpenGL
	//ID3D11Buffer* indexBuffer;	//--> Convert to OpenGL
	//void* m_pVertices;

	GLuint vbo;
	GLuint ibo;
	GLuint vbo_uv;
	GLuint texID;

	glm::vec3* m_pVertices;
	unsigned short* m_pIndices;
	int vertCount;
	int indexCount;

	MeshSubset();
	~MeshSubset();
	void ShutDown();

};

class Model3D
{
public:
	Model3D(void);
	virtual ~Model3D(void);

	virtual int LoadFromFile(char* szPath);
	virtual void Update(float elapsed/*,ID3D11DeviceContext* context*/);

	virtual int BakeMesh(/*ID3D11Device* device*/);
	virtual void Draw(/*ID3D11DeviceContext* context*/);


protected:
	std::vector<MeshSubset*> m_subsets;
	int m_nVertexCount;
	int m_nIndexCount;
	bool m_bAnim;
	int m_iCurrentAnim;

	std::string m_strPathDir;
};

