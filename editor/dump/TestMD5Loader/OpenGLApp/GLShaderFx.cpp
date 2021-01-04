#include <stdio.h>
#include "GLShaderFx.h"


GLShaderFx::GLShaderFx(void)
{
	//...
}


GLShaderFx::~GLShaderFx(void)
{
	//...
	//glDeleteShader(VertexShaderID);
    //glDeleteShader(FragmentShaderID);
}

void GLShaderFx::Init()
{
	m_program = glCreateProgram();
}

void GLShaderFx::LoadVertexShaderFromFile(const char* file)
{
	m_vertexShader = LoadShaderFromFile(file, GL_VERTEX_SHADER);
	glAttachShader(m_program, m_vertexShader);
}

void GLShaderFx::LoadFragmentShaderFromFile(const char* file)
{
	m_fragmentShader = LoadShaderFromFile(file, GL_FRAGMENT_SHADER);
	glAttachShader(m_program, m_fragmentShader);
}

void GLShaderFx::Build()
{
	glLinkProgram(m_program);
	GLenum error = glGetError();

	// Get LINKING status
	GLint Success = 0;
	GLchar ErrorLog[1024] = { 0 };
	glGetProgramiv(m_program, GL_LINK_STATUS, &Success);
	if (Success == 0) {
		glGetProgramInfoLog(m_program, sizeof(ErrorLog), NULL, ErrorLog);

		//----------------------------------
		fprintf(stderr, "Error linking shader program: '%s'\n", ErrorLog);
		//----------------------------------
	}
}

void GLShaderFx::Use()
{
	glUseProgram(m_program);
	GLenum error = glGetError();
}

GLuint GLShaderFx::LoadShaderFromFile(const char* file, GLenum type)
{
	unsigned long size = 0;
	int ret = 0;
	GLuint shader = 0;

	FILE* fp = fopen(file, "r");
	fseek(fp, 0, SEEK_END);
	size = ftell(fp);
	fseek(fp, 0, SEEK_SET);

	unsigned char* szShaderSrc = new unsigned char[size];
	fread(szShaderSrc, size, 1, fp);

	//**
	shader = LoadShader( (const char*)szShaderSrc, type, size );
	//**

	delete[] szShaderSrc;
	if(fp) {
		fclose(fp);
	}

	return shader;
}

GLuint GLShaderFx::LoadShader(const char* szShaderSrc, GLenum type, int size)
{
	GLuint shader = 0;

	// Create the shader object
	shader = glCreateShader(type);
	


	// Load the shader source
	if( size ) {
		glShaderSource(shader,1, &szShaderSrc, &size);
	} else {
		glShaderSource(shader,1, &szShaderSrc, NULL);
	}

	// Compile the shader
	glCompileShader(shader);

	GLint success;
	glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
	if (!success) {
		GLchar InfoLog[1024];
		glGetShaderInfoLog(shader, sizeof(InfoLog), NULL, InfoLog);
	
		//----------------------------------
		char msg[1024];
		sprintf_s(msg,"%s", InfoLog);
		::OutputDebugStringA(msg);
		//----------------------------------
	
	}

	return shader;
}

int GLShaderFx::GetUniformLocation(const char* variable)
{
	return glGetUniformLocation(m_program, variable);
}

void GLShaderFx::SetUniformMatrix( int uniformid, glm::mat4& matrix)
{
	glUniformMatrix4fv(uniformid, 1, GL_FALSE, glm::value_ptr(matrix));
}