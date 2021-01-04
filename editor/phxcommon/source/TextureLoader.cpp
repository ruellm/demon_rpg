#include "TextureLoader.h"

std::vector <TextureResource> TextureLoader::m_list;
TextureLoader* TextureLoader::m_pInstance = NULL;

TextureLoader::TextureLoader(void)
{
}


TextureLoader::~TextureLoader(void)
{
}

GLuint TextureLoader::GetTexture(const char* szFname, bool invert )
{
	TextureResource resource;
	std::string strPath(szFname);
	GLuint texID = 	 GetResource (strPath.c_str());
	unsigned int flags = SOIL_FLAG_MIPMAPS | SOIL_FLAG_NTSC_SAFE_RGB;

	if( invert ) {
		flags |= SOIL_FLAG_INVERT_Y;
	}

	if( texID == 0 ) {
		texID = SOIL_load_OGL_texture
		(
			strPath.c_str(),
			SOIL_LOAD_AUTO,
			SOIL_CREATE_NEW_ID,
			flags
		);
	
		if( 0 != texID )
		{
			// "Bind" the newly created texture : all future texture functions will modify this texture
			glBindTexture(GL_TEXTURE_2D, texID);

			// Nice trilinear filtering.
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
			glGenerateMipmap(GL_TEXTURE_2D);

			resource.texID = texID;
			resource.currDir = resource.oldDir = strPath;
			resource.oldDir = resource.currDir;
			m_list.push_back(resource);

			glBindTexture(GL_TEXTURE_2D, 0);
		}

	}
	return texID;
}

GLuint TextureLoader::GetTexture(const char* szFname, const char* szDir, 
				   bool invert )
{
	TextureResource resource;
	std::string strFname(szFname);
	std::string strDir(szDir);
	std::string strPath = strDir + "/" + strFname;
	return GetTexture(strPath.c_str(), invert);
}

void TextureLoader::Cleanup()
{
	for(int i=0; i < m_list.size(); i++ ) {
		TextureResource resource = m_list[i];
		glDeleteTextures(1, &resource.texID);
	}
	m_list.empty();
}

GLuint TextureLoader::GetResource(const char* szPath)
{

	for(int i=0; i < m_list.size(); i++ ) {
		TextureResource resource = m_list[i];
		if( strcmp(resource.currDir.c_str(), szPath) == 0 ||
			strcmp(resource.oldDir.c_str(), szPath) == 0 ) {
			return resource.texID;
		}
	}
	return 0;
}

 std::string TextureLoader::GetFileName( GLuint texID )
 {

	bool found = false;
	int i = 0;
	std::string strFname("");

	for( i=0; i < m_list.size(); i++ ) {
		TextureResource resource = m_list[i];
		if( resource.texID == texID ) {
			found = true;
			break;
		}
	}

	if( found ) {
		//return _GetFileName( m_list[i].currDir.c_str() );
		return m_list[i].currDir.c_str();
	}

	return strFname;
 }

 void TextureLoader::SaveSession(const char* saveDir)
 {
	//...
	 std::string strSaveDir(saveDir);

	 for(int i=0; i < m_list.size(); i++ ) {
		TextureResource* resource = &m_list[i];
		std::string filename = _GetFileName(resource->currDir.c_str());
		std::string newLocation = 	strSaveDir + "/" + filename;

#ifdef WIN32
		BOOL ret = ::CopyFileA( resource->currDir.c_str(),
			newLocation.c_str(),
			FALSE);

		if( ret ) {
			resource->currDir = newLocation;
		}
#endif
	}

 }

 void TextureLoader::OpenSession(const char* dir)
 {
	  std::string strSaveDir(dir);

	 for(int i=0; i < m_list.size(); i++ ) {
		TextureResource* resource = &m_list[i];
		std::string filename = _GetFileName(resource->currDir.c_str());
		resource->currDir = 	strSaveDir + "/" + filename;
	 }
 }