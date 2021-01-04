#include "OBJModel.h"
#include "TextureLoader.h"

OBJModel::OBJModel(void) :
	Model3D()
{
	SetModelType( MODEL_OBJ );
}


OBJModel::~OBJModel(void)
{
	Destroy();
}

void OBJModel::Destroy()
{
	  for(int x=0; x < m_meshList.size(); x++) {
			OBJMesh* meshActual = &m_meshList[x];

			glDeleteBuffers(1, &meshActual->vbo);
			glDeleteBuffers(1, &meshActual->vbo_uv);
			glDeleteBuffers(1, &meshActual->vbo_normals);
			glDeleteVertexArrays(1, &meshActual->vao);
			 
	  }
	  m_meshList.clear();
}

bool OBJModel::LoadFromFile (const char* szFile, const char* szDirectory)
{
	// Store the Path variable
	m_strPathDir = std::string( szDirectory );
	m_strFName = std::string( szFile );

	std::string path = m_strPathDir + "/" + std::string(szFile);

	if( !ParseFile(path.c_str()) ) return false;

	//CalcNormals();	//TODO: Calculate Normals

	UploadBuffers( );

	return true;
}

bool OBJModel::ParseFile(const char* szFile )
{
	//-------------------------------------------------------------------------------------------
	typedef struct OBJMAttributeSet
	{
		std::string material_name;
		std::vector<unsigned int> vertexIndices;
		std::vector<unsigned int> uvIndices;
		std::vector<unsigned int> normalIndices;

	} OBJMGroupSet;
	//-------------------------------------------------------------------------------------------
	
	FILE* fp = NULL;
	std::vector<glm::vec3> out_vertices; 
	std::vector<glm::vec2> out_uvs;
	std::vector<glm::vec3> out_normals;

	std::vector<unsigned int> vertexIndices, uvIndices, normalIndices;
	std::vector<glm::vec3> temp_vertices; 
	std::vector<glm::vec2> temp_uvs;
	std::vector<glm::vec3> temp_normals;

	std::vector<std::string> mtllib_list;
	std::string strActiveMatrial;

	// Create attribute mesh object
	OBJMAttributeSet* mesh = NULL;
	std::vector<OBJMAttributeSet> mesh_list;

	//-------------------------------------------------------------------------------------------
	//Loading and parsing file
	fp = fopen(szFile, "r");
	if( fp == NULL ){ return false;	}
	
	while( 1 ) {

		char lineHeader[128];
		// read the first word of the line
		int res = fscanf(fp, "%s", lineHeader);
		if (res == EOF)
			break; // EOF = End Of fp. Quit the loop.

		if ( strcmp( lineHeader, "v" ) == 0 ){
			glm::vec3 vertex;
			fscanf(fp, "%f %f %f\n", &vertex.x, &vertex.y, &vertex.z );
			temp_vertices.push_back(vertex);
		}else if ( strcmp( lineHeader, "vt" ) == 0 ){
			glm::vec2 uv;
			fscanf(fp, "%f %f\n", &uv.x, &uv.y );
			//uv.y = uv.y; 
			temp_uvs.push_back(uv);
		}else if ( strcmp( lineHeader, "vn" ) == 0 ){
			glm::vec3 normal;
			fscanf(fp, "%f %f %f\n", &normal.x, &normal.y, &normal.z );
			temp_normals.push_back(normal);
		}else if ( strcmp( lineHeader, "f" ) == 0 ){

			std::string vertex1, vertex2, vertex3;

			unsigned int vertexIndex[3]={0}, uvIndex[3] = {0}, normalIndex[3] = {0};

			int matches = fscanf(fp, "%d/%d/%d %d/%d/%d %d/%d/%d\n", 
				&vertexIndex[0], &uvIndex[0], &normalIndex[0], 
				&vertexIndex[1], &uvIndex[1], &normalIndex[1], 
				&vertexIndex[2], &uvIndex[2], &normalIndex[2] );

			if (matches == 9){
				mesh->vertexIndices.push_back(vertexIndex[0]);
				mesh->vertexIndices.push_back(vertexIndex[1]);
				mesh->vertexIndices.push_back(vertexIndex[2]);
				mesh->uvIndices    .push_back(uvIndex[0]);
				mesh->uvIndices    .push_back(uvIndex[1]);
				mesh->uvIndices    .push_back(uvIndex[2]);
				mesh->normalIndices.push_back(normalIndex[0]);
				mesh->normalIndices.push_back(normalIndex[1]);
				mesh->normalIndices.push_back(normalIndex[2]);	
			} else if( matches == 1) {
				
				matches = fscanf(fp, "%d %d \n", &vertexIndex[1], &vertexIndex[2]); 
				if( matches == 2 ) {
					//only vertex information available: f v1 v2 v3
					mesh->vertexIndices.push_back(vertexIndex[0]);
					mesh->vertexIndices.push_back(vertexIndex[1]);
					mesh->vertexIndices.push_back(vertexIndex[2]);

				}  else if (matches == 0 ){
					// Vertex/normal only f v1//vn1 v2//vn2 v3//vn3 ...
					matches = fscanf(fp, "/%d %d//%d %d//%d\n", 
						&normalIndex[0], 
						&vertexIndex[1],  &normalIndex[1], 
						&vertexIndex[2],  &normalIndex[2]);

					mesh->vertexIndices.push_back(vertexIndex[0]);
					mesh->vertexIndices.push_back(vertexIndex[1]);
					mesh->vertexIndices.push_back(vertexIndex[2]);
					mesh->normalIndices.push_back(normalIndex[0]);
					mesh->normalIndices.push_back(normalIndex[1]);
					mesh->normalIndices.push_back(normalIndex[2]);

				}
			} else if (matches == 2) {
				// Vertex and Texture coords only: f v1/vt1 v2/vt2 v3/vt3 
				 int matches = fscanf(fp, "%d/%d %d/%d \n", 
					&vertexIndex[1], &uvIndex[1], 
					&vertexIndex[2], &uvIndex[2] );

				mesh->vertexIndices.push_back(vertexIndex[0]);
				mesh->vertexIndices.push_back(vertexIndex[1]);
				mesh->vertexIndices.push_back(vertexIndex[2]);
				mesh->uvIndices    .push_back(uvIndex[0]);
				mesh->uvIndices    .push_back(uvIndex[1]);
				mesh->uvIndices    .push_back(uvIndex[2]);
			}

		} else if( strcmp( lineHeader, "mtllib" ) == 0 ){
				// Load material in here
				char szName[100];
				fscanf(fp, "%s", szName);
				std::string mtlname(szName);
				mtllib_list.push_back(szName);

		} else if ( strcmp(lineHeader, "usemtl") == 0)  {
				char szName[100];
				fscanf(fp, "%s", szName);
				std::string mtlname(szName);
				
				OBJMAttributeSet currmesh;
				currmesh.material_name = mtlname;
				mesh_list.push_back(currmesh);
				mesh = &mesh_list[mesh_list.size()-1];

		} else{
			// Probably a comment, eat up the rest of the line
			char stupidBuffer[1000];
			fgets(stupidBuffer, 1000, fp);
		}
	}
	//-------------------------------------------------------------------------------------------

	//
	// Load the Materials
	//
	std::map < std::string, OBJMMaterial > result;
	LoadMaterials( mtllib_list, result );

	//
	// Calculate Normal vectors
	//
	glm::vec3* pNormals = 0;

	if( temp_normals.size() <= 0 ) {
		pNormals = new glm::vec3[temp_vertices.size()];

		for( int x=0; x < mesh_list.size(); x++ ) {
			OBJMAttributeSet* mesh = &mesh_list[x];

			for( unsigned int i=0; i < mesh->vertexIndices.size(); i += 3 ) {
					unsigned int v0Idx =  mesh->vertexIndices[i]-1;
					unsigned int v1Idx =  mesh->vertexIndices[i+1]-1;
					unsigned int v2Idx =  mesh->vertexIndices[i+2]-1;

					glm::vec3 v0 = temp_vertices[v0Idx];
					glm::vec3 v1 = temp_vertices[v1Idx];
					glm::vec3 v2 = temp_vertices[v2Idx];

					glm::vec3 normal = glm::normalize( glm::cross( v1 - v0, v2 - v0 ) );
				
					pNormals[ v0Idx ] += normal;
					pNormals[ v1Idx ] += normal;
					pNormals[ v2Idx ] += normal;
			 }
		} 

		for(int i = 0; i <  temp_vertices.size(); i++) {
			pNormals[i] = glm::normalize( pNormals[i] ); 
		}
	}
	//
	// Prepare the Data
	//
	for( int x=0; x < mesh_list.size(); x++ ) {

		OBJMAttributeSet* mesh = &mesh_list[x];
		OBJMesh meshActual;

		for( unsigned int i=0; i < mesh->vertexIndices.size(); i++ ) {

            // Get the indices of its attributes
            int vertexIndex = mesh->vertexIndices[i]-1;
			int uvIndex = ( mesh->uvIndices.size() > 0) ? mesh->uvIndices[i] : -1;
			int normalIndex = ( mesh->normalIndices.size() > 0) ? mesh->normalIndices[i] : -1;
                
			meshActual.vertexBuffer.push_back( temp_vertices[ vertexIndex ] );

			if( uvIndex != -1 ) {
				meshActual.texCoordBuffer.push_back( temp_uvs[ uvIndex-1 ] );
			}
           
			
			if(normalIndex != -1 ){ 
				meshActual.normalBuffer.push_back( temp_normals[ normalIndex-1 ] );
			} else {	
				meshActual.normalBuffer.push_back( pNormals[vertexIndex] );
			}

			// Assign materials
			meshActual.material = result[ mesh->material_name ];
		}

		meshActual.hasTexture = ( mesh->uvIndices.size() <= 0 )? false: true;
		meshActual.hasNormal = true;
		//meshActual.hasNormal = ( mesh->normalIndices.size() <= 0 )? false: true;
		m_meshList.push_back(meshActual);

	}

	if( fp ) {	fclose(fp);	}
	SAFE_DELETE(pNormals);


	return true;
}

void OBJModel::UploadBuffers( )
{
	 for(int x=0; x < m_meshList.size(); x++) {
		OBJMesh* mesh = &m_meshList[x];
		
		glGenVertexArrays(1, &mesh->vao);
		glBindVertexArray(mesh->vao);

		glGenBuffers(1, &mesh->vbo);
		glBindBuffer(GL_ARRAY_BUFFER, mesh->vbo);

		glBufferData(GL_ARRAY_BUFFER, mesh->vertexBuffer.size() * sizeof(glm::vec3), 
			(void*)&mesh->vertexBuffer[0], GL_STATIC_DRAW);

		 glVertexAttribPointer( 
				0,                           // attribute 0. No particular reason for 0, but must match the layout in the shader. 
				3,                           // size or count of elements 
				GL_FLOAT,           // type 							
				GL_FALSE,           // normalized? 						
				0,                         // stride 					
				(void*)0            // array buffer offset 				
				); 

		if(   mesh->texCoordBuffer.size() > 0 ) {
			glGenBuffers(1, &mesh->vbo_uv);
			glBindBuffer(GL_ARRAY_BUFFER, mesh->vbo_uv);

			glBufferData(GL_ARRAY_BUFFER, mesh->texCoordBuffer.size() * sizeof(glm::vec2), 
				(void*)&mesh->texCoordBuffer[0], GL_STATIC_DRAW);

			 glVertexAttribPointer( 
					1,                            
					2,                            
					GL_FLOAT,           							
					GL_FALSE,            						
					0,                   					
					(void*)0            				
					); 

		}

		if( mesh->hasNormal ) {
			// Normal Vertex Buffer Object
			glGenBuffers(1, &mesh->vbo_normals);
			glBindBuffer(GL_ARRAY_BUFFER, mesh->vbo_normals);

			glBufferData(GL_ARRAY_BUFFER, mesh->normalBuffer.size() * sizeof(glm::vec3), 
				(void*)&mesh->normalBuffer[0], GL_STATIC_DRAW);

			 glVertexAttribPointer( 
					2,                            
					3,                            
					GL_FLOAT,           							
					GL_FALSE,            						
					0,                   					
					(void*)0            				
				); 

		}
		glBindVertexArray(0);
		glBindBuffer(GL_ARRAY_BUFFER,0);
	 }
}

void OBJModel::Draw( )
{
	  for(int x=0; x < m_meshList.size(); x++) {
			OBJMesh* mesh = &m_meshList[x];

			glBindVertexArray(mesh->vao);
			glEnableVertexAttribArray(0);
			
			if( mesh->hasTexture ) {
				glEnableVertexAttribArray(1);
			}
			
			if( mesh->hasNormal ) {
				glEnableVertexAttribArray(2);
			}

			GLsizei size = mesh->vertexBuffer.size();

			//Texture Setup
			glActiveTexture(GL_TEXTURE0);
			glBindTexture( GL_TEXTURE_2D, mesh->material.texID );
			glDrawArrays(GL_TRIANGLES, 0, size);
			
			GLenum error = glGetError();

			glDisableVertexAttribArray(0);
			glDisableVertexAttribArray(1);
			glDisableVertexAttribArray(2);
			glBindVertexArray(0);

	  }
}

void OBJModel::LoadMaterials( const std::vector<std::string>& mtllib_list, 
	std::map<std::string, OBJMMaterial>& result )
{
	OBJMMaterial* pCurrent = NULL;

	for( int i = 0; i < mtllib_list.size(); i++ ) {
		std::string fileterial = mtllib_list[i];
		
		m_mtlFileList.push_back( fileterial );

		FILE* fp = NULL;
		std::string fname = _GetFileName(fileterial.c_str());
		std::string fullpath = 	m_strPathDir + std::string("/") + fname;
		if ( (fp = fopen( fullpath.c_str(), "r" )) == 0 ) continue;

		while ( 1 ) { 
				char lineHeader[128];
				// read the first word of the line
				int res = fscanf(fp, "%s", lineHeader);
				if (res == EOF)
					break; // EOF = End Of fp. Quit the loop.

				if( strcmp( lineHeader, "newmtl" ) == 0 ) {
					// material definition
					char szName[100];
					fscanf(fp, "%s", szName);
					std::string mtlname(szName);

					// new material created
					pCurrent = &result[mtlname];
	
				} else if (strcmp( lineHeader, "Ka" ) == 0) {
					// ambient material
					if( pCurrent == NULL ) continue;
					
					fscanf(fp, "%f %f %f\n", &pCurrent->ambient.x, 
						&pCurrent->ambient.y, &pCurrent->ambient.z);

				} else if (strcmp( lineHeader, "Kd" ) == 0) {
					// diffuse material
					if( pCurrent == NULL ) continue;
					
					fscanf(fp, "%f %f %f\n", &pCurrent->diffuse.x, 
						&pCurrent->diffuse.y, &pCurrent->diffuse.z);

				} else if (strcmp( lineHeader, "Ks" ) == 0) {
					// specular material
					if( pCurrent == NULL ) continue;
					
					fscanf(fp, "%f %f %f\n", &pCurrent->specular.x, 
						&pCurrent->specular.y, &pCurrent->specular.z);

				} else if (strcmp( lineHeader, "map_Kd" ) == 0) {
					// Diffuse Texture map
					if( pCurrent == NULL ) continue;
					
					char szName[100];
					fscanf(fp, "%s", szName);
					std::string mtlname(szName);
					pCurrent->strTextureName = mtlname;


				}
				else{
					// Probably a comment, eat up the rest of the line
					char stupidBuffer[1000];
					fgets(stupidBuffer, 1000, fp);
				}
		}

		if( fp ) fclose(fp);
	}

	//---------------------------------------------
	// Load Texture using SOIL - Opengl library
	// !!!NOTE: USE TEXTURE RESOURCE LOADER IN THE ENGINE!!!
	typedef std::map<std::string, OBJMMaterial>::iterator it_type;
	for(it_type it = result.begin(); it != result.end(); it++) {
		OBJMMaterial* material = &it->second;

		//--------------------------------
		std::string fname = _GetFileName(material->strTextureName.c_str());
		material->texID = TextureLoader::GetTexture(fname.c_str(), m_strPathDir.c_str());
		if( 0 == material->texID )
		{
			Log::Dbg( "SOIL loading error: '%s' %s \n", SOIL_last_result(), fname.c_str());
		}
		//--------------------------------
	}

	//---------------------------------------------
}

void OBJModel::OnSave( const char* szSavePath )
{
#ifdef WIN32
	
	// copy the mesh file
	std::string fullname = GetPathDir() + "\\" 
		+  GetFileName();

	std::string targetmode = std::string(szSavePath) + "\\" +  
		GetFileName();

	BOOL ret = ::CopyFileA( fullname.c_str(),
		targetmode.c_str(),
		FALSE);

	for(int x=0; x < m_meshList.size(); x++) {
			OBJMesh* mesh = &m_meshList[x];

			fullname =  GetPathDir() + "\\" + 
				mesh->material.strTextureName;
			targetmode = std::string(szSavePath) + "\\" +  
					mesh->material.strTextureName;
			
			ret = ::CopyFileA( fullname.c_str(),
				targetmode.c_str(),
				FALSE);

	}

	for(int i=0; i < m_mtlFileList.size(); i++ )
	{
		// copy the mesh file
		std::string fullname = GetPathDir() + "\\" 
			+  m_mtlFileList[i].c_str();

		std::string targetmode = std::string(szSavePath) + "\\" +  
			m_mtlFileList[i].c_str();

		BOOL ret = ::CopyFileA( fullname.c_str(),
			targetmode.c_str(),
			FALSE);
	}
#endif

}

