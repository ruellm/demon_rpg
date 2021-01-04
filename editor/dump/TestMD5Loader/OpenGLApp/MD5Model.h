#pragma once

#include "common.h"
#include "Model3D.h"

///////////////////////////////////////////////////////////////////////
// MD5 File structures
typedef 
struct md5Joint
{	
	std::string name;
	int parentIdx;

	//Bone/joint matrices(?) relative to its parent bone
	glm::vec3 pos;
	glm::quat orientation;
} MD5Joint;

typedef 
struct md5Vertex 
{
	int vertIdx;
	glm::vec2 texCoords;
	int startWeight;
	int weightCount;
} MD5Vertex;

typedef
struct md5Triangle
{
	int triIdx;
	int vertIdx0;
	int vertIdx1;
	int vertIdx2;
} MD5Triangle;

typedef
struct md5Weight
{
	int weightIdx;
	int jointIdx;
	float bias;
	glm::vec3 weightPos;
} MD5Weight;

typedef
struct md5Mesh
{
	std::vector<std::string> textureList;
	int numVerts;
	int numTriangles;
	int numWeights;

	std::vector<md5Vertex> vertices;
	std::vector<md5Triangle> triangles;
	std::vector<md5Weight> weights;

} MD5Mesh;

//---------------------------------------------------------------------------------------
// MD5 Animation File Structures
//---------------------------------------------------------------------------------------

typedef
struct md5Hierarchy
{
	std::string jointName;
	int parentIdx;
	int flags;
	int startIdx;
}MD5Hierarchy;

typedef
struct md5Bounds
{
	glm::vec3 boundMin;
	glm::vec3 boundMax;
}MD5Bounds;

typedef
struct md5BaseFrame
{
	glm::vec3 position;
	glm::quat orientation;
}MD5BaseFrame;

typedef 
struct md5Frame
{
	int frameID;
	std::vector<float> frameData;
}MD5Frame;

struct md5FrameSkeleton
{
	std::vector<md5Joint> joints;
};

typedef
struct md5Animation
{
	int numFrames;
	int numJoints;
	int frameRate;
	int numAnimatedComponents;

	std::vector<md5Hierarchy> hierarchyList;
	std::vector<md5Bounds> boundsList;
	std::vector<md5BaseFrame> baseFrameList;
	std::vector<md5Frame> frameList;

	// The following section is related to rendering and NO CONNECTION to parsing
	std::vector<md5FrameSkeleton> frameSkeleton;  // can be modified to use type definition instead
	float currAnimTime;	// current animation time
	float perFrameTime;	// time duration of each frame
	float totalAnimTime;	// Total animation duration

}MD5Animation;


///////////////////////////////////////////////////////////////////////

class MD5Model : public Model3D
{
public:
	MD5Model(void);
	~MD5Model(void);

	int AddAnimation(char* szPath);

	//-----------------------------------------------------------------
	// Functions inherited from Model3D
	//-----------------------------------------------------------------
	virtual int LoadFromFile(char* szFname, char* szDirectory);
	virtual void Update(float elapsed /*, ID3D11DeviceContext* context*/);

	virtual int BakeMesh(/*ID3D11Device* device*/);
	virtual void Draw(/*ID3D11DeviceContext* context*/);

private:
	
	std::vector<md5Mesh> m_meshList;
	std::vector<md5Joint> m_jointList;
	std::vector<md5Animation> m_animationList;

	std::string GetNextToken( std::fstream* file );
	std::string RemoveQuote( std::string source );
	int GetIntToken( std::fstream* file );
	float GetFloatToken( std::fstream* file );

	int ParseJoints( std::fstream* file );
	int ParseMesh( std::fstream* file );

	//-----------------------------------------------------------------
	// Methods related to Rendering
	//-----------------------------------------------------------------
	void BuildFrameSkeleton();
	void BuildBuffers(/*ID3D11Device* device*/);

	int UpdateVertices(const md5FrameSkeleton& skeleton );
	void UpdateNormals( );

};

