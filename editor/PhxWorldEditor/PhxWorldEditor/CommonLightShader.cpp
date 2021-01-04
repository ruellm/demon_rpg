#include "CommonLightShader.h"


CommonLightShader::CommonLightShader(void)
{
}


CommonLightShader::~CommonLightShader(void)
{
	for(int i=0; i < m_params.size(); i++) {
		SAFE_DELETE(m_params[i]);
	}
}

void CommonLightShader::Load(GLuint program)
{

	LSParameters* param = new LSParameters();
	memset(param, 0, sizeof(LSParameters));
	param->program = program;
	m_params.push_back(param);

	param->dirColor = glGetUniformLocation(program,"gDirectionalLight.Color");
	if(  0xFFFFFFFF == param->dirColor) {
		Log::Dbg("Error getting gDirectionalLight.Color object");
	}/**/

	param->ambientColor = glGetUniformLocation(program,"gAmbientColor");
	if(  0xFFFFFFFF == param->ambientColor) {
		Log::Dbg("Error getting ambientColor object");
	}/**/

	param->dirAmbientIntensity = glGetUniformLocation(program,"gAmbientIntensity");
	if(  0xFFFFFFFF == param->dirAmbientIntensity) {
		Log::Dbg("Error getting gDirectionalLight.AmbientIntensity object");
	}/**/

	param->dirDiffIntensity = glGetUniformLocation(program,"gDirectionalLight.DiffuseIntensity");
	if(  0xFFFFFFFF == param->dirDiffIntensity) {
		Log::Dbg("Error getting gDirectionalLight.DiffuseIntensity object");
	}/**/

	param->direction = glGetUniformLocation(program,"gDirectionalLight.Direction");
	if(  0xFFFFFFFF == param->direction) {
		Log::Dbg("Error getting gDirectionalLight.Direction object");
	}/**/

}
 
void CommonLightShader::Use( GLuint program )
 {
	LSParameters* param = NULL;
	for(int i=0; i < m_params.size(); i++) {
	
		if( m_params[i]->program == program )  {
			param = m_params[i];
			break;
		}
	}

	if( param == NULL ) return;

	glUniform1f( param->dirAmbientIntensity, m_ambient.intensity);
	glUniform3f(param->ambientColor, m_ambient.color.x,
		m_ambient.color.y, m_ambient.color.z);

	glUniform1f( param->dirDiffIntensity, m_dirLight.intensity);
	glm::vec3 dir = glm::normalize(m_dirLight.direction);
	glUniform3f(param->direction, m_dirLight.direction.x, 
			m_dirLight.direction.y, m_dirLight.direction.z);

	glUniform3f(param->dirColor, m_dirLight.color.x, m_dirLight.color.y, 
			m_dirLight.color.z); 
}

void CommonLightShader::SetEnvLight(const DirectionalLight& dirlight, const AmbientLight& amb)
{
	m_dirLight = dirlight;
	m_ambient = amb;
}