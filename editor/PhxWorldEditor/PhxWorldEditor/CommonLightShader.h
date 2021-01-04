#pragma once

#include "GLShaderFx.h"
#include "Lights.h"

class CommonLightShader : public CommonShaderBase
{
public:
	
	struct LSParameters	{
		GLuint dirColor;
		GLuint ambientColor;
		GLuint dirAmbientIntensity;
		GLuint dirDiffIntensity;
		GLuint direction;
		GLuint program;
	};


	CommonLightShader(void);
	~CommonLightShader(void);

	virtual void Load(GLuint program);

	void SetEnvLight(const DirectionalLight& dirlight, const AmbientLight& amb);
	void Use( GLuint program );

private:
	DirectionalLight m_dirLight;
	AmbientLight m_ambient;

	std::vector<LSParameters*> m_params;
};

