#include "TerrainShaderFx.h"

TerrainShaderFx::TerrainShaderFx()
{
	//...
}

TerrainShaderFx::~TerrainShaderFx()
{
	//...
}


void TerrainShaderFx::Load()
{
	Init();
	LoadVertexShaderFromFile("shaders/terrain_vshader.txt");
	LoadFragmentShaderFromFile("shaders/terrain_fshader.txt");
	Build();
	Use();

	m_VP = GetUniformLocation("gVP");
	if(  0xFFFFFFFF == m_VP) {
		Log::Dbg("Error getting gVP object");
	}/**/

	m_repeatRate = GetUniformLocation("repeatRate");
	if(  0xFFFFFFFF == m_repeatRate) {
		Log::Dbg("Error getting repeatRate object");
	}/**/

	m_baseTexture = GetUniformLocation("gBaseSampler");
	if(  0xFFFFFFFF == m_baseTexture) {
		Log::Dbg("Error getting gBaseSampler object");
	}/**/


	m_numTextures	= GetUniformLocation("numTextures");
	if(  0xFFFFFFFF == m_numTextures) {
		Log::Dbg("Error getting numTextures object");
	}/**/

	for(int i = 0; i < 3; i++) {
		char name[30];
		sprintf(name, "gTextureMap[%d]",i);
		
		m_samplers[i] = GetUniformLocation( name );
		if(  0xFFFFFFFF == m_VP) {
			Log::Dbg("Error Getting sampler %d", i);
		}/**/

		sprintf(name, "gAlphaMap[%d]",i);
		m_alpha[i] = GetUniformLocation( name );
		if(  0xFFFFFFFF == m_VP) {
			Log::Dbg("Error Getting sampler %d", i);
		}/**/

	}

	//Must be called if USING COMMON shader
	LoadIncludes();
}

void TerrainShaderFx::ActivateSamplers(int count)
{
	int texture_unit = 1;

    glUniform1i( m_numTextures, count);

	for(int i=0; i < count; i++) {
		glUniform1i(m_samplers[i], texture_unit++);
		glUniform1i(m_alpha[i], texture_unit++);
	}
}



void TerrainShaderFx::SetVPMatrix(glm::mat4& vp)
{
	SetUniformMatrix( m_VP, vp );
}

void TerrainShaderFx::SetRepeatRate(float rate)
{
	glUniform1f( m_repeatRate, rate);
}

void TerrainShaderFx::ActivateBaseTexture()
{
	glUniform1i(m_baseTexture, 0);
}
