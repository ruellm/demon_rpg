// MD5 Loader 
// Written: Ruell Magpayo
// April 20, 2013 9:00am PST
// @ Westchester Garden Apts, 6680 West 86th Place, Los Angeles, CA 90045
// -- Ported to OpenGL 3.3 November 23, 2013 --

#include "MD5Model.h"
#include <iostream>
#include <fstream>
#include <math.h>	//for floor and ceil

using namespace std;

MD5Model::MD5Model(void)
{
	SetModelType( MODEL_MD5 );
}


MD5Model::~MD5Model(void)
{
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// Parsing related functions
////////////////////////////////////////////////////////////////////////////////////////////////////////////
std::string MD5Model::GetNextToken( std::fstream* file )
{
	std::string strToken = "";
	*file >> strToken;
	
	while (strToken == " " || 
		/*strToken == "{" ||
		strToken == "{" ||*/
		strToken == "(" || 
		strToken == ")" ||
		strToken =="//" ) {

		if(strToken == "//") {
			std::getline (*file, strToken);
		}
		
		*file >> strToken;
	}
	
	if ( file->eof() ) {
		return ("");
	}

	//Detect Strings
	if (strToken.at(0) == '\"' && 
		strToken.at(strToken.length()-1) != '\"'){
		std::string remain;
		do {
			*file >> remain;
			strToken += " " + remain;
		}while (remain.at(remain.length()-1) != '\"');
	}

	return RemoveQuote(strToken);
}

int MD5Model::GetIntToken( std::fstream* file )
{
	std::string param = GetNextToken( file );
	if( param == "") return 0;
	return atoi(param.c_str());
}

float MD5Model::GetFloatToken( std::fstream* file )
{
	std::string param = GetNextToken( file );
	if( param == "") return 0.0f;
	return atof(param.c_str());
}

std::string 
MD5Model::RemoveQuote( std::string source )
{
	if( source.at(0) != '\"' && 
		source.at(source.length()-1) != '\"' )
		return source;
	return source.substr(1, source.length()-2);
}

int MD5Model::ParseJoints( std::fstream* file)
{
	// Get "{"
	std::string param = GetNextToken(file);

	while (!file->eof() &&
		(param = GetNextToken(file)) != "}") {
		md5Joint joint;

		// Store the joint's name
		joint.name = param;

		// Get parent Index
		joint.parentIdx = GetIntToken(file);

		// Get Position
		// interchange the Y and the Z axis
		joint.pos.x = GetFloatToken(file);
		joint.pos.y = GetFloatToken(file);
		joint.pos.z = GetFloatToken(file);

		// Get orientation
		joint.orientation.x = GetFloatToken(file);
		joint.orientation.y = GetFloatToken(file);
		joint.orientation.z = GetFloatToken(file);
		
		// Compute the w axis of the quaternion (The MD5 model uses a 3D vector to describe the
		// direction the bone is facing. However, we need to turn this into a quaternion, and the way
		// quaternions work, is the xyz values describe the axis of rotation, while the w is a value
		// between 0 and 1 which describes the angle of rotation)
		float t = 1.0f - ( joint.orientation.x * joint.orientation.x )
				- ( joint.orientation.y * joint.orientation.y )
				- ( joint.orientation.z * joint.orientation.z );
		if ( t < 0.0f )
		{
			joint.orientation.w = 0.0f;
		}
		else
		{
			joint.orientation.w = -sqrtf(t);
		}

		// add joints to list
		m_jointList.push_back(joint);
	}
	return 0;
}

int MD5Model::ParseMesh( std::fstream* file )
{
	MD5Mesh mesh;

	// Get "{"
	std::string param = GetNextToken(file);

	while (!file->eof() &&
		(param = GetNextToken(file)) != "}") {
		if( param == "shader" ) {
			// read texture file
			param = GetNextToken(file);
			mesh.textureList.push_back(param);

		} else if (param == "numverts") {
			param = GetNextToken(file);
			int num_verts = atoi(param.c_str());
			mesh.numVerts = num_verts;

			for(int i = 0; i < num_verts; i++) {

				MD5Vertex vertex;

				// read the "vert" string
				param = GetNextToken(file);

				//read vertex index
				vertex.vertIdx = GetIntToken(file);

				//read tex Coords
				vertex.texCoords.x = GetFloatToken(file);
				vertex.texCoords.y = GetFloatToken(file);

				// read startweight and weight count
				vertex.startWeight = GetIntToken(file);
				vertex.weightCount = GetIntToken(file);

				mesh.vertices.push_back(vertex);

			}
		} else if (param == "numtris") { 
			param = GetNextToken(file);
			int num_tri = atoi(param.c_str());
			mesh.numTriangles = num_tri;

			for(int i = 0; i < num_tri; i++) {
				MD5Triangle triangle;

			    // read the "tri" string
				param = GetNextToken(file);
				
				// read the triangle index
				triangle.triIdx = GetIntToken(file);

				//read vert coord 0, 1, 2
				triangle.vertIdx0 = GetIntToken(file);
				triangle.vertIdx1 = GetIntToken(file);
				triangle.vertIdx2 = GetIntToken(file);

				mesh.triangles.push_back(triangle);
			}

		} else if (param == "numweights") {
			param = GetNextToken(file);
			int num_weights = atoi(param.c_str());
			mesh.numWeights = num_weights;

			for(int i = 0; i < num_weights; i++) {
				
				MD5Weight weight;

				// read the "weight" string
				param = GetNextToken(file);
				
				// read the weight index
				weight.weightIdx = GetIntToken(file);

				// read the joint index
				weight.jointIdx = GetIntToken(file);

				// read the bias
				weight.bias = GetFloatToken(file);

				// read weight position, interchange Y and Z
				weight.weightPos.x = GetFloatToken(file);
				weight.weightPos.y = GetFloatToken(file);
				weight.weightPos.z = GetFloatToken(file);

				mesh.weights.push_back(weight);
			}
		
		}
	}
	
	// add to mesh list
	m_meshList.push_back( mesh );

	return 0;
}

bool MD5Model::LoadFromFile(const char* szPath, const char * szDirectory)
{
	fstream file;
	std::string param = "";

	// Store the Path variable
	m_strPathDir = std::string( szDirectory );
	std::string path = m_strPathDir + "/" + std::string(szPath);
	
	file.open( path.c_str() );
	
	// if file not found return error
	if (!file.is_open()) return false;

	// Read MD5 signature
	std::string md5check = GetNextToken(&file);
	std::string version = GetNextToken(&file);
	if ( md5check != "MD5Version" && version != "10" ) {
		return false;
	}
	
	// read "commandline" and ""
	param = GetNextToken(&file);
	param = GetNextToken(&file);

	// get joints count
	int num_joints = 0;
	param = GetNextToken(&file);	// Read "numJoints"
	param = GetNextToken(&file);	// the the joints count
	num_joints = atoi(param.c_str());

	// get mesh count
	int num_mesh = 0;
	param = GetNextToken(&file);	// Read "numMeshes"
	param = GetNextToken(&file);	// the the mesh count
	num_mesh = atoi(param.c_str());

	while (!file.eof()) {
		param = GetNextToken(&file);
		if ( param == "joints") {
			ParseJoints( &file );
		} 
		else if (param == "mesh") {
			ParseMesh ( &file );
		}
	}

	file.close();

	return true;
}

int MD5Model::AddAnimation(const char* szPath)
{
	fstream file;
	file.open(szPath);
	if (!file.is_open()) return -1;
	
	MD5Animation animation;
	std::string param;

	// Read MD5 signature
	std::string md5check = GetNextToken(&file);
	std::string version = GetNextToken(&file);
	if ( md5check != "MD5Version" && version != "10" ) {
		return -1;
	}
	
	// read "commandline" and ""
	param = GetNextToken(&file);
	param = GetNextToken(&file);

	while (!file.eof()) {
		param = GetNextToken(&file);
		if (param == "numFrames") {
			animation.numFrames = GetIntToken(&file);
		} else if (param == "numJoints") {
			animation.numJoints = GetIntToken(&file);
		} else if (param == "frameRate") {
			animation.frameRate = GetIntToken(&file);
		} else if (param == "numAnimatedComponents") {
			animation.numAnimatedComponents = GetIntToken(&file);
		} else if (param == "hierarchy") {
			// escape the "{"
			param = GetNextToken(&file);

			// Read the firs data, joint's name first
			while ( (param = GetNextToken(&file)) != "}") {
				md5Hierarchy hierarchy;

				//Read the "Joint name"
				hierarchy.jointName = param;

				// Read parent Index
				hierarchy.parentIdx	= GetIntToken(&file);

				// Read the flags
				hierarchy.flags = GetIntToken(&file);

				//Read start Index
				hierarchy.startIdx = GetIntToken(&file);

				// Add the hierarchy
				animation.hierarchyList.push_back(hierarchy);
			}

		} else if (param == "bounds") {

			// escape the "{"
			param = GetNextToken(&file);
			while ( (param = GetNextToken(&file)) != "}") {
				md5Bounds bounds;

				// Interchange Y and Z, 
				// most modeler has the Z axis pointing Up/Down
				bounds.boundMin.x = atof(param.c_str());
				bounds.boundMin.y = GetFloatToken(&file);
				bounds.boundMin.z = GetFloatToken(&file);

				bounds.boundMax.x = GetFloatToken(&file);
				bounds.boundMax.y = GetFloatToken(&file);
				bounds.boundMax.z = GetFloatToken(&file);
				
				animation.boundsList.push_back(bounds);
			}

		} else if (param == "baseframe") {
		
			// escape the "{"
			param = GetNextToken(&file);
			while ( (param = GetNextToken(&file)) != "}") {
				md5BaseFrame baseframe;

				//interchange Y and Z
				baseframe.position.x = atof(param.c_str());
				baseframe.position.y = GetFloatToken(&file);
				baseframe.position.z = GetFloatToken(&file);

				baseframe.orientation.x = GetFloatToken(&file);
				baseframe.orientation.y = GetFloatToken(&file);
				baseframe.orientation.z = GetFloatToken(&file);
							
				animation.baseFrameList.push_back(baseframe);
			}

		} else if (param == "frame") {

			MD5Frame frame;
			frame.frameID = GetIntToken(&file);
			
			// escape the "{"
			param = GetNextToken(&file);
			
			while ( (param = GetNextToken(&file)) != "}") {
				float value = atof(param.c_str());
				frame.frameData.push_back(value);
			}

			animation.frameList.push_back(frame);
		}
	}

	file.close();
	m_animationList.push_back(animation);

	return 0;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////
/// Renderer related functions
// Converted to OpenGL, originally written for DirectX 11 -- 11/23/2013
//////////////////////////////////////////////////////////////////////////////////////////////////////

void MD5Model::BuildFrameSkeleton()
{
	for (int a = 0; a < m_animationList.size(); a++) {
		BuildFrameSkeleton( a );
	}
}

void MD5Model::BuildFrameSkeleton( int a )
{
	// Builds/Prepares the skeleton pose per frame, every Update(), the animation
	// is interpolated between these frames
	// 1. Read the original joints information from baseframe
	// 2. Replace the necessary information from the current frame data based on
	//	heirarchy info of the joints.
	// 3. Compute the Quaternion W.
	// 4. If it has a parent, apply the parents position and orientation
	// 5. compute frame time variables

	for (int f = 0; f < m_animationList[a].numFrames; f++) {
			md5FrameSkeleton frameSkeleton;
			md5Frame frame = m_animationList[a].frameList[f];
			std::vector<float> frameData = frame.frameData;

			for(int j = 0; j < m_animationList[a].hierarchyList.size(); j++) {
				md5Hierarchy joints =  m_animationList[a].hierarchyList[j];
				md5Joint tempJoint;

				//Set default joints from baseframe
				tempJoint.pos = m_animationList[a].baseFrameList[j].position;
				tempJoint.orientation = m_animationList[a].baseFrameList[j].orientation;
				int offset = 0;

				if(joints.flags & 1) {
					tempJoint.pos.x = frameData[joints.startIdx+offset];
					++offset;
				}
				if(joints.flags & 2) {
				   tempJoint.pos.y = frameData[joints.startIdx+offset];
					++offset;
				}
				if(joints.flags & 4) {
				  tempJoint.pos.z = frameData[joints.startIdx+offset];
					++offset;
				}

				if(joints.flags & 8) {
					tempJoint.orientation.x = frameData[joints.startIdx+offset];
					++offset;
				}
				if(joints.flags & 16) {
				   tempJoint.orientation.y = frameData[joints.startIdx+offset];
					++offset;
				}
				if(joints.flags & 32) {
				   tempJoint.orientation.z = frameData[joints.startIdx+offset];
					++offset;
				}

				// Compute the quaternions w
				float t = 1.0f - ( tempJoint.orientation.x * tempJoint.orientation.x )
					- ( tempJoint.orientation.y * tempJoint.orientation.y )
					- ( tempJoint.orientation.z * tempJoint.orientation.z );
				if ( t < 0.0f )
				{
					tempJoint.orientation.w = 0.0f;
				}
				else
				{
					tempJoint.orientation.w = -sqrtf(t);
				}

				// Now, if the upper arm of your skeleton moves, you need to also move the lower part of your arm, and then the hands, and then finally the fingers (possibly weapon or tool too)
				// This is where joint hierarchy comes in. We start at the top of the hierarchy, and move down to each joints child, rotating and translating them based on their parents rotation
				// and translation. We can assume that by the time we get to the child, the parent has already been rotated and transformed based of it's parent. We can assume this because
				// the child should never come before the parent in the files we loaded in.
				if(joints.parentIdx >= 0 ){
					//Apply parent position and orientation
					md5Joint parent = frameSkeleton.joints[joints.parentIdx];

					//---
					glm::vec3 rotPos = parent.orientation * tempJoint.pos;
					tempJoint.pos = parent.pos + rotPos;
					tempJoint.orientation = parent.orientation * tempJoint.orientation;
 
					tempJoint.orientation = glm::normalize( tempJoint.orientation );
							//---
				}

				// add the joint
				frameSkeleton.joints.push_back(tempJoint);
			}

			m_animationList[a].frameSkeleton.push_back(frameSkeleton);
			
			// initialize animation time
			// Computes the time of each frame
			// compute total animation time of all frames
			m_animationList[a].currAnimTime = 0;
			m_animationList[a].perFrameTime = 1.0 / (float)m_animationList[a].frameRate;
			m_animationList[a].totalAnimTime = m_animationList[a].perFrameTime * 
				(float)m_animationList[a].numFrames;

		}
}

void MD5Model::BuildBuffers( )
{
	//Build blank Mesh3D data
	for (int i = 0; i < m_meshList.size(); i++) { 
		MeshSubset* mesh = new MeshSubset();
		int num_verts = m_meshList[i].numVerts;

		//TODO: temporary, use POSITION for now
		//mesh->m_pVertices = new D3DVertex[num_verts]; --> Convert to OpenGL
		mesh->m_pVertices	= new glm::vec3[num_verts];
		mesh->vertCount = num_verts;

		// Create a blank buffer
		memset(mesh->m_pVertices, 0, sizeof(glm::vec3) * mesh->vertCount);

		glGenVertexArrays(1, &mesh->vao);
		glBindVertexArray(mesh->vao);

		// Generate Vertex Buffer
		glGenBuffers(1, &mesh->vbo);
		glBindBuffer(GL_ARRAY_BUFFER, mesh->vbo);
		glBufferData(GL_ARRAY_BUFFER, 
			sizeof(glm::vec3) * mesh->vertCount, 
			mesh->m_pVertices, GL_STATIC_DRAW);

		glVertexAttribPointer( 0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);


		//-------------------------------------------------------------------
		// Build Index Buffers
		//-------------------------------------------------------------------
		//Build the index list
		int index_size = m_meshList[i].numTriangles * 3;
		mesh->m_pIndices = new unsigned short[ index_size ];
		mesh->indexCount = index_size;

		//load the indices from MD5 Polygon/triangle to index buffer
		int index = 0;
		for (int k = 0; k < m_meshList[i].numTriangles; k++) {
			mesh->m_pIndices[index++] = m_meshList[i].triangles[k].vertIdx0;
			mesh->m_pIndices[index++] = m_meshList[i].triangles[k].vertIdx1;
			mesh->m_pIndices[index++] = m_meshList[i].triangles[k].vertIdx2;
		}

		// Create Index Buffer
		glGenBuffers(1, &mesh->ibo);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh->ibo);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, 
			sizeof(unsigned short) * index_size, 
			mesh->m_pIndices, GL_STATIC_DRAW);

		/* Populate texture coordinate List */
		glm::vec2* textCoords = new glm::vec2[ mesh->vertCount ];

		int index_uv = 0;
		for(int v=0; v < m_meshList[i].numVerts; v++ ){
			textCoords[index_uv++] = m_meshList[i].vertices[v].texCoords;
		}

		glGenBuffers(1, &mesh->vbo_uv);
		glBindBuffer(GL_ARRAY_BUFFER, mesh->vbo_uv);
		glBufferData(GL_ARRAY_BUFFER, 
			sizeof(glm::vec2) * mesh->vertCount, (void*)&textCoords[0], GL_STATIC_DRAW);
		
		glVertexAttribPointer( 1, 2, GL_FLOAT, GL_FALSE, 0, (void*)0);
		
		// Cleanup
		delete[] textCoords;
		textCoords = 0;

		glBindVertexArray(0);
		glBindBuffer(GL_ARRAY_BUFFER,0);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,0);

		//---------------------------------------------------------------------------------
		// LOAD Texture 
		
		if( m_meshList[i].textureList[0].size() > 0 ) {
	
			std::string strPath = m_meshList[i].textureList.at(0);	
			std::string fname = _GetFileName(strPath.c_str());
	
			mesh->texID = TextureLoader::GetTexture(fname.c_str(), m_strPathDir.c_str(),false);
			if( 0 == mesh->texID  )
			{
				Log::Dbg( "SOIL loading error: '%s' %s \n", SOIL_last_result(), fname.c_str());
			}
		}
		//---------------------------------------------------------------------------------

		//store the mesh
		m_subsets.push_back(mesh);
	}
}

int MD5Model::UpdateVertices(const md5FrameSkeleton& skeleton )
{
	// Updates the vertex information based from the skeleton infromation 
	// (position and orientation) provided
	// 1. Loop thru all vertex
	// 2. loop thru all Vertex's weights. Weight information contains joint ID
	// 3. From JointID get the current bone/joint.
	// 4. apply joint position and orientation to the vertex using quaternion '
	// 5. Multiply the weight bias.	the weight bias is the amount of influence of 
	// the bone/joint to the vertex

	//loop thru all md5 mesh
	for(int i=0; i < m_meshList.size(); i++) {
		for(int v=0; v < m_meshList[i].numVerts; v++) {
			md5Vertex vertex = m_meshList[i].vertices[v];
			glm::vec3 finalPos = glm::vec3(0,0,0);

			///////////////////////////////////////////////////////////////////////////////////////////
			// TEMPORARY!!!
			//D3DVertex* pCurrentVertex = (D3DVertex*)m_subsets[i]->m_pVertices;
			//////////////////////////////////////////////////////////////////////////////////////////
			glm::vec3* pCurrentVertex = (glm::vec3*) m_subsets[i]->m_pVertices;

			for(int w=0; w < vertex.weightCount; w++) {
				md5Weight weight = m_meshList[i].weights[ vertex.startWeight + w];
				md5Joint joint = skeleton.joints[weight.jointIdx];

				//...
				 // Convert the weight position from Joint local space to object space
				glm::vec3 rotPos = joint.orientation * weight.weightPos;
				finalPos += ( joint.pos + rotPos ) * weight.bias;
				//...
				
			}

			// Store position
			//pCurrentVertex[v].pos = finalPos;
			pCurrentVertex[v] = finalPos;
		}
	}

	return 0;
}

void UpdateNormals( )
{
	//...
}

int MD5Model::BakeMesh(  )
{
	if( m_animationList.size() > 0 ) { 
		m_bAnim = true;
	}
	
	//Build the mesh objects, vertex and index buffers
	BuildBuffers(/*device*/);

	if ( m_bAnim ) {
		// Build the skeleton per frame
		BuildFrameSkeleton( );
	}
	else {
	
		//build frame skeleton from bind-pose joint
		md5FrameSkeleton skeleton;
		skeleton.joints = m_jointList;
		UpdateVertices(skeleton);
		BakeVertices( );
	}

	return 0;
}

void MD5Model::BakeVertices( )
{
	for(int m=0; m < m_subsets.size(); m++) {
		MeshSubset* subset =  (MeshSubset*)m_subsets[m];

		glBindBuffer(GL_ARRAY_BUFFER, subset->vbo);
		GLvoid* position = glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY);
		memcpy(position, subset->m_pVertices, subset->vertCount * sizeof(glm::vec3));
		glUnmapBuffer(GL_ARRAY_BUFFER);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
	}
}

void MD5Model::AddAnimationBake( const char* szFname, const char* szDir)
{
	if( AddAnimation( szFname ) == 0 ) {
		BuildFrameSkeleton( m_animationList.size() - 1 );

		m_bAnim = true;
	}
}

void MD5Model::Update( float elapsed )
{
	if (!m_bAnim) return; 
	
	md5Animation* pCurrentAnim = &m_animationList[m_iCurrentAnim];
	pCurrentAnim->currAnimTime += elapsed;

	if(pCurrentAnim->currAnimTime > pCurrentAnim->totalAnimTime) { 
		pCurrentAnim->currAnimTime = 0;
	}

	// compute the current frame based on the current animation time.
	float factor = pCurrentAnim->currAnimTime * pCurrentAnim->frameRate;
	int frame0 = floor(factor);
	int frame1 = ceil(factor);
	frame0 = frame0 % pCurrentAnim->numFrames;
	frame1 = frame1 % pCurrentAnim->numFrames;
		
	float interpolation = factor - frame0;	// Get the remainder (in time) between frame0 and frame1 to use as interpolation factor

	std::vector<md5Joint> interpolatedSkeleton;		// Create a frame skeleton to store the interpolated skeletons in
	for (int j=0; j < pCurrentAnim->hierarchyList.size(); j++) {
		md5Joint tempJoint;
		md5Joint joint0 = pCurrentAnim->frameSkeleton[frame0].joints[j];	// Get the i'th joint of frame0's skeleton
		md5Joint joint1 = pCurrentAnim->frameSkeleton[frame1].joints[j];	// Get the i'th joint of frame1's skeleton

		tempJoint.parentIdx = joint0.parentIdx;								// Set the tempJoints parent id

		// Interpolate positions
		tempJoint.pos.x = joint0.pos.x + (interpolation * (joint1.pos.x - joint0.pos.x));
		tempJoint.pos.y = joint0.pos.y + (interpolation * (joint1.pos.y - joint0.pos.y));
		tempJoint.pos.z = joint0.pos.z + (interpolation * (joint1.pos.z - joint0.pos.z));
		tempJoint.orientation = glm::mix( joint0.orientation, joint1.orientation, interpolation );

		interpolatedSkeleton.push_back(tempJoint);		// Push the joint back into our interpolated skeleton
	}

	// Update vertices based on interpolated skeleton
	md5FrameSkeleton skeleton;
	skeleton.joints = interpolatedSkeleton;
	UpdateVertices(skeleton);

	// Update the buffers
	BakeVertices();
}

void MD5Model::Draw( )
{
	for(int s = 0; s < m_subsets.size(); s++) {
		MeshSubset* subset =(MeshSubset*)m_subsets[s];

		glBindVertexArray(subset->vao);
		glEnableVertexAttribArray(0);    
			
		if( subset->texID  ) {
			glEnableVertexAttribArray(1);

			//Texture Setup
			glActiveTexture(GL_TEXTURE0);
			glBindTexture( GL_TEXTURE_2D, subset->texID );
		}

		glDrawElements(GL_TRIANGLES, subset->indexCount, GL_UNSIGNED_SHORT, 0);
		glDisableVertexAttribArray(0);
		glDisableVertexAttribArray(1);
		glBindVertexArray(0);
		glBindTexture( GL_TEXTURE_2D, 0 );
	}
}
