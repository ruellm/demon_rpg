#pragma once

#include "common.h"

typedef struct TextureResource
{
	GLuint texID;
	std::string oldDir;
	std::string currDir;
} TextureResource; 

class TextureLoader
{
private:
	static std::vector< TextureResource > m_list;
	static TextureLoader* m_pInstance;

	static GLuint GetResource(const char* szPath);

public:
	TextureLoader(void);
	~TextureLoader(void);
	static GLuint GetTexture(const char* szFname, const char* szDir, bool invert = true);
	static GLuint GetTexture(const char* szFname, bool invert = true);
	static std::string GetFileName( GLuint texID );
	static void SaveSession(const char* saveDir);
	static void OpenSession(const char* dir);
	static void Cleanup();

};

