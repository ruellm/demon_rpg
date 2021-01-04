// OpenGLWnd.cpp : implementation file
//

#include "stdafx.h"
#include "OpenGLWnd.h"


/////////////////////////////////////////
// FOR TESTING!!
#include "Model3D.h"
#include "OBJModel.h"
#include "MD5Model.h"

//OBJModel model;
bool model_loaded = false;
float g_angleX = 0.0f;
float g_angleY = 0.0f;
float g_angleZ = 0.0f;
GLuint model_world_object = 0;
glm::mat4 m_modelMatx = glm::mat4(1.0f);

GLuint sampler;

Model3D* g_model = NULL;

////////////////////////////////////////
// OpenGLWnd

OpenGLWnd::OpenGLWnd()	 
	: m_mouseLBtnDown(false), m_hdc(NULL), m_elapsed(0.0f)
{
	m_iWidth = 200;
	m_iHeight =200;
	m_iViewPortWidth =200;
	m_iViewPortHeight=200;

}

OpenGLWnd::~OpenGLWnd()
{
	DestroyScene();
}

BEGIN_MESSAGE_MAP(OpenGLWnd, CWnd)
	ON_WM_SIZE()
	ON_WM_TIMER()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_MOUSEMOVE()
	ON_WM_MOUSEWHEEL()
	ON_MESSAGE(WM_MOUSEHOVER, OnMouseHover)
	ON_MESSAGE(WM_MOUSELEAVE, OnMouseLeave)
END_MESSAGE_MAP()


// OpenGLWnd message handlers

bool OpenGLWnd::CreateGLContext(CRect rect, CWnd *parent)
{
	CString className = AfxRegisterWndClass(CS_HREDRAW | CS_VREDRAW | CS_OWNDC, NULL, (HBRUSH)GetStockObject(WHITE_BRUSH), NULL); //default background colour
	CreateEx(0, className, _T("OpenGL with MFC/CDialog"), WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN, rect, parent, 0);

	SetViewPortSize(rect.Width(), rect.Height());

	m_WindowOld = rect;
	m_WindowOrig = rect;
	if(!InitContext() )
	{
		MessageBox(_T("ERROR Creating InitContext"));
		return false;
	};
	return true;
}


bool OpenGLWnd::InitContext()
{
        PIXELFORMATDESCRIPTOR pfd;
        memset(&pfd, 0, sizeof(PIXELFORMATDESCRIPTOR));
        pfd.nSize  = sizeof(PIXELFORMATDESCRIPTOR);
        pfd.nVersion   = 1;
        pfd.dwFlags    = PFD_DOUBLEBUFFER | PFD_SUPPORT_OPENGL | PFD_DRAW_TO_WINDOW;
        pfd.iPixelType = PFD_TYPE_RGBA;
        pfd.cColorBits = 32;
        pfd.cDepthBits = 32;
        pfd.iLayerType = PFD_MAIN_PLANE;
		
		m_hdc = GetDC()->m_hDC;

        int nPixelFormat = ChoosePixelFormat(m_hdc, &pfd); 
        if (nPixelFormat == 0) return false;
        BOOL bResult = SetPixelFormat (m_hdc, nPixelFormat, &pfd);
        if (!bResult) return false; 
 
        HGLRC tempContext = wglCreateContext(m_hdc);
		wglMakeCurrent( m_hdc, tempContext);
 	
		GLenum GlewInitResult;
		glewExperimental = GL_TRUE;
		GlewInitResult = glewInit();
		
        if (GlewInitResult != GLEW_OK)
        {
                AfxMessageBox(_T("GLEW is not initialized!"));
        }
 
        int attribs[] =
        {
                WGL_CONTEXT_MAJOR_VERSION_ARB, 3,
                WGL_CONTEXT_MINOR_VERSION_ARB, 3,
                WGL_CONTEXT_FLAGS_ARB, 0,
                0
        };
 
        if(wglewIsSupported("WGL_ARB_create_context") == 1)
        {
                m_hrc = wglCreateContextAttribsARB(m_hdc,0, attribs);
                wglMakeCurrent(NULL,NULL);
                wglDeleteContext(tempContext);
                wglMakeCurrent(m_hdc, m_hrc);
        }
        else
        {       //It's not possible to make a GL 3.x context. Use the old style context (GL 2.1 and before)
                m_hrc = tempContext;
        }
 
        if (!m_hrc)
			return false;

		CString str;
		str.Format(_T("OpenGL version: %s\n"),(CString)glGetString(GL_VERSION));
		Log::Dbg( str );

		return true;
}

void OpenGLWnd::PrepareScene()
{
	wglMakeCurrent(m_hdc, m_hrc);
	glClearColor(0.22, 0.22, 0.22, 0.0);

	//do other preparations here

	//create perspective projection matrix
	float aspect_ratio =(float)GetViewPortWidth()/ (float)GetViewPortHeight();

	// Projection matrix : 45° Field of View, 4:3 ratio, display range : 0.1 unit <-> 100 units
	m_projMatrix = glm::perspective(45.0f, aspect_ratio, 0.1f, 1000.0f);

	// Load the common global shader
	m_globalFx.Init();
	m_globalFx.LoadVertexShaderFromFile("shaders/global_vshader.txt");
	m_globalFx.LoadFragmentShaderFromFile("shaders/global_fshader.txt");
	m_globalFx.Build();

	// Shader for the Grid
	m_gridShader.Init();
	m_gridShader.LoadVertexShaderFromFile("shaders/grid_vshader.txt");
	m_gridShader.LoadFragmentShaderFromFile("shaders/grid_fshader.txt");
	m_gridShader.Build();

	// use program/effects before getting uniforms
	m_globalFx.Use();

	// Get uniform variables
	// NOTE: THe right way how to use GLShader class is to inherit and
	// manage uniform inside the class
	m_WVPObject = m_globalFx.GetUniformLocation("gVP");
	if(  0xFFFFFFFF == m_WVPObject) {
		Log::Dbg("Error getting gVP object");
	}/**/
	
	sampler = m_globalFx.GetUniformLocation("gSampler");
	if(  0xFFFFFFFF == sampler) {
		Log::Dbg("Error getting gSampler object");
	}/**/
	
	model_world_object = m_globalFx.GetUniformLocation("gWorld");
	if(  0xFFFFFFFF == model_world_object) {
		Log::Dbg("Error getting gWorld object");
	}

	m_gridShader.Use();
	m_GridObject = m_gridShader.GetUniformLocation("gVP");
	if(  0xFFFFFFFF == m_GridObject) {
		Log::Dbg("Error Grid VP object");
	}

	glm::mat4 matrix = glm::mat4(1.0f);
	m_globalFx.SetUniformMatrix(m_WVPObject, matrix);
	GLenum error = glGetError();

	//setup camera
	m_camera.LookAt( glm::vec3(0,5,5),
		glm::vec3(0,1,0),
		glm::vec3(0,0,-1));

	// Prepare the timer
	m_lastTime = timeGetTime();

	GenerateGrid(15,8);

	wglMakeCurrent(NULL, NULL);
}

void OpenGLWnd::Reshape(UINT nType, int w, int h)
{
	OnSize(nType, w, h);
}

void OpenGLWnd::OnSize(UINT nType, int cx, int cy)
{
	if(m_hdc != NULL)
	{
		wglMakeCurrent(m_hdc, m_hrc);
		//---------------------------------
		glViewport(0, 0, (GLsizei)cx, (GLsizei)cy); 
		//---------------------------------

		switch (nType)
		{
			case SIZE_RESTORED:
			case SIZE_MAXIMIZED:
				{
					MoveWindow(m_WindowOld.left,m_WindowOld.top,GetViewPortWidth(),GetViewPortHeight(),TRUE);
					break;
				}
			default:
				break;
		}
	}
	wglMakeCurrent(NULL, NULL);
}

void OpenGLWnd::OnTimer(UINT nIDEvent)
{
	switch (nIDEvent)
	{
		case 1:
		{
			// Clear color and depth buffer bits
			//glClear(GL_COLOR_BUFFER_BIT );

			unsigned long currTime = timeGetTime();
			m_elapsed = ((float)currTime - (float)m_lastTime) /1000.0;
			m_lastTime = currTime;

			DetectKeyInput();
			Update();

			// Draw OpenGL scene
			DrawScene();

			// Swap buffers
			//SwapBuffers(m_hdc); //--> we already have a swap buffers in DrawScene
			break;
		}
		default:
			break;
	}
	CWnd::OnTimer(nIDEvent);
}

void OpenGLWnd::Update()
{
	// NOTE: wglMakeCurrent IS IMPORTANT TO MAKE OBJECT in CURRENT THREAD
	wglMakeCurrent(NULL, NULL);
	wglMakeCurrent(m_hdc, m_hrc);

	//Updat Camera
	m_camera.Update();

	// --------------------------------------------
	// TEMPORARY
	glm::mat4 world = glm::mat4(1.0f);

	// Compute model world matrix
	m_modelMatx = glm::mat4(1.0f);
	glm::mat4 rotateX = glm::rotate( g_angleX, glm::vec3(1.0f, 0.0f, 0.0f) );
	glm::mat4 rotateY = glm::rotate( g_angleY, glm::vec3(0.0f, 1.0f, 0.0f) );
	glm::mat4 rotateZ = glm::rotate( g_angleZ, glm::vec3(0.0f, 0.0f, 1.0f) );
	m_modelMatx = rotateZ * rotateY * rotateX ;

	if( g_model != NULL )
		g_model->Update(m_elapsed);

	wglMakeCurrent(NULL, NULL);
}

void OpenGLWnd::DrawScene()
{
 	//////---------------------------------
	wglMakeCurrent(NULL, NULL);

	wglMakeCurrent(m_hdc, m_hrc);
	//--------------------------------
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	//glFrontFace(GL_CW);
	//glCullFace(GL_BACK);
	//glEnable(GL_CULL_FACE);
	glEnable(GL_DEPTH_TEST);	// --- VERY VERY IMPORTANT, enable DEPTH TESTING

	// Use program/effects before setting uniform
	glm::mat4 vp = m_projMatrix * m_camera.GetViewMatrix();
	
	m_globalFx.Use();
	m_globalFx.SetUniformMatrix(m_WVPObject, vp);

	//Set the model matrix with rotation
	 m_globalFx.SetUniformMatrix(model_world_object, m_modelMatx);
	 glUniform1i(sampler, 0); //-- activate sampler--NOT SURE, Make this common!

	 if( g_model != NULL )
		g_model->Draw();

	 m_gridShader.Use();
	 m_gridShader.SetUniformMatrix(m_GridObject,vp);

#if 1	
	glBindVertexArray(grid_vba);
	glEnableVertexAttribArray(0);
	glDrawArrays(GL_LINES, 0, grid_vert_count);
	glDisableVertexAttribArray(0);
	glBindVertexArray(0);
#endif


	//--------------------------------
	glFlush();
	SwapBuffers(m_hdc);
	wglMakeCurrent(NULL, NULL);
}

void OpenGLWnd::DestroyScene()
{
	DestroyWindow();

	glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	glDeleteBuffers(1, &grid_vbo);
	
    glBindVertexArray(0);
    glDeleteVertexArrays(1, &grid_vba);

	wglMakeCurrent(NULL, NULL);
	if(m_hrc)
	{
		wglDeleteContext(m_hrc);
		m_hrc = NULL;
	}

	TextureLoader::Cleanup();
}

void OpenGLWnd::OnLButtonDown(UINT flags, CPoint loc)
{
	m_mouseLBtnDown = true;		
	m_mouseLoc = loc;
}

void OpenGLWnd::OnLButtonUp(UINT flags, CPoint loc)
{
	//...	 
	m_mouseLBtnDown = false;
}

void OpenGLWnd::OnMouseMove(UINT flags, CPoint loc)
{
	// Track mouse movement
	TRACKMOUSEEVENT tme;
    tme.cbSize = sizeof(tme);
    tme.hwndTrack = m_hWnd;
    tme.dwFlags = TME_LEAVE|TME_HOVER;
    tme.dwHoverTime = 1;
    TrackMouseEvent(&tme);

	if( m_mouseLBtnDown ) 
	{
		CPoint result =  loc - m_mouseLoc;
		float angleX = glm::radians( glm::abs((float)result.x) );
		float angleY = glm::radians( glm::abs((float)result.y) );

		//... perform drag effect
		float friction = 2.0f;
		if( result.x > 0 ) {
			m_camera.Yaw( -(angleX * friction) );
		} else {
			m_camera.Yaw(  angleX * friction );	
		}

		m_mouseLoc = loc;

		friction = 5.0f;
		if( result.y > 0 ){
			m_camera.Pitch( -(angleY * friction));
		} else {
			m_camera.Pitch(  angleY * friction );
		}
	}
}

BOOL OpenGLWnd::OnMouseWheel(UINT flags, short zDelta, CPoint pt)
{

	if( zDelta > 0 ) {
		m_camera.Walk( 50 * m_elapsed );
	} else {
		m_camera.Walk( -50 * m_elapsed );
	}

	return TRUE;
}

LRESULT OpenGLWnd::OnMouseHover(WPARAM wparam, LPARAM lparam) {
    CDC* pDC = GetWindowDC();
    CRect rect;
    GetWindowRect(&rect);
    rect.OffsetRect( -rect.left, -rect.top);
    CBrush brush( RGB(0, 0, 255));
    pDC->FrameRect( &rect, &brush);
    ReleaseDC(pDC);
    return true;
}

LRESULT OpenGLWnd::OnMouseLeave(WPARAM wparam, LPARAM lparam) 
{
	m_mouseLBtnDown = false;	
	return FALSE;
}
void OpenGLWnd::DetectKeyInput()
{
	//...
	if( GetKeyState(VK_CONTROL) & 0x8000 )
	{
		if( (GetKeyState(VK_LEFT) & 0x8000) ) {
			m_camera.Strafe(-2 * m_elapsed);
		} else if ( GetKeyState(VK_RIGHT) & 0x8000 ) {
			m_camera.Strafe(2 * m_elapsed);
		} 
	}

	if( model_loaded ) {
		if( GetKeyState('A') & 0x8000 )	{
			 g_angleY --;
		}else if ( GetKeyState('D') & 0x8000 ) {
			g_angleY++;
		}
	   if( GetKeyState('W') & 0x8000 )	{
			 g_angleX --;
		}else if ( GetKeyState('S') & 0x8000 ) {
			g_angleX++;
		}

	    if( GetKeyState('Q') & 0x8000 )	{
			 g_angleZ --;
		}else if ( GetKeyState('E') & 0x8000 ) {
			g_angleZ++;
		}
	}
}

void OpenGLWnd::GenerateGrid(int cellcnt_w, int cellcnt_h)
{
#if 1
	int horiz_cnt = cellcnt_h + 1;
	int vert_cnt = cellcnt_w +1;
	float cellspace = 0.5f;
	float WIDE = cellspace * (float)cellcnt_w;
	float HEIGHT = cellspace * (float)cellcnt_h;

	grid_vert_count = (horiz_cnt * 2) + (vert_cnt * 2);

	glGenVertexArrays(1, &grid_vba);
	glBindVertexArray(grid_vba);

	glGenBuffers(1, &grid_vbo);
	glBindBuffer(GL_ARRAY_BUFFER, grid_vbo);

	//--------------------------------------------
	//Temporary Grid will be built in Y plane instead of Z.
	//Actual will be on Z Axis
	unsigned long size = (horiz_cnt * 6) + (vert_cnt * 6);
	float* buffer = new float[size];

	float xoffset = -WIDE/2.0f;
	float yoffset = WIDE/2.0f;
	int index = 0;

	for(int x=0; x < horiz_cnt; x++) {

		buffer[index++] = xoffset;
		buffer[index++] = 0;		
		buffer[index++] = yoffset;
		
		buffer[index++] = xoffset + WIDE;
		buffer[index++] = 0;
		buffer[index++] = yoffset;
		
		yoffset -= 	cellspace;
	}

	xoffset = -WIDE/2;
	yoffset = WIDE/2;
	for(int x=0; x < vert_cnt; x++) {
		buffer[index++] = xoffset;
		buffer[index++] = 0;
		buffer[index++] = yoffset;

		buffer[index++] = xoffset;
		buffer[index++] = 0;		
		buffer[index++] = yoffset - HEIGHT;
				
		xoffset += 	cellspace;
	}

	// Give our vertices to OpenGL.
	glBufferData(GL_ARRAY_BUFFER, size * sizeof(float), buffer, GL_STATIC_DRAW);
	glVertexAttribPointer(
            0,                  // attribute 0. No particular reason for 0, but must match the layout in the shader.
            3,                  // size
            GL_FLOAT,           // type
            GL_FALSE,           // normalized?
            0,                  // stride
            (void*)0            // array buffer offset
        );


	//make sure to cleanup
	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	
	GLenum error = glGetError();
	delete[] buffer;

#endif

}

void OpenGLWnd::LoadModel(const char* szFname, const char* szPath)
{
	wglMakeCurrent(m_hdc, m_hrc);
		//model.LoadFromFile(szFname, szPath);
		//model_loaded = true;
	if( g_model != NULL ) {
		delete g_model;
		g_model = NULL;
	}

	std::string strfile (szFname);
	if( strfile.find(".obj") != std::string::npos ) {
		g_model = new OBJModel();
	} else if (strfile.find(".md5mesh") != std::string::npos) {
		g_model = new MD5Model();
	}
	if( g_model ) {
		g_model->LoadFromFile( szFname, szPath);
		model_loaded = true;
		if( g_model->GetModelType() == MODEL_MD5 ) {
			((MD5Model*)g_model)->BakeMesh();
		}
	}

	wglMakeCurrent(NULL, NULL);
}

bool OpenGLWnd::AddMD5Animation( const char* szFname )
{
	if( g_model->GetModelType() == MODEL_MD5 ) {
			((MD5Model*)g_model)->AddAnimationBake(szFname,0);
			return true;
	}
	return false;
}

void OpenGLWnd::SetMD5Animation(int index)
{
	if( g_model->GetModelType() == MODEL_MD5 ) {
		((MD5Model*)g_model)->SetAnimationIdx( index );
	}
}

void OpenGLWnd::CloseModel()
{
	wglMakeCurrent(m_hdc, m_hrc);
		//model.Destroy();
		if( g_model != NULL ) {
			delete g_model;
			g_model = NULL;
		}
		model_loaded = false;

		g_angleX = 0.0f;
		g_angleY = 0.0f;
		g_angleZ = 0.0f;

		m_camera.LookAt( glm::vec3(0,5,5),
			glm::vec3(0,1,0),
			glm::vec3(0,0,-1));

	wglMakeCurrent(NULL, NULL);
}
