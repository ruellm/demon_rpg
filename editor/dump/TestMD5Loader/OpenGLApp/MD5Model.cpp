// MD5 Loader 
// Written: Ruell Magpayo
// April 20, 2013 9:00am PST
// @ Westchester Garden Apts, 6680 West 86th Place, Los Angeles, CA 90045
// -- Ported to OpenGL 3.3 November 23, 2013 --

#include "StdAfx.h"
#include "MD5Model.h"
#include <iostream>
#include <fstream>
#include <math.h>	//for floor and ceil

using namespace std;

MD5Model::MD5Model(void)
{
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

int MD5Model::LoadFromFile(char* szPath, char * szDirectory)
{
	fstream file;
	std::string param = "";
	int ret = 0;

	std::string path = std::string(szDirectory) + "/" + 
		std::string(szPath);
	
	file.open( path.c_str() );
	
	// if file not found return error
	if (!file.is_open()) return -1;

	// Read MD5 signature
	std::string md5check = GetNextToken(&file);
	std::string version = GetNextToken(&file);
	if ( md5check != "MD5Version" && version != "10" ) {
		return -1;
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

	// Store the Path variable
	m_strPathDir = std::string( szDirectory );
	
	return ret;
}

int MD5Model::AddAnimation(char* szPath)
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
//////////////////////////////////////////////////////////////////////////////////////////////////////

void MD5Model::BuildFrameSkeleton()
{
	// Builds/Prepares the skeleton pose per frame, every Update(), the animation
	// is interpolated between these frames
	// 1. Read the original joints information from baseframe
	// 2. Replace the necessary information from the current frame data based on
	//	heirarchy info of the joints.
	// 3. Compute the Quaternion W.
	// 4. If it has a parent, apply the parents position and orientation
	// 5. compute frame time variables

	for (int a = 0; a < m_animationList.size(); a++) {
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

#if 0
					/*QuatRotatePosition(&tempJoint.pos, &tempJoint.orientation, 
						parent.pos, parent.orientation);*/

					// Turn the XMFLOAT3 and 4's into vectors for easier computation
					XMVECTOR parentJointOrientation = XMVectorSet(parent.orientation.x, parent.orientation.y, parent.orientation.z, parent.orientation.w);
					XMVECTOR tempJointPos = XMVectorSet(tempJoint.pos.x, tempJoint.pos.y, tempJoint.pos.z, 0.0f);
					XMVECTOR parentOrientationConjugate = XMVectorSet(-parent.orientation.x, -parent.orientation.y, -parent.orientation.z, parent.orientation.w);

					// Calculate current joints position relative to its parents position
					XMFLOAT3 rotatedPos;
					XMStoreFloat3(&rotatedPos, XMQuaternionMultiply(XMQuaternionMultiply(parentJointOrientation, tempJointPos), parentOrientationConjugate));

					// Translate the joint to model space by adding the parent joint's pos to it
					tempJoint.pos.x = rotatedPos.x + parent.pos.x;
					tempJoint.pos.y = rotatedPos.y + parent.pos.y;
					tempJoint.pos.z = rotatedPos.z + parent.pos.z;

					// Currently the joint is oriented in its parent joints space, we now need to orient it in
					// model space by multiplying the two orientations together (parentOrientation * childOrientation) <- In that order
					XMVECTOR tempJointOrient = XMVectorSet(tempJoint.orientation.x, tempJoint.orientation.y, tempJoint.orientation.z, tempJoint.orientation.w);
					tempJointOrient = XMQuaternionMultiply(parentJointOrientation, tempJointOrient);

					// Normalize the orienation quaternion
					tempJointOrient = XMQuaternionNormalize(tempJointOrient);

					XMStoreFloat4(&tempJoint.orientation, tempJointOrient);
#endif

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
}

void MD5Model::BuildBuffers(/*ID3D11Device* device*/)
{
	//Build blank Mesh3D data
	for (int i = 0; i < m_meshList.size(); i++) { 
		MeshSubset* mesh = new MeshSubset();
		int num_verts = m_meshList[i].numVerts;

		//TODO: temporary, use POSITION for now
		//mesh->m_pVertices = new D3DVertex[num_verts]; --> Convert to OpenGL
		mesh->m_pVertices	= new glm::vec3[num_verts];
		mesh->vertCount = num_verts;

		/* -- Convert to OpenGL -- */
#if 0
		// Create the vertex buffer
		// Create a dynamic buffer since we will keep on updating the buffer
		// during animation
		D3D11_BUFFER_DESC vb = {0};
		vb.Usage = D3D11_USAGE_DYNAMIC;
		vb.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		vb.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		
		////////////////////////////////////////////////////////////////////////
		vb.ByteWidth = sizeof(D3DVertex) * num_verts;
		///////////////////////////////////////////////////////////////////////

		HRESULT hr = device->CreateBuffer(&vb, 0, &mesh->vertBuffer);
		if(FAILED(hr)){
			::OutputDebugString(DXGetErrorDescription(hr));
			return;
		}
#endif
		// Generate Vertex Buffer
		glGenBuffers(1, &mesh->vbo);

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
			mesh->m_pIndices[index++] = m_meshList[i].triangles[k].vertIdx1;  //why invert?
			mesh->m_pIndices[index++] = m_meshList[i].triangles[k].vertIdx2;
		}

	/* -- Convert to OpenGL -- */
#if 0
		//Create Index Buffer
		D3D11_BUFFER_DESC indexBuff = {0};
		indexBuff.BindFlags = D3D11_BIND_INDEX_BUFFER;
		indexBuff.Usage = D3D11_USAGE_IMMUTABLE;
		indexBuff.ByteWidth =  sizeof(unsigned short) * index_size;

		//Create initial data
		D3D11_SUBRESOURCE_DATA subres = {0};
		subres.pSysMem = mesh->m_pIndices;
		hr = device->CreateBuffer(&indexBuff,&subres,&mesh->indexBuffer);
		if(FAILED(hr)) {
			::OutputDebugString(DXGetErrorDescription(hr));
			return;
		}
#endif 
		// Create Index Buffer
		glGenBuffers(1, &mesh->ibo);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh->ibo);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, 
			sizeof(unsigned short) * index_size, 
			mesh->m_pIndices, GL_STATIC_DRAW);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

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
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		
		// Cleanup
		delete[] textCoords;
		textCoords = 0;

		//---------------------------------------------------------------------------------
		// LOAD Texture 
		
		//----------------------------------------
		// ZBU_UNSURE : path computation
		
		if( m_meshList[i].textureList[0].size() > 0 ) {
			size_t iLastSeparator = 0;

			std::string strPath = m_meshList[i].textureList.at(0);	
			std::string filename;

			//ZBU_UNSURE : Path directory "\\" to "/"
			  size_t pos = strPath.find_last_of("\\");
			
			  // ZBU_TODO : make a common function for getting the filename out of a path
			//Extract file name. some model files contains non standard directories.
			  if(pos != std::string::npos)
				filename.assign(strPath.begin() + pos + 1, strPath.end());
			  else
				filename = strPath;

			filename = m_strPathDir + "/" + filename;
			//----------------------------------------
			// ZBU_TODO : Assign this to Texture Loader Manager
			mesh->texID = SOIL_load_OGL_texture
					(
						filename.c_str(),
						SOIL_LOAD_AUTO,
						SOIL_CREATE_NEW_ID,
						SOIL_FLAG_MIPMAPS | /*SOIL_FLAG_INVERT_Y |*/ SOIL_FLAG_NTSC_SAFE_RGB
					);

			//--> WHen to use SOIL invert?
		
			//--------------------------------
			if( 0 == mesh->texID )
			{
				char msg[200];
				sprintf(msg, "SOIL loading error: '%s'\n %s \n", SOIL_last_result(), filename.c_str());
				::OutputDebugStringA(msg);
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

				/////////////////////////////////////////////////////////////////////////////////////
				// Apply Quaternion	 Rotation.
				// TODO: Study Quaternion Later
#if 0
				// Convert joint orientation and weight pos to vectors for easier computation
				// When converting a 3d vector to a quaternion, you should put 0 for "w", and
				// When converting a quaternion to a 3d vector, you can just ignore the "w"
				XMVECTOR tempJointOrientation = XMVectorSet(joint.orientation.x, joint.orientation.y, 
					joint.orientation.z, joint.orientation.w);

				XMVECTOR tempWeightPos = XMVectorSet(weight.weightPos.x, weight.weightPos.y, weight.weightPos.z, 0.0f);

				// We will need to use the conjugate of the joint orientation quaternion
				// To get the conjugate of a quaternion, all you have to do is inverse the x, y, and z
				XMVECTOR tempJointOrientationConjugate = XMVectorSet(-joint.orientation.x, -joint.orientation.y, 
					-joint.orientation.z, joint.orientation.w);

				// Calculate vertex position (in joint space, eg. rotate the point around (0,0,0)) for this weight using the joint orientation quaternion and its conjugate
				// We can rotate a point using a quaternion with the equation "rotatedPoint = quaternion * point * quaternionConjugate"
				XMFLOAT3 rotatedPoint;
				XMStoreFloat3(&rotatedPoint, XMQuaternionMultiply(XMQuaternionMultiply(tempJointOrientation, tempWeightPos), 
					tempJointOrientationConjugate));

				// Now move the verices position from joint space (0,0,0) to the joints position in world space, taking the weights bias into account
				// The weight bias is used because multiple weights might have an effect on the vertices final position. Each weight is attached to one joint.
				finalPos.x += ( joint.pos.x + rotatedPoint.x ) * weight.bias;
				finalPos.y += ( joint.pos.y + rotatedPoint.y ) * weight.bias;
				finalPos.z += ( joint.pos.z + rotatedPoint.z ) * weight.bias;

				// Basically what has happened above, is we have taken the weights position relative to the joints position
				// we then rotate the weights position (so that the weight is actually being rotated around (0, 0, 0) in world space) using
				// the quaternion describing the joints rotation. We have stored this rotated point in rotatedPoint, which we then add to
				// the joints position (because we rotated the weight's position around (0,0,0) in world space, and now need to translate it
				// so that it appears to have been rotated around the joints position). Finally we multiply the answer with the weights bias,
				// or how much control the weight has over the final vertices position. All weight's bias effecting a single vertex's position
				// must add up to 1.
				/////////////////////////////////////////////////////////////////////////////////////


#endif

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

int MD5Model::BakeMesh( /*ID3D11Device* device*/)
{
	if( m_animationList.size() > 0 ) { 
		m_bAnim = true;
	}
	
	//Build the mesh objects, vertex and index buffers
	BuildBuffers(/*device*/);

	if ( m_bAnim ) {
		// Build the skeleton per frame
		BuildFrameSkeleton( );
#if 0
		// Just for testing Start at 0 Frame
		UpdateVertices(m_animationList[0].frameSkeleton[0]);
#endif 
	}
	else {
	
		//build frame skeleton from bind-pose joint
		md5FrameSkeleton skeleton;
		skeleton.joints = m_jointList;
		UpdateVertices(skeleton);
		
	}

	return 0;
}

void MD5Model::Update(float elapsed/*,
	ID3D11DeviceContext* context*/)
{
	if (m_bAnim) {
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

#if 0
			// Turn the two quaternions into XMVECTORs for easy computations
			XMVECTOR joint0Orient = XMVectorSet(joint0.orientation.x, joint0.orientation.y, joint0.orientation.z, joint0.orientation.w);
			XMVECTOR joint1Orient = XMVectorSet(joint1.orientation.x, joint1.orientation.y, joint1.orientation.z, joint1.orientation.w);

			// Interpolate positions
			tempJoint.pos.x = joint0.pos.x + (interpolation * (joint1.pos.x - joint0.pos.x));
			tempJoint.pos.y = joint0.pos.y + (interpolation * (joint1.pos.y - joint0.pos.y));
			tempJoint.pos.z = joint0.pos.z + (interpolation * (joint1.pos.z - joint0.pos.z));

			// Interpolate orientations using spherical interpolation (Slerp)
			XMStoreFloat4(&tempJoint.orientation, XMQuaternionSlerp(joint0Orient, joint1Orient, interpolation));
#endif
			// From http://3dgep.com/?p=1053
			// InterpolateSkeletons()
			tempJoint.parentIdx = joint0.parentIdx;
 
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
	}

	for(int m=0; m < m_subsets.size(); m++) {
		MeshSubset* subset =  (MeshSubset*)m_subsets[m];
	
		// Convert to OpenGL
#if 0
		D3D11_MAPPED_SUBRESOURCE subresource;
		HRESULT hr = context->Map(subset->vertBuffer,0, D3D11_MAP_WRITE_DISCARD, 0, &subresource);
		memcpy(subresource.pData, subset->m_pVertices, subset->vertCount * sizeof(D3DVertex));			//TEMPORARY
		context->Unmap(subset->vertBuffer, 0);
#endif

		// upload vertex data to GPU
		glBindBuffer(GL_ARRAY_BUFFER, subset->vbo);
		glBufferData(GL_ARRAY_BUFFER, 
			sizeof(glm::vec3) * subset->vertCount, subset->m_pVertices, GL_STATIC_DRAW);
		GLenum error = glGetError();
		glBindBuffer(GL_ARRAY_BUFFER, 0);
	}
}

void MD5Model::Draw( /*ID3D11DeviceContext* context*/ )
{
	for(int s = 0; s < m_subsets.size(); s++) {
		MeshSubset* subset =(MeshSubset*)m_subsets[s];

		// Convert to OpenGL
#if 0
		//set the vertext buffer
		UINT stride = sizeof( D3DVertex );						//TEMPORARY
		UINT offset = 0;
		context->IASetVertexBuffers(0,1, &subset->vertBuffer, &stride, &offset);
		context->IASetIndexBuffer(subset->indexBuffer, DXGI_FORMAT_R16_UINT, 0);

		context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		context->DrawIndexed(subset->indexCount,0,0);
#endif

		glEnableVertexAttribArray(0);    
		
		glBindBuffer(GL_ARRAY_BUFFER, subset->vbo);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, subset->ibo);

		glVertexAttribPointer(
			   0, 3,                   
			   GL_FLOAT,           
			   GL_FALSE,           
			   0, (void*)0            
			);
		
		if( subset->texID ) {
			//if texture exist	
			glEnableVertexAttribArray(1);
			glActiveTexture(GL_TEXTURE0);     
			glBindTexture( GL_TEXTURE_2D, subset->texID );
			glBindBuffer(GL_ARRAY_BUFFER, subset->vbo_uv);
			glVertexAttribPointer(
				   1, 2,                   
				   GL_FLOAT,           
				   GL_FALSE,           
				   0, (void*)0            
				);
		}

		//glDrawArrays(GL_TRIANGLES, 0, subset->indexCount/3); 
		glDrawElements(GL_TRIANGLES, subset->indexCount, GL_UNSIGNED_SHORT, 0);

		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
		glDisableVertexAttribArray(0);
		glDisableVertexAttribArray(1);

	}
}
