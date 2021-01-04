#pragma once

#include "common.h"

class GLShaderFx
{
public:
	GLShaderFx(void);
	~GLShaderFx(void);

	GLuint LoadShaderFromFile(const char* file, GLenum type);
	GLuint LoadShader(const char* szShaderSrc, GLenum type, int size = 0);

	void LoadVertexShaderFromFile(const char* file);
	void LoadFragmentShaderFromFile(const char* file);
	void Build();
	void Use();
	void Init();

	int GetUniformLocation(const char* variable);
	void SetUniformMatrix( int uniformid, glm::mat4& matrix);

	GLuint GetProgram() { return m_program; }

private:
	GLuint	m_program;
	GLuint	m_vertexShader;
	GLuint	m_fragmentShader;
};

