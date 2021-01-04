#pragma once

#include "common.h"
#include "GLShaderFx.h"

class TerrainShaderFx : public GLShaderFx
{
public:
	  TerrainShaderFx();
	  ~TerrainShaderFx();

	  void Load();
	  void ActivateSamplers(int count);
	  void SetVPMatrix(glm::mat4& vp);
	  void SetRepeatRate(float rate);
	  void ActivateBaseTexture();

private:
	GLuint m_VP;
	GLuint m_repeatRate;
	GLuint m_baseTexture;
	GLuint m_samplers[3];
	GLuint m_alpha[3];

	GLuint m_numTextures;
};