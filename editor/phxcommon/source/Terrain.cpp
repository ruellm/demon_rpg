// Terrain.cpp : Terrain Mesh implementation in OpenGL
// 
// Author: Ruell Magpayo <ruellm@yahoo.com>
// Date Created: Nov 27, 2013
// Some codes were adopted from DirectX RTS Book

//#include "StdAfx.h"
#include "Terrain.h"

Terrain::Terrain(void)	
	: m_pVertex(NULL), m_width(0), 
	m_height(0), m_tileDist(TILE_DIST), vao(0),
	vbo(0), ibo(0), m_pIndices(0), m_IndexCount(0),
	m_pNormals(0), vbo_normal(0), m_maxHeight(255.0f),
	m_pTexCoords(0), vbo_uv(0), baseTexID(0), m_repeatRate(1)
{
	//...
}


Terrain::~Terrain(void)
{
	Destroy();
}

void Terrain::Destroy( )
{
	SAFE_DELETE_ARRAY(m_pVertex);
	SAFE_DELETE_ARRAY(m_pIndices);
	SAFE_DELETE_ARRAY(m_pNormals);
	SAFE_DELETE_ARRAY(m_pTexCoords);

	glDeleteBuffers(1, &vbo);
	glDeleteBuffers(1, &vbo_normal);
	glDeleteBuffers(1, &vbo_uv);
	glDeleteVertexArrays(1, &vao);

	glDeleteTextures(1, &baseTexID);

	for (int i=0; i < m_textureList.size(); i++) {
		if( m_textureList[i].texID ) {
			glDeleteTextures(1, &m_textureList[i].texID);		
		}

		if( m_textureList[i].alphaTexID ) {
			glDeleteTextures(1, &m_textureList[i].alphaTexID);		
		}

		if( m_textureList[i].pAlphaData ) {
			SAFE_DELETE_ARRAY( m_textureList[i].pAlphaData );
		}
	}
}

void Terrain::Create(const TerrainInitParam& param )
{
	SetDimension( param.width, param.height );

	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	GenerateVertices( param );
	GenerateNormals( );
	GenerateTexture( param );

	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void Terrain::GenerateVertices( const TerrainInitParam& param )
{
		//float actualWidth = m_width * m_tileDist;
		//float actualHeight = m_height * m_tileDist;

		m_tileDist = param.tileDist;

		float startX = -( (m_width*m_tileDist) / 2.0f);
		float startZ = -( (m_height*m_tileDist) / 2.0f);
		int vertSize = m_width * m_height;
		//
		m_pVertex = new glm::vec3[ vertSize ];

		int index = 0;
		for(int z = 0; z < m_height; z++ ) {
				for(int x = 0; x < m_width; x++ ) {
						m_pVertex[index].x = startX + (x * m_tileDist);
						m_pVertex[index].y = 0;
						m_pVertex[index].z = startZ + (z * m_tileDist);
						index++;
				}
		}

		if( param.genType == TERRAIN_GENTYTPE_RANDOM ) {
			CreateRandomHeightMap(rand()%2000, param.randParam.noiseSize, 
				param.randParam.persistence, param.randParam.octaves);
				SmoothTerrain();
		}  else if (param.genType == TERRAIN_GENTYPE_HMAP ) {
			//...
			for(int c = 0; c < (m_width * m_height); c++ ) {
				m_pVertex[c].y = param.pHmap[c];
			}
		}

		glGenBuffers(1, &vbo);
		glBindBuffer(GL_ARRAY_BUFFER, vbo);
		glBufferData(GL_ARRAY_BUFFER, vertSize * sizeof(glm::vec3), 
				m_pVertex, GL_STATIC_DRAW);
	
		glVertexAttribPointer( 0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0 ); 

		int index_size = (m_width-1) * (m_height-1) * 6;
		m_IndexCount = index_size;
		m_pIndices = new unsigned int[index_size];
		
		unsigned int* pIdx = m_pIndices;
		for(int z = 0; z < m_height - 1; z++) {
				for( int x = 0; x < m_width - 1; x++) {
						//Triangle 1
						*pIdx++ = (z*m_width) + x;
						*pIdx++ = ((z+1)*m_width) + x;
						*pIdx++ = ( (z+1) * m_width) + (x+1);

						//Triangle 2
						*pIdx++ = (z*m_width) + x;
						*pIdx++ = ( (z+1) * m_width) + (x+1);
						*pIdx++ = (z*m_width) + x+1;
				}
		}

		glGenBuffers(1, &ibo);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, 
				m_IndexCount * sizeof(unsigned int), 
				m_pIndices, GL_STATIC_DRAW);

		m_drawnIndexCount = m_IndexCount;

}
void Terrain::GenerateNormals()
{
		m_pNormals = new glm::vec3[ m_width * m_height ];
		ComputeNormals();

		glGenBuffers(1, &vbo_normal);
		glBindBuffer(GL_ARRAY_BUFFER, vbo_normal);
		glBufferData(GL_ARRAY_BUFFER, 
				m_width * m_height * sizeof(glm::vec3), 
				m_pNormals, GL_STATIC_DRAW);

		glVertexAttribPointer( 1, 3, GL_FLOAT, GL_FALSE, 0, (void*)0 ); 
}
void Terrain::GenerateTexture( const TerrainInitParam& param )
{
	if( param.strBaseTexture.empty() ) return;

	// store repeat rate
	m_repeatRate = param.repeatRate;

	int vertCount = m_width * m_height;
	int index = 0;

	baseTexID = TextureLoader::GetTexture( param.strBaseTexture.c_str() );
	if( baseTexID == 0 ) return;

	m_pTexCoords = new glm::vec2[ vertCount ];
	for(int z = 0; z < m_height; z++ ) {
		for(int x = 0; x < m_width; x++ ) {

			float u = x / ((float)m_width-  1.0f);
			float v = z / ((float)m_height - 1.0f);

			m_pTexCoords[index].x = u * param.repeatRate;
			m_pTexCoords[index].y = (1-v) * param.repeatRate;

			index++;
		}
	}

	glGenBuffers(1, &vbo_uv);
	glBindBuffer(GL_ARRAY_BUFFER, vbo_uv);
	glBufferData(GL_ARRAY_BUFFER, vertCount * sizeof(glm::vec2), 
			m_pTexCoords, GL_STATIC_DRAW);

	glVertexAttribPointer( 2, 2, GL_FLOAT, GL_FALSE, 0, (void*)0 ); 
	
}

void Terrain::ComputeNormals()
{
	unsigned int* pIdx = m_pIndices;
	int normal_size =  m_width * m_height;
	
	for( int i=0; i < m_IndexCount; i+=3 ) {
		unsigned int v0idx = *pIdx++;
		unsigned int v1idx = *pIdx++;
		unsigned int v2idx = *pIdx++;

		glm::vec3 v0 = m_pVertex[v0idx];
		glm::vec3 v1 = m_pVertex[v1idx];
		glm::vec3 v2 = m_pVertex[v2idx];

		glm::vec3 normal = glm::normalize( glm::cross( v1 - v0, v2 - v0 ) );
		
		m_pNormals[ v0idx ] += normal;
        m_pNormals[ v1idx ] += normal;
        m_pNormals[ v2idx ] += normal;

	}

	const glm::vec3 UP( 0.0f, 1.0f, 0.0f );
    for ( unsigned int i = 0; i < normal_size; ++i )
    {
        m_pNormals[i] = glm::normalize( m_pNormals[i] ); 
    }
}

void Terrain::UpdateNormals()
{
	ComputeNormals();
	glBindBuffer(GL_ARRAY_BUFFER, vbo_normal);
	GLvoid* position = glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY);
		memcpy(position, m_pNormals, m_width * m_height * sizeof(glm::vec3));
	glUnmapBuffer(GL_ARRAY_BUFFER);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void Terrain::Draw( )
{
	glBindVertexArray(vao);
	glEnableVertexAttribArray(0);  // Position
	glEnableVertexAttribArray(1);  // normals
	
	if( m_pTexCoords ) {
		glEnableVertexAttribArray(2);  	// texture

		glActiveTexture(GL_TEXTURE0);
		glBindTexture( GL_TEXTURE_2D, baseTexID );

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

	}

	if( m_textureList.size() > 0 ) {
		int textureUnit = 0;
		for(int i = 0; i < m_textureList.size(); i++ ){
			glActiveTexture(GL_TEXTURE1 + textureUnit++);	
			glBindTexture( GL_TEXTURE_2D, m_textureList[i].texID );

			glActiveTexture(GL_TEXTURE1 + textureUnit++);	
			glBindTexture( GL_TEXTURE_2D, m_textureList[i].alphaTexID );
		}
	}

	//glDrawElements(GL_TRIANGLES, m_IndexCount, 
	//	GL_UNSIGNED_INT, 0);
	glDrawElements(GL_TRIANGLES, m_drawnIndexCount, 
		GL_UNSIGNED_INT, 0);
	
	glDisableVertexAttribArray(0);
	glDisableVertexAttribArray(1);
	glDisableVertexAttribArray(2);

	glBindVertexArray(0);
}

bool 
Terrain::MousePick( glm::vec3& ray_origin, 
					glm::vec3& ray_dir, TerrainHitInfo* result )
{

	// loop thru all indices
	unsigned int* pIdx = m_pIndices;
	int index = 0;
	bool hit = false;
	glm::vec3 hitpos = glm::vec3(0.0, 0.0, 0.0);
	std::vector<TerrainHitInfo> hitList;

	for( int i=0; i < m_IndexCount; i+=3 ) {
		unsigned int v0idx = *pIdx++;
		unsigned int v1idx = *pIdx++;
		unsigned int v2idx = *pIdx++;

		glm::vec3 v0 = m_pVertex[v0idx];
		glm::vec3 v1 = m_pVertex[v1idx];
		glm::vec3 v2 = m_pVertex[v2idx];

		hit = glm::intersectRayTriangle(ray_origin, ray_dir, 
				v0, v1, v2, hitpos);

		if( hit ) {
			
			TerrainHitInfo hitlist;
			hitlist.v0 = v0idx;
			hitlist.v1 = v1idx;
			hitlist.v2 = v2idx;
			hitlist.hit_position = hitpos;

			hitList.push_back(hitlist);
		}

	}

	if( hitList.size() > 0 ) {
		float minDistance = 1000000.0f;
		TerrainHitInfo min = hitList[0];

		for(int i=0; i < hitList.size(); i++ ) {
			glm::vec3 actualHit = ray_origin + 
				(ray_dir * hitList[i].hit_position.z);  

			float distance = glm::length(actualHit - ray_origin);
			if( distance < minDistance ) {
				minDistance = distance;
				min = hitList[i];
			}
		}

		result->v0 = min.v0;
		result->v1 = min.v1;
		result->v2 = min.v2;
		result->hit_position = ray_origin + 
			(ray_dir * min.hit_position.z);
		
		return true;
	}

	return false;
}

unsigned int 
Terrain::GetSubjectVert(const TerrainHitInfo& info)
{
	glm::vec3 v0 = m_pVertex[info.v0];
	glm::vec3 v1 = m_pVertex[info.v1];
	glm::vec3 v2 = m_pVertex[info.v2];

	glm::vec3 hitpos_tmp = glm::vec3(info.hit_position.x, 0, 
		info.hit_position.z);

	//glm::vec3 v0temp = glm::vec3(v0.x, 0, v0.z);
	//glm::vec3 v1temp = glm::vec3(v1.x, 0, v1.z);
	//glm::vec3 v2temp = glm::vec3(v2.x, 0, v2.z);

	float v0len = glm::length(hitpos_tmp - v0);
	float v1len = glm::length(hitpos_tmp - v1);
	float v2len = glm::length(hitpos_tmp - v2);

	unsigned int subject = info.v0;
	float subj_val = v0len;
	if( subj_val > v1len ) {
		subject = info.v1;
		subj_val = v1len;
	}
	if( subj_val > v2len ) {
		subject = info.v2;
	}

	return subject;
}

void Terrain::ModifyRect(const TerrainHitInfo& info, float value, int width, int height)
{
   unsigned int subject = GetSubjectVert( info );
   for(int y = 0; y < height; y++) {
	   	int subjBase = subject + (m_width * y);
		int lowbound = (subjBase/m_width) * m_width;
		int highbound = lowbound + (m_width-1);
	   for( int x = 0; x < width; x++ ) {
			int index = (subject+x) + (m_width * y);

			if( index > highbound) continue;
			if(index > m_width * m_height) continue;

			m_pVertex[ index ].y += value;
			if( m_pVertex[index].y >= m_maxHeight ) {
				m_pVertex[index].y = m_maxHeight;	
			}
	   }
  
   }

   // SmoothTerrain();
	RefreshVBO();
	UpdateNormals();
	UpdateTexture();
}

void Terrain::ModifyVertex( const TerrainHitInfo& info, float value, float R )
{
	unsigned int subject = GetSubjectVert( info );
	int idx_cnt = m_width * m_height;
	float pi = glm::pi<float>();

	std::vector<int> vertices;
	glm::vec3 center = m_pVertex[subject];
	std::vector<float> dist_result;

	GetVertFromRadius( subject, R, vertices, dist_result);
	
	for(int i=0; i < vertices.size(); i++) {
		unsigned int n = vertices[i];
		float distance = dist_result[i];

		m_pVertex[n].y += (distance * value);
		if( m_pVertex[n].y >= m_maxHeight ) {
			m_pVertex[n].y = m_maxHeight;	
		}
	}

	//SmoothTerrain();
	RefreshVBO();
	UpdateNormals();
	UpdateTexture();
}

void Terrain::GetVertFromRadius(int subject, float R, std::vector<int>& output, 
								std::vector<float>& dist_result)
{
	glm::vec3 center = m_pVertex[subject];
	int idx_cnt = m_width * m_height;
	float pi = glm::pi<float>();

	// Get the vertices in the radial distance of the center(subject)
	// vector2 d = p[n] - c;
	for(int n=0; n < idx_cnt; n++ ) {
		glm::vec3 vec = m_pVertex[n] - center;
		if(vec.x*vec.x + vec.z*vec.z < R*R) {
			// point p[n] is insid tehe circle R units from the point c		
			glm::vec3 tmp =  m_pVertex[n] - center;
			tmp = glm::vec3( tmp.x, 0, tmp.z);

			/*  if you want to make a smooth hill with height H and radius R, 
			then select all vertices within a radius R from the selection. 
			Then raise vertex n by H*(.5 + .5*cos(|pn-c|*pi/R)), where pn is 
			the position of the point n and c is the center point. The cosine 
			function makes the hill smooth, with height H at the center and falls 
			of to zero at radius R.

			Credit to: Brother Bob of Gamedev.net,
			http://www.gamedev.net/topic/650606-terrain-editing-question-how-to-make-hills/
			*/

			float distance = 0.5f + (0.5f * cos(glm::length(tmp) * (pi/R)));	
			dist_result.push_back(distance);
			output.push_back(n);
		} 	
	}

}

void Terrain::RefreshVBO()
{
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	GLvoid* position = glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY);
	GLenum error = glGetError();
	if( !position ) return;

	memcpy(position, m_pVertex, m_width * m_height * sizeof(glm::vec3));
	glUnmapBuffer(GL_ARRAY_BUFFER);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

/* Adooted from RTS Book */
void Terrain::SmoothTerrain()
{
	// create a temporary heightmap
	float* hm = new float[m_width * m_height];
	memset(hm, 0, sizeof(float) * m_width * m_height);


	for( int y = 0; y < m_height; y++ ) {
		for( int x = 0; x < m_width; x++ ) {
			float totalHeight = 0.0f;
			int noNodes = 0;
			
			for(int y1=y-1;y1<=y+1;y1++) {
				for(int x1=x-1;x1<=x+1;x1++) {
					if(x1 >= 0 && x1 < m_width && y1 >= 0 && y1 < m_height) {
						totalHeight += m_pVertex[ x1 + y1 * m_width ].y;
						noNodes++;

					}
				}
			}
			hm[ x + y * m_width] = totalHeight / (float) noNodes;
		}
	}


	for(int i=0; i < m_width * m_height; i++) {
		m_pVertex[i].y = hm[i];
	}

	delete[] hm;

}

/* Adopted from RTS Book - 11/30/2013 */
float Noise(int x)
{
    x = (x<<13) ^ x;
    return (1.0 - ((x * (x*x * 15731 + 789221) + 
		1376312589) & 0x7fffffff) / 1073741824.0);    
}

float CosInterpolate(float v1, float v2, float a)
{
	float angle = a * 3.1416f;
	float prc = (1.0f - cos(angle)) * 0.5f;
	return  v1*(1.0f - prc) + v2*prc;
}

//
// Note: THe higher the persistence value the more "mountainy" look it can get, if it is lower
// then we get a rolling hills look.
//
void Terrain::CreateRandomHeightMap(int seed, float noiseSize, float persistence, int octaves)
{
	//For each map node
	for(int y=0;y< m_height;y++)
		for(int x=0;x<m_width;x++)
		{
			//Scale x & y to the range of 0.0 - m_size
			float xf = ((float)x / (float)m_width) * noiseSize;
			float yf = ((float)y / (float)m_height) * noiseSize;

			float total = 0;			

			// For each octave
			for(int i=0;i<octaves;i++)
			{
				//Calculate frequency and amplitude (different for each octave)
				float freq = pow(2.0f, i);
				float amp = pow(persistence, i);

				//Calculate the x,y noise coordinates
				float tx = xf * freq;
				float ty = yf * freq;
				int tx_int = tx;
				int ty_int = ty;

				//Calculate the fractions of x & y
			    float fracX = tx - tx_int;
			    float fracY = ty - ty_int;

				//Get the noise of this octave for each of these 4 points
				float v1 = Noise(tx_int + ty_int * 57 + seed);
				float v2 = Noise(tx_int+ 1 + ty_int * 57 + seed);
				float v3 = Noise(tx_int + (ty_int+1) * 57 + seed);
				float v4 = Noise(tx_int + 1 + (ty_int+1) * 57 + seed);

				//Smooth in the X-axis
				float i1 = CosInterpolate(v1 , v2 , fracX);
				float i2 = CosInterpolate(v3 , v4 , fracX);

				//Smooth in the Y-axis
				total += CosInterpolate(i1 , i2 , fracY) * amp;
			}

			int b = 128 + total * 128.0f;
			if(b < 0)b = 0;
			if(b > 255)b = 255;

			//Save to heightMap
			m_pVertex[x + y * m_width].y = (b / 255.0f) * m_maxHeight;
		}
}

int Terrain::AddTexture( const char* path )
{
	TerrainTexture texture = {0};

	texture.pAlphaData = new unsigned char[ m_width * m_height ];
	memset( texture.pAlphaData, 0, m_width * m_height);

	glGenTextures(1, &texture.alphaTexID);
    glBindTexture(GL_TEXTURE_2D, texture.alphaTexID);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, m_width, m_height, 0, 
		GL_RED, GL_UNSIGNED_BYTE, texture.pAlphaData);
	glBindTexture(GL_TEXTURE_2D, 0);

	texture.texID = TextureLoader::GetTexture( path );

	texture.from = 0.0f;
	texture.to = 0.0f;

	m_textureList.push_back( texture );
	return m_textureList.size() - 1;
}
	
void Terrain::PaintTexture( const TerrainHitInfo& info, 
						   int texIdx, float R ,float alpha, bool unpaint)
{
	unsigned int subject = GetSubjectVert( info );
	TerrainTexture* pTexture = &m_textureList[texIdx];
	std::vector<int> vertices;
	glm::vec3 center = m_pVertex[subject];
	std::vector<float> dist_result;

	GetVertFromRadius( subject, R, vertices, dist_result);

	for(int i=0; i < vertices.size(); i++) {
		unsigned int n = vertices[i];
		float distance = dist_result[i];
		float value = 255.0f * alpha;
		int actual = pTexture->pAlphaData[n] + (value * distance);

		if(actual > value) {
			actual = value;
		}

		if( unpaint ) {
			actual = 0;
		}

		pTexture->pAlphaData[n] = actual;
	}
	
	// Update Texture data
	RefreshTexture(texIdx);
}

void Terrain::SetTextureHeight( int texIdx, float min, float max)
{
	TerrainTexture* pTexture = &m_textureList[texIdx];
	pTexture->from = min;
	pTexture->to = max;

	int vcount = m_width * m_height;
	for(int i=0; i < vcount; i++) { 
		
		if( m_pVertex[i].y >= min && m_pVertex[i].y <= max ) {
			pTexture->pAlphaData[i] = 255;
		}
	}

	// Update Texture data
	RefreshTexture(texIdx);
}

void Terrain::UpdateTexture()
{
	int vcount = m_width * m_height;
	for(int texIdx=0; texIdx < m_textureList.size(); texIdx++ ) {
		TerrainTexture* pTexture = &m_textureList[texIdx];
		for(int i=0; i < vcount; i++) { 
			
			if( pTexture->from == 0.0f && pTexture->to == 0.0f ) break;

			if( m_pVertex[i].y >= pTexture->from && m_pVertex[i].y <= pTexture->to ) {
				pTexture->pAlphaData[i] = 255;
			} else {
				pTexture->pAlphaData[i] = 0;
			}
		}

		RefreshTexture(texIdx);
	}
}	

void Terrain::RefreshTexture( int index )
{
	TerrainTexture* pTexture = &m_textureList[index];
	
	// Update Texture data
	glBindTexture(GL_TEXTURE_2D, pTexture->alphaTexID);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, m_width, m_height, 0, 
		GL_RED, GL_UNSIGNED_BYTE, pTexture->pAlphaData);
	glBindTexture(GL_TEXTURE_2D, 0);

}

void Terrain::SaveToFile(FILE* fp)
{
#ifdef WIN32	
	//...
	if( fp == NULL ) return;

	fprintf(fp,"terrrain_dimensison %d %d %f \n", m_width, m_height, m_tileDist);

	std::string strBaseFileName = TextureLoader::GetFileName(baseTexID);
	strBaseFileName = _GetFileName(strBaseFileName.c_str());
	fprintf(fp,"terrain_base_texture %s \n", strBaseFileName.c_str());
	fprintf(fp,"terrain_repeat_rate %d \n", m_repeatRate);

	for(int i=0; i < m_textureList.size(); i++){
		TerrainTexture texture = m_textureList[i];
		strBaseFileName = TextureLoader::GetFileName(texture.texID);
		strBaseFileName = _GetFileName(strBaseFileName.c_str());
		fprintf(fp,"terrain_texture %s \n", strBaseFileName.c_str());

		for(int v=0; v < m_width * m_height; v++) {
			//...
			fprintf(fp,"terrain_alphamap %d %d \n", i, (int)texture.pAlphaData[v]);
		}
	}

	for(int i=0; i < m_width * m_height; i++) {
		fprintf(fp,"terrain_hmap %f \n", m_pVertex[i].y);
	}
#endif
}

void Terrain::SaveTextures(const char* texPath)
{
#ifdef WIN32
	std::string  strBaseFileName = TextureLoader::GetFileName(baseTexID);
	
	std::string targetmode = std::string(texPath) + "\\" +  
			_GetFileName(strBaseFileName.c_str());

	BOOL ret = ::CopyFileA( strBaseFileName.c_str(),
			targetmode.c_str(),
			FALSE);

	for(int i=0; i < m_textureList.size(); i++){
		TerrainTexture texture = m_textureList[i];
		std::string  strBaseFileName = TextureLoader::GetFileName(texture.texID);
		
		// copy the mesh file
		targetmode = std::string(texPath) + "\\" +  
			_GetFileName(strBaseFileName.c_str());

		ret = ::CopyFileA( strBaseFileName.c_str(),
			targetmode.c_str(),
			FALSE);
	}
#endif

}

void Terrain::FrustumClip( Frustum& frustum )
{
	// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!.
	// This implementation is SUPER SLOWW!!!
	// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	unsigned int* pIdx = m_pIndices;
	m_pVisibleIndices.clear();

	for( int i=0; i < m_IndexCount; i+=3 ) {
		unsigned int v0idx = *pIdx++;
		unsigned int v1idx = *pIdx++;
		unsigned int v2idx = *pIdx++;

		glm::vec3 v0 = m_pVertex[v0idx];
		glm::vec3 v1 = m_pVertex[v1idx];
		glm::vec3 v2 = m_pVertex[v2idx];

		int face = 0;
		if( frustum.CheckPoint( v0 ) ) {
			face++;
		} else if( frustum.CheckPoint( v1 ) ) {
			face++;
		} else if( frustum.CheckPoint( v2 ) ) {
			face++;
		}

		if( face == 0 ) 
			continue;
		
		m_pVisibleIndices.push_back(v0idx);
		m_pVisibleIndices.push_back(v1idx);
		m_pVisibleIndices.push_back(v2idx);
	}

	// Refresh Index buffer
	m_drawnIndexCount = m_pVisibleIndices.size();
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
	GLvoid* indices = glMapBuffer(GL_ELEMENT_ARRAY_BUFFER, GL_WRITE_ONLY);
	GLenum error = glGetError();
	if( !indices ) return;

	memcpy(indices, (void*)&m_pVisibleIndices[0], 
		m_drawnIndexCount * sizeof(unsigned int));
	
	glUnmapBuffer(GL_ELEMENT_ARRAY_BUFFER);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

}
