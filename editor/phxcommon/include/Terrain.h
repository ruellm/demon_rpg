#pragma once

#include "common.h"
#include "Frustum.h"

#define TILE_DIST 2

typedef enum TERRAIN_GENTYPE
{
	TERRAIN_GENTYPE_FLAT,
	TERRAIN_GENTYTPE_RANDOM,
	TERRAIN_GENTYPE_HMAP
} TERRAIN_GENTYPE;

typedef struct TerrainHitInfo
{
	glm::vec3 hit_position;
	unsigned int v0;
	unsigned int v1;
	unsigned int v2;
} TerrainHitInfo;

typedef struct TerrainInitParam
{
	int width;
	int height;
	float tileDist;
	TERRAIN_GENTYPE genType;
	std::string strBaseTexture;
	int repeatRate;
	float* pHmap;

	struct TerrainRandomType {
		float noiseSize; 
		float persistence; 
		int octaves;
	} randParam;

} TerrainInitParam;

typedef struct TerrainTexture
{
	GLuint alphaTexID;
	GLuint texID;
	unsigned char* pAlphaData;
	float from;
	float to;
}TerrainTexture;


class Terrain
{

public:
	Terrain(void);
	~Terrain(void);

	void Draw();
	void Destroy();
	void RefreshVBO();
	void SmoothTerrain();
	void UpdateNormals();
		
	void SaveToFile(FILE* fp);
	void SaveTextures(const char* texPath);

	void Create(const TerrainInitParam& param ); 

	void ModifyRect(const TerrainHitInfo& info, float value, 
		int width, int height);
	
	void ModifyVertex( const TerrainHitInfo& info, 
		float value, float radius );

	void CreateRandomHeightMap(int seed, float noiseSize, 
		float persistence, int octaves);	

	bool MousePick( glm::vec3& ray_origin, 
		glm::vec3& ray_dir, TerrainHitInfo* result );
	
	int AddTexture( const char* path );
	
	void PaintTexture( const TerrainHitInfo& info, 
		int texIdx, float radius, float alpha=1.0f, bool unpaint = false );

	void SetTextureHeight( int texIdx, float min, float max);

	void RefreshTexture( int index );

	void FrustumClip( Frustum& frustum );

	inline float GetHeightAt( int idx )
	{
		return m_pVertex[ idx ].y;
	}

	inline void SetMaxHeight( float maxheight ) {
		m_maxHeight = maxheight;
	}
	
	inline void SetDimension( int width, int height ) {
		m_width = width;
		m_height = height;
	}

	inline float GetRepeatRate() const {
		return m_repeatRate;
	}

	inline int GetTextureCount() const {
		return m_textureList.size();
	}

	inline TerrainTexture* GetTextureAt(int i ){
		return 	&m_textureList[i];
	}

private:
	glm::vec3* m_pVertex;
	glm::vec3* m_pNormals;
	glm::vec2* m_pTexCoords;
	unsigned int* m_pIndices;

	std::vector<unsigned int> m_pVisibleIndices;
	int m_drawnIndexCount;

	int m_IndexCount;
	int m_width;
	int m_height;
	float m_tileDist;
	float m_maxHeight;
	int m_repeatRate;

	GLuint vao;
	GLuint vbo;
	GLuint ibo;
	GLuint vbo_normal;
	GLuint vbo_uv;

	GLuint baseTexID;
	std::vector<TerrainTexture> m_textureList;

	void UpdateTexture();
	void ComputeNormals();
	void GenerateNormals();
	void GenerateVertices(const TerrainInitParam& param);
	void GenerateTexture( const TerrainInitParam& param);
	unsigned int GetSubjectVert(const TerrainHitInfo& info);
	void GetVertFromRadius(int subject, float R, std::vector<int>& output, 
		std::vector<float>& distance);
};

