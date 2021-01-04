#include "StdAfx.h"
#include "Model3D.h"


MeshSubset::MeshSubset() : 
	//vertBuffer(NULL), indexBuffer(NULL),
	vbo(0), ibo(0), texID(0),
	m_pVertices(NULL), m_pIndices(NULL),
	vbo_uv(0),
	vertCount(0), indexCount(0) {
	/*..*/
}

MeshSubset::~MeshSubset(){ 
	ShutDown(); 
}

void MeshSubset::ShutDown() {
	//SAFE_RELEASE(vertBuffer);
	//SAFE_RELEASE(indexBuffer);
	SAFE_DELETE_ARRAY(m_pVertices);
	SAFE_DELETE_ARRAY(m_pIndices);

	glDeleteBuffers(1, &vbo);
	glDeleteBuffers(1, &ibo);
	glDeleteTextures(1, & texID);
}

////////////////////////////////////////////////////////////////////////////////////
// Mesh Subset
///////////////////////////////////////////////////////////////////////////////////

Model3D::Model3D(void) 
	: m_nVertexCount(0), m_nIndexCount(0),
	m_bAnim(false), m_iCurrentAnim(0)
{
	//...
}


Model3D::~Model3D(void)
{
	for(int i =0; i < m_subsets.size(); i++) {
		m_subsets[i]->ShutDown();
		SAFE_DELETE(m_subsets[i]);
	}
}

int Model3D::LoadFromFile(char* szPath)
{
	return 0;
}

void Model3D::Update(float elapsed/*, 
	ID3D11DeviceContext* context*/)
{
	//...
}

int Model3D::BakeMesh(/*ID3D11Device* device*/)
{
	return 0;
}

void Model3D::Draw(/*ID3D11DeviceContext* context*/)
{
	//...
}

