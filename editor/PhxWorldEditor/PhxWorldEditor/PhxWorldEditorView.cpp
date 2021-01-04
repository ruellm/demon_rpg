
// PhxWorldEditorView.cpp : implementation of the CPhxWorldEditorView class
// Author: Ruell Magpayo <ruellm@yahoo.com>
// Started Date: Dec 2, 2013

#include "stdafx.h"
// SHARED_HANDLERS can be defined in an ATL project implementing preview, thumbnail
// and search filter handlers and allows sharing of document code with that project.
#ifndef SHARED_HANDLERS
#include "PhxWorldEditor.h"
#endif
#include "MainFrm.h"
#include "PhxWorldEditorDoc.h"
#include "PhxWorldEditorView.h"
#include "GenTerrainDlg.h"
#include "PathUtility.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#include "common.h"
#include "GLShaderFx.h"
#include "TerrainShaderFx.h"
#include "ModelShaderFx.h"
#include "Camera.h"
#include "Frustum.h"
#include "CommonLightShader.h"

//
// Global variables used for Rendering
//
HDC g_hDC;
HGLRC g_hrc;
GLuint grid_vba;
GLuint grid_vbo;
int grid_vert_count = 0;
Camera g_camera;

glm::mat4 g_projMatrix;
GLShaderFx g_gridShader;
TerrainShaderFx g_terrainShader;
ModelShaderFx g_modelShader;
CommonLightShader g_lightShader;

GLuint g_gridVP = 0;
GLuint g_WorldObj = 0;
GLuint g_lineColorObj = 0;

float g_elapsed = 1.0f;
unsigned long m_lastTime =  timeGetTime();;
float g_cameraStepRatio = 50.0f;
Frustum g_frustum;

// CPhxWorldEditorView

IMPLEMENT_DYNCREATE(CPhxWorldEditorView, CView)

BEGIN_MESSAGE_MAP(CPhxWorldEditorView, CView)
	// Standard printing commands
	//ON_COMMAND(ID_FILE_PRINT, &CView::OnFilePrint)
	//ON_COMMAND(ID_FILE_PRINT_DIRECT, &CView::OnFilePrint)
	//ON_COMMAND(ID_FILE_PRINT_PREVIEW, &CPhxWorldEditorView::OnFilePrintPreview)
//	ON_WM_CONTEXTMENU()
	//ON_WM_RBUTTONUP()
	ON_WM_CREATE()
	ON_WM_TIMER()
	ON_WM_SIZE()
	ON_WM_ERASEBKGND()
	ON_WM_DESTROY()
	ON_COMMAND(ID_TERRAIN_GE, &CPhxWorldEditorView::OnTerrainGenerate)
	ON_COMMAND(ID_FILE_CLOSE, &CPhxWorldEditorView::OnClose)
	ON_COMMAND(ID_SETTINGS_MOVEMENT, &CPhxWorldEditorView::OnMovementSettings)
	ON_COMMAND(IDD_ENVIRONMENT_DLG, &CPhxWorldEditorView::OnEnvironmentSettings)

	ON_COMMAND(ID_FILE_XAVE, &CPhxWorldEditorView::OnSave)
	ON_COMMAND(ID_FILE_OPEN, &CPhxWorldEditorView::OnOpen)

	ON_WM_MOUSEWHEEL()
	ON_WM_KEYDOWN()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_MOUSEMOVE()
	ON_WM_MOUSEWHEEL()
	ON_MESSAGE(WM_MOUSEHOVER, OnMouseHover)
	ON_MESSAGE(WM_MOUSELEAVE, OnMouseLeave)

	ON_COMMAND(ID_CAMFREE,OnToolBarFreeCam)
	ON_COMMAND(ID_CAMPAN,OnToolBarPanCam)
	ON_COMMAND(ID_CAMEDTHGT,OnToolBarEdtHgtCam)
	ON_COMMAND(ID_CAMPNTTRN,OnToolBarPntTrnCam)
	ON_COMMAND(ID_MMADD,OnToolBarAddObjCam)
	
	ON_UPDATE_COMMAND_UI( ID_CAMFREE, OnFreeCamUpdate)
	ON_UPDATE_COMMAND_UI( ID_CAMPAN, OnPanCamUpdate)
	ON_UPDATE_COMMAND_UI( ID_CAMEDTHGT, OnEdtHgtCamUpdate)
	ON_UPDATE_COMMAND_UI( ID_CAMPNTTRN, OnPntTrnCamUpdate)
	ON_UPDATE_COMMAND_UI(ID_MMADD,OnAddObjCamUpdate)

	ON_COMMAND(ID_SETTINGS_ENVIRONMENT, &CPhxWorldEditorView::OnSettingsEnvironment)
END_MESSAGE_MAP()


// CPhxWorldEditorView construction/destruction

CPhxWorldEditorView::CPhxWorldEditorView() :
	m_width(0), m_height(0), m_bInit(false), 
	m_genTerrainDlg(0), m_pTerrain(0), m_mouseLBtnDown(false),
	m_freeCamFlag(0), m_PanCamFlag(0), m_EdtHgtCamFlag(0), 
	m_PntTrnCamFlag(0), m_mouseMode(MOUSE_MODE_NONE), m_AddObjCamFlag(0),
	m_moveMentSettingsDlg(0), m_bSave(false), m_envSettingsDlg(0)

{
	// TODO: add construction code here

}

CPhxWorldEditorView::~CPhxWorldEditorView()
{
	DestroyObjects();
}

BOOL CPhxWorldEditorView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs
	// For OpenGL rendering
	cs.style |= ( WS_CLIPCHILDREN | 
    WS_CLIPSIBLINGS | CS_OWNDC ); 

	return CView::PreCreateWindow(cs);
}

// CPhxWorldEditorView drawing

void CPhxWorldEditorView::OnDraw(CDC* /*pDC*/)
{
	CPhxWorldEditorDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;

	// TODO: add draw code for native data here
}


// CPhxWorldEditorView printing


void CPhxWorldEditorView::OnFilePrintPreview()
{
#ifndef SHARED_HANDLERS
	AFXPrintPreview(this);
#endif
}

BOOL CPhxWorldEditorView::OnPreparePrinting(CPrintInfo* pInfo)
{
	// default preparation
	return DoPreparePrinting(pInfo);
}

void CPhxWorldEditorView::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: add extra initialization before printing
}

void CPhxWorldEditorView::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: add cleanup after printing
}

void CPhxWorldEditorView::OnRButtonUp(UINT /* nFlags */, CPoint point)
{
	ClientToScreen(&point);
	OnContextMenu(this, point);
}

void CPhxWorldEditorView::OnContextMenu(CWnd* /* pWnd */, CPoint point)
{
#ifndef SHARED_HANDLERS
	theApp.GetContextMenuManager()->ShowPopupMenu(IDR_POPUP_EDIT, point.x, point.y, this, TRUE);
#endif
}


// CPhxWorldEditorView diagnostics

#ifdef _DEBUG
void CPhxWorldEditorView::AssertValid() const
{
	CView::AssertValid();
}

void CPhxWorldEditorView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CPhxWorldEditorDoc* CPhxWorldEditorView::GetDocument() const // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CPhxWorldEditorDoc)));
	return (CPhxWorldEditorDoc*)m_pDocument;
}
#endif //_DEBUG


// CPhxWorldEditorView message handlers


int CPhxWorldEditorView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CView::OnCreate(lpCreateStruct) == -1)
		return -1;

#ifndef DEBUG
	//
	Log::SetLogMode(LOG_MODE_FILE);
	Log::SetLogFile("./PhxLog.txt");
#endif

	SetupOGLContext();
	SetTimer(1.0/60.0f, 1, 0); // 60 FPS

	// create object
	m_genTerrainDlg = new CGenTerrainDlg(this);
	BOOL ret = m_genTerrainDlg->Create(IDD_GEN_TERRAIN, this);

	m_moveMentSettingsDlg = new CMovementSettingsDlg();
	ret = m_moveMentSettingsDlg->Create(IDD_MOVEMENT_DLG, this);

	m_envSettingsDlg = new CEnvSettingsDlg();
	ret = m_envSettingsDlg->Create(IDD_ENVIRONMENT_DLG, this);


	return 0;
}

//---------------------------------------------------------------------------
// !!OpenGL context Setup related!!
bool CPhxWorldEditorView::SetupOGLContext()
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

		g_hDC = ::GetDC( m_hWnd );

        int nPixelFormat = ChoosePixelFormat(g_hDC, &pfd); 
        if (nPixelFormat == 0) return false;
        BOOL bResult = SetPixelFormat (g_hDC, nPixelFormat, &pfd);
        if (!bResult) return false; 
 
        HGLRC tempContext = wglCreateContext(g_hDC);
		wglMakeCurrent( g_hDC, tempContext);
 	
		GLenum GlewInitResult;
		glewExperimental = GL_TRUE;
		GlewInitResult = glewInit();
		
        if (GlewInitResult != GLEW_OK)
        {
                //AfxMessageBox(_T("GLEW is not initialized!"));
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
                g_hrc = wglCreateContextAttribsARB(g_hDC,0, attribs);
                wglMakeCurrent(NULL,NULL);
                wglDeleteContext(tempContext);
                wglMakeCurrent(g_hDC, g_hrc);
        }
        else
        {       //It's not possible to make a GL 3.x context. Use the old style context (GL 2.1 and before)
                g_hrc = tempContext;
        }
 
        if (!g_hrc)
			return false;

		//CString str;
		//str.Format(_T("OpenGL version: %s\n"),(CString)glGetString(GL_VERSION));
		//TRACE(str);
		Log::Dbg("OpenGL version: %s\n", glGetString(GL_VERSION));
		return true;
}

void CPhxWorldEditorView::OnTimer(UINT nIDEvent)
{
	wglMakeCurrent(NULL, NULL);
	wglMakeCurrent(g_hDC, g_hrc);

	unsigned long currTime = timeGetTime();
	g_elapsed = ((float)currTime - (float)m_lastTime) /1000.0;
	m_lastTime = currTime;

	//------------------------
	if( !m_bInit ) {
		Init();
		m_bInit = true;
	}
	//------------------------

	Update(g_elapsed);
	DrawScene();
			
	wglMakeCurrent(NULL, NULL);

	CWnd::OnTimer(nIDEvent);
}

void CPhxWorldEditorView::DrawScene()
{
	//--------------------------------
	glClearColor(0.22, 0.22, 0.22, 0.0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glEnable(GL_DEPTH_TEST);

	// Setup Common Shaders -- Environment light
	DirectionalLight dir = m_envSettingsDlg->GetDirectionalLight();
	AmbientLight amb = m_envSettingsDlg->GetAmbientLight();
	g_lightShader.SetEnvLight( dir, amb );

	// Defer rendering to other function
	DrawTerrain();

	// Draw the 3D objects
	DrawModels();

	// Draw bounding box of selected objects
	DrawSelected();

	//--------------------------------------
	// Draw Grid
	if( !m_pTerrain ) {
		g_gridShader.Use();
		g_gridShader.SetUniformMatrix(g_gridVP, 
			(g_projMatrix * g_camera.GetViewMatrix()));

		glm::mat4 world = glm::mat4(1.0f);
		g_gridShader.SetUniformMatrix(g_WorldObj, world);
		g_gridShader.SetUniformVector3(	g_lineColorObj, glm::vec3(0.8f, 0.8f, 0.8f));

		glBindVertexArray(grid_vba);
		glEnableVertexAttribArray(0);
		glDrawArrays(GL_LINES, 0, grid_vert_count);
		glDisableVertexAttribArray(0);
		glBindVertexArray(0);
	//-------------------------------------
	}
	glFlush();
	SwapBuffers(g_hDC);
}

void CPhxWorldEditorView::OnSize(UINT nType, int cx, int cy) 
{ 
       CView::OnSize(nType, cx, cy); 

       // TODO: Add your message handler code here 
       wglMakeCurrent( g_hDC, g_hrc ); 
       glViewport( 0, 0, cx, cy); 

       wglMakeCurrent( NULL, NULL ); 
       InvalidateRect(NULL); 

	   m_width = cx;
	   m_height = cy;

	   ResizeProjection();
  } 

BOOL CPhxWorldEditorView::OnEraseBkgnd(CDC* pDC) 
  { 
       // TODO: Add your message handler code here and/or call default 

       return 1; 
  } 

void CPhxWorldEditorView::OnDestroy() 
  { 
       CView::OnDestroy(); 

       // TODO: Add your message handler code here 
       wglMakeCurrent( NULL, NULL ); 
       wglDeleteContext( g_hrc ); 
       ::ReleaseDC( m_hWnd, g_hDC ); 

#ifndef DEBUG
	   Log::Close();
#endif
  } 

void CPhxWorldEditorView::OnTerrainGenerate()
{
	m_genTerrainDlg->ShowWindow(SW_SHOW);
}

void CPhxWorldEditorView::ResizeProjection()
{
	RECT rect;
	::GetClientRect(m_hWnd, &rect);
	
	float aspect_ratio =(float)(rect.right- rect.left)/ (float)(rect.bottom-rect.top);
	g_projMatrix = glm::perspective(45.0f, aspect_ratio, 0.1f, 1000.0f);
}

void CPhxWorldEditorView::Init()
{
	GenerateGrid(15,8);

	g_camera.LookAt( glm::vec3(0,5,5),
		glm::vec3(0,1,0),
		glm::vec3(0,0,-1));
	
	LoadShaders();
}

void CPhxWorldEditorView::DestroyObjects()
{
	OnClose();
	SAFE_DELETE(m_genTerrainDlg);
	SAFE_DELETE(m_moveMentSettingsDlg);
	SAFE_DELETE(m_envSettingsDlg);
}

void CPhxWorldEditorView::Update(float elapsed)
{
	g_camera.Update();
	
	for( int i=0; i < m_obj3DList.size(); i++ ) {
		Object3D* obj = m_obj3DList[i];
		obj->Update(elapsed);
	}

	// compute movement ratio
	// movement ratio depends on the "zoom" value of the camera position
	// 50:5 movement
	glm::vec3 position = g_camera.GetPosition();
	const float fix_distance = 
		m_moveMentSettingsDlg->GetCameraStep(); 

	float ratio = position.y / fix_distance;
	if(ratio < 1.0f ) ratio = 1.0f;
	g_cameraStepRatio =  fix_distance * ratio;

	//Update Frustum
	g_frustum.Update( g_projMatrix * g_camera.GetViewMatrix());

	// Update terrain with frustum
	if( m_pTerrain ) {
	//	m_pTerrain->FrustumClip( g_frustum );
	}

	// Check the models against Frustum
	// ZBU_TODO : must be efficiently design on engine, including the relationship between Frustum, box and scene node.
	for( int i=0; i < m_obj3DList.size(); i++ ) {
		Object3D* obj = m_obj3DList[i];
		BoundingBox* bbox = obj->GetBoundingBox();
		if( bbox == NULL ) continue;

		glm::vec4 transformMin = obj->GetWorldMatrix() * glm::vec4(bbox->min,1);
		glm::vec4 transformMax	= obj->GetWorldMatrix() * glm::vec4(bbox->max,1);

		glm::vec3 newMin = glm::vec3 ( transformMin.x, transformMin.y, transformMin.z );
		glm::vec3 newMax = glm::vec3 ( transformMax.x, transformMax.y, transformMax.z );

		bool isVisible = g_frustum.CheckOOBB( newMin, newMax );
		obj->SetVisible(isVisible);
	
	}
}

void CPhxWorldEditorView::GenerateGrid(int cellcnt_w, int cellcnt_h)
{
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
}

void CPhxWorldEditorView::LoadShaders()
{
	// common light shader
	//std::string lightShader = GLShaderFx::LoadShaderString("shaders/light_fshader_common.txt");
	g_lightShader.LoadShaderFromFile("shaders/light_fshader_common.txt");

	//Load terrain shader
	g_terrainShader.fsInclude(&g_lightShader);
	g_terrainShader.Load();

	//model shaders
	g_modelShader.fsInclude(&g_lightShader);
	g_modelShader.Load();

	// Shader for the Grid
	g_gridShader.Init();
	g_gridShader.LoadVertexShaderFromFile("shaders/grid_vshader.txt");
	g_gridShader.LoadFragmentShaderFromFile("shaders/grid_fshader.txt");
	g_gridShader.Build();
	
	// use program/effects before getting uniforms
	g_gridShader.Use();

	// Get uniform variables
	// NOTE: THe right way how to use GLShader class is to inherit and
	// manage uniform inside the class
	g_gridVP = g_gridShader.GetUniformLocation("gVP");
	if(  0xFFFFFFFF == g_gridVP) {
		Log::Dbg("Error getting gVP object for Grid Shaders");
	}/**/

	g_WorldObj = g_gridShader.GetUniformLocation("g_World");
	if(  0xFFFFFFFF == g_WorldObj) {
		Log::Dbg("Error getting g_World object for Grid Shaders");
	}/**/

	g_lineColorObj	= g_gridShader.GetUniformLocation("lineColor");
	if(  0xFFFFFFFF == g_WorldObj) {
		Log::Dbg("Error getting lineColor object for Grid Shaders");
	}/**/


}

void CPhxWorldEditorView::GoGenerateTerrain(const TerrainInitParam& param)
{
	wglMakeCurrent(g_hDC, g_hrc);

	SAFE_DELETE( m_pTerrain );
	m_pTerrain = new Terrain();
	m_pTerrain->Create( param );

	g_camera.LookAt( glm::vec3(0,50,50),
		glm::vec3(0,1,0),
		glm::vec3(0,0,-1));

	wglMakeCurrent(NULL, NULL);


}

void CPhxWorldEditorView::DrawTerrain()
{
	if( !m_pTerrain ) return; 
	
	g_terrainShader.Use();
	glm::mat4 vp = g_projMatrix * g_camera.GetViewMatrix();
	g_terrainShader.SetVPMatrix(vp);

	g_terrainShader.ActivateBaseTexture();
	g_terrainShader.ActivateSamplers( m_pTerrain->GetTextureCount() );		// well use active samplers
	g_terrainShader.SetRepeatRate(m_pTerrain->GetRepeatRate());

	m_pTerrain->Draw();
				 
}

void CPhxWorldEditorView::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	//...
	// Standard Keyboard shortcuts
	if( GetKeyState(VK_CONTROL) & 0x8000 ) { 
		if( GetKeyState('O') & 0x8000 ) {
			OnOpen();
		}
		if( GetKeyState('S') & 0x8000 ) {
			OnSave();
		}
		if( GetKeyState('X') & 0x8000 ) {
			OnClose();
		}
	}

	//....
	if( m_mouseMode == MOUSE_MODE_NONE && m_selectedList.size() > 0) {
		float movementRate = m_moveMentSettingsDlg->GetObjectStep();
		float value = 0.0f;	  
		bool moved = true;
		glm::vec3 dir = glm::vec3(0, 0, 0);

		if( GetKeyState(VK_CONTROL) & 0x8000 ) { 
				if( GetKeyState(VK_UP) & 0x8000) {
					dir = g_camera.GetLook();
					value = movementRate;
				} else if (GetKeyState(VK_DOWN) & 0x8000)  {
					dir = g_camera.GetLook();
					value = -movementRate;
				} else if (GetKeyState(VK_RIGHT) & 0x8000)  {
					dir = g_camera.GetRight();
					value = movementRate;
				} else if (GetKeyState(VK_LEFT) & 0x8000)  {
					dir = g_camera.GetRight();
					value = -movementRate;
				} else {
					moved = false;
				} 

				if( moved ) {
					dir = glm::vec3(dir.x, 0, dir.z);
				}
			
		} else if( GetKeyState(VK_SHIFT) & 0x8000 ) {
			dir = glm::vec3(0, 1, 0);
			if( GetKeyState(VK_UP) & 0x8000) {
				value = movementRate;
			} else if (GetKeyState(VK_DOWN) & 0x8000)  {
				value = -movementRate;
			} else {
				moved = false;
			}
		}

		if( moved ) {
			for( int i = 0; i <  m_selectedList.size() ; i++ ) {
				m_selectedList[i]->Move( dir, value );
			}
		}
	}

	if( m_mouseMode == MOUSE_MODE_FREE ) {
		
		float step = g_cameraStepRatio;

		if( GetKeyState(VK_UP) & 0x8000) {
			g_camera.Walk( step * g_elapsed );
		} else if (GetKeyState(VK_DOWN) & 0x8000)  {
			g_camera.Walk( -step * g_elapsed );
		} else if (GetKeyState(VK_RIGHT) & 0x8000)  {
			g_camera.Strafe( step * g_elapsed );
		} else if (GetKeyState(VK_LEFT) & 0x8000)  {
			g_camera.Strafe( -step * g_elapsed );
		}
	}

	// Single Key Events
	if( m_selectedList.size() > 0) {
		if( GetKeyState(VK_DELETE) & 0x8000 ) {
			for( int i = 0; i <  m_selectedList.size() ; i++ ) {
				
				for( int s=0; s< m_obj3DList.size(); s ) {
					if( m_obj3DList[s] == m_selectedList[i]) {
						m_obj3DList.erase(m_obj3DList.begin()+s);
						break;
					}else {
						s++;
					}
				}
			}
			m_selectedList.clear();
		}
		else if (GetKeyState(VK_SHIFT) & 0x8000) {
				float objRate = m_moveMentSettingsDlg->GetObjectRotate();
				float objScale = m_moveMentSettingsDlg->GetObjectScale();

			// -- Rotate control --
				if( GetKeyState('D') & 0x8000 ) {
					for( int i = 0; i <  m_selectedList.size() ; i++ ) {
						m_selectedList[i]->RotateY( objRate );
					}
				} else if( GetKeyState('A') & 0x8000 ) {
					for( int i = 0; i <  m_selectedList.size() ; i++ ) {
						m_selectedList[i]->RotateY( -objRate );
					}
				}

				if( GetKeyState('W') & 0x8000 ) {
					for( int i = 0; i <  m_selectedList.size() ; i++ ) {
						m_selectedList[i]->RotateX( objRate );
					}
				} else if( GetKeyState('S') & 0x8000 ) {
					for( int i = 0; i <  m_selectedList.size() ; i++ ) {
						m_selectedList[i]->RotateX( -objRate );
					}
				}

				if( GetKeyState('Q') & 0x8000 ) {
					for( int i = 0; i <  m_selectedList.size() ; i++ ) {
						m_selectedList[i]->RotateZ( objRate );
					}
				} else if( GetKeyState('E') & 0x8000 ) {
					for( int i = 0; i <  m_selectedList.size() ; i++ ) {
						m_selectedList[i]->RotateZ( -objRate );
					}
				}

				// Scale support -- Added Feb 18, 2014
				if( GetKeyState('J') & 0x8000 ) {
				   for( int i = 0; i <  m_selectedList.size() ; i++ ) {
						m_selectedList[i]->ScaleX( objScale );
					}
				} else if (GetKeyState('L') & 0x8000) {
					 for( int i = 0; i <  m_selectedList.size() ; i++ ) {
						m_selectedList[i]->ScaleX( -objScale );
					}
				}

				if( GetKeyState('I') & 0x8000 ) {
					for( int i = 0; i <  m_selectedList.size() ; i++ ) {
						m_selectedList[i]->ScaleY( objScale );
					}
				} else if (GetKeyState('K') & 0x8000) {
					for( int i = 0; i <  m_selectedList.size() ; i++ ) {
						m_selectedList[i]->ScaleY( -objScale );
					}
				}

				if( GetKeyState('U') & 0x8000 ) {
					for( int i = 0; i <  m_selectedList.size() ; i++ ) {
						m_selectedList[i]->ScaleZ( objScale );
					}
				} else if (GetKeyState('O') & 0x8000) {
					for( int i = 0; i <  m_selectedList.size() ; i++ ) {
						m_selectedList[i]->ScaleZ( -objScale );
					}
				}

				if( GetKeyState('N') & 0x8000 ) {
					for( int i = 0; i <  m_selectedList.size() ; i++ ) {
						m_selectedList[i]->ScaleX( objScale );
						m_selectedList[i]->ScaleY( objScale );
						m_selectedList[i]->ScaleZ( objScale );

					}
				} else if (GetKeyState('M') & 0x8000) {
					for( int i = 0; i <  m_selectedList.size() ; i++ ) {
						m_selectedList[i]->ScaleX( -objScale );
						m_selectedList[i]->ScaleY( -objScale );
						m_selectedList[i]->ScaleZ( -objScale );
					}
				}
		}
	}
}

void CPhxWorldEditorView::OnLButtonDown(UINT flags, CPoint loc)
{
	
	// clear the list by default.
	if( !(GetKeyState(VK_CONTROL) & 0x8000) ) {
		m_selectedList.clear();
	}

	m_mouseLBtnDown = true;		
	m_mouseLoc = loc;
	if( !m_pTerrain ) return;

	CFrameWnd * pFrame = (CFrameWnd *)(AfxGetApp()->m_pMainWnd);
	CMainFrame* mainFrame = (CMainFrame*)pFrame;

	glm::vec3 start, dir;
	BuildRay(loc.x, loc.y, start, dir);
	
	if( m_pTerrain == NULL ) return;
	if ( m_mouseMode == MOUSE_MODE_EDIT_TERRAIN ) {
			
			HeightEditParam* hp = mainFrame->GetHEditDlg()->GetHeightEditParam();
			m_pTerrain->SetMaxHeight( hp->max_height );

			TerrainHitInfo result;
			bool hit = m_pTerrain->MousePick(start, dir, &result);
		
			if( hit ) {
				 wglMakeCurrent(g_hDC, g_hrc);
				 if( hp->mode == EDIT_MODE_CIRCLE ) {
					 m_pTerrain->ModifyVertex( result, hp->step_size, hp->radius);
				 }else if ( hp->mode == EDIT_MODE_RECTANGLE ) {
					 m_pTerrain->ModifyRect(result, hp->step_size, hp->width, hp->height);
				 }

				 wglMakeCurrent(NULL, NULL);
			}

	} else if (m_mouseMode == MOUSE_MODE_PAINT_TERRAIN ) {
			CTerrainTextureDlg* td = mainFrame->GetTerrainTextureDlg();

			TerrainHitInfo result;
			bool hit = m_pTerrain->MousePick(start, dir, &result);
			if( hit && td->GetActiveTexture() != -1) {
				wglMakeCurrent(g_hDC, g_hrc);

				float alph = td->GetAlpha() / 100.0f;

				m_pTerrain->PaintTexture( result, 
					td->GetActiveTexture(), 
					td->GetPaintRadius(), 
					alph,
					td->IsUnPaint());

				 wglMakeCurrent(NULL, NULL);
			}
	} else if ( m_mouseMode == MOUSE_MODE_ADD_OBJ ){
			//...
		CModel3DDlg* modelg = mainFrame->GetModel3DDlg();
		int index = modelg->GetSelectedModel();
		if( index < 0 ) return;
		
		TerrainHitInfo result;
		bool hit = m_pTerrain->MousePick(start, dir, &result);
		if( !hit ) return;

		wglMakeCurrent(g_hDC, g_hrc);

		//--------------------------------------------------
		// TEMPORARY!!
		// as for now use Hit position as position of object
		Object3D* object = new Object3D;
		object->SetPosition( result.hit_position );
		object->SetModel3D( m_modelList[index] );
		object->GenBoundingVolume();
		//--------------------------------------------------

		wglMakeCurrent(NULL, NULL);
		
		m_obj3DList.push_back( object );
	} else {
		// Default mouse settings is -- Select
		for( int i = 0; i < m_obj3DList.size(); i++) {
			glm::vec3 hitPost = glm::vec3(0,0,0);
			if( m_obj3DList[i]->MousePick( 	start, dir, &hitPost ) ) {
				m_selectedList.push_back( m_obj3DList[i] );
			} 
		}
	}
}

void CPhxWorldEditorView::BuildRay( int x, int y, glm::vec3& start, glm::vec3& dir)
{
	RECT rect;
	::GetClientRect(m_hWnd, &rect);

	int HEIGHT =  rect.bottom - rect.top;
	glm::vec4 viewport = glm::vec4(0,0, rect.right, rect.bottom );

	// why do we need to invert screen Y coordinates?
	 start = glm::unProject(
		 glm::vec3( x, rect.bottom - y, 0.0), 
		 g_camera.GetViewMatrix(),  g_projMatrix, viewport);

	glm::vec3 end = glm::unProject(
		glm::vec3( x, rect.bottom - y, 1.0), 
		 g_camera.GetViewMatrix(),  g_projMatrix, viewport);

	dir = glm::normalize( end -start );
}

void CPhxWorldEditorView::OnLButtonUp(UINT flags, CPoint loc)
{
	//...	 
	m_mouseLBtnDown = false;
}

void CPhxWorldEditorView::OnMouseMove(UINT flags, CPoint loc)
{
	// Track mouse movement
	TRACKMOUSEEVENT tme;
    tme.cbSize = sizeof(tme);
    tme.hwndTrack = m_hWnd;
    tme.dwFlags = TME_LEAVE|TME_HOVER;
    tme.dwHoverTime = 1;
    TrackMouseEvent(&tme);

	CFrameWnd * pFrame = (CFrameWnd *)(AfxGetApp()->m_pMainWnd);
	CMainFrame* mainFrame = (CMainFrame*)pFrame;

	if( m_mouseLBtnDown ) 
	{
		CPoint result =  loc - m_mouseLoc;
		float angleX = glm::radians( glm::abs((float)result.x) );
		float angleY = glm::radians( glm::abs((float)result.y) );

		glm::vec3 start, dir;
		BuildRay(loc.x, loc.y, start, dir);

		if( m_mouseMode == MOUSE_MODE_FREE ) {
			//... perform drag effect
			float friction = m_moveMentSettingsDlg->GetCameraDragFriction();

			if( result.y > 0 ){
				g_camera.Pitch( -(angleY * friction));
			} else {
				g_camera.Pitch(  angleY * friction );
			}

			if( result.x > 0 ) {
				g_camera.Yaw( -(angleX * friction) );
			} else {
				g_camera.Yaw(  angleX * friction );	
			}

		} else if ( m_mouseMode == MOUSE_MODE_PAN ) {
			
			const float pan_step = m_moveMentSettingsDlg->GetPanStep();
			float movement = g_camera.GetPosition().y / pan_step;
			if( movement < 1.0f ) 
				movement = 1.0f;
			
			g_camera.Strafe( -(result.x * movement)* g_elapsed, CAMERA_TYPE_LAND );
			g_camera.Walk( ((result.y* movement) * g_elapsed), CAMERA_TYPE_LAND );
		
		} else if (m_mouseMode == MOUSE_MODE_PAINT_TERRAIN /*|| 
			m_mouseMode == MOUSE_MODE_EDIT_TERRAIN*/) {

			OnLButtonDown(flags, loc);

		}

		// Added Feb 25, 2014 -- Rotate Camera at the center of the terrain
		if( (GetKeyState(VK_CONTROL) & 0x8000) &&
			(GetKeyState(VK_SHIFT) & 0x8000)) {
				g_camera.RotatePosition( glm::vec3(0,1,0), (float)result.x); 

		}



		m_mouseLoc = loc;
	}
}

BOOL CPhxWorldEditorView::OnMouseWheel(UINT flags, short zDelta, CPoint pt)
{
	float step = g_cameraStepRatio;

	if( GetKeyState(VK_CONTROL) & 0x8000 ) {
		if( zDelta > 0 ) {
			g_camera.Fly( step * g_elapsed );
		} else {
			g_camera.Fly( -step * g_elapsed );
		}

	} else if (GetKeyState(VK_SHIFT) & 0x8000) {
		if( zDelta > 0 ) {
			g_camera.Strafe( step * g_elapsed );
		} else {
			g_camera.Strafe( -step * g_elapsed );
		}
	}
	else {
		if( zDelta > 0 ) {
			g_camera.Walk( step * g_elapsed );
		} else {
			g_camera.Walk( -step * g_elapsed );
		}
	}

	return TRUE;
}

LRESULT CPhxWorldEditorView::OnMouseHover(WPARAM wparam, LPARAM lparam) {
    CDC* pDC = GetWindowDC();
    CRect rect;
    GetWindowRect(&rect);
    rect.OffsetRect( -rect.left, -rect.top);
    CBrush brush( RGB(0, 0, 255));
    pDC->FrameRect( &rect, &brush);
    ReleaseDC(pDC);
    return true;
}

LRESULT CPhxWorldEditorView::OnMouseLeave(WPARAM wparam, LPARAM lparam) 
{
	m_mouseLBtnDown = false;	
	return FALSE;
}

void CPhxWorldEditorView::UpdateToggleCam(int from)
{
	if( from == 0 ) {
		m_PanCamFlag = 0;
		m_EdtHgtCamFlag = 0;
		m_PntTrnCamFlag = 0;
		m_AddObjCamFlag = 0;
		m_mouseMode = MOUSE_MODE_FREE;
	}
	if( from == 1 ) {
		m_freeCamFlag = 0;
		m_EdtHgtCamFlag = 0;
		m_PntTrnCamFlag = 0;
		m_AddObjCamFlag = 0;
		m_mouseMode = MOUSE_MODE_PAN;

	}

	if( from == 2 ) {
		m_PanCamFlag = 0;
		m_freeCamFlag = 0;
		m_PntTrnCamFlag = 0;
		m_AddObjCamFlag = 0;
		m_mouseMode = MOUSE_MODE_EDIT_TERRAIN;
	}

	if( from == 3 ) {
		m_PanCamFlag = 0;
		m_EdtHgtCamFlag = 0;
		m_freeCamFlag = 0;
		m_AddObjCamFlag = 0;
		m_mouseMode = MOUSE_MODE_PAINT_TERRAIN;
	}
	if( from == 4 ) {
		m_PanCamFlag = 0;
		m_EdtHgtCamFlag = 0;
		m_freeCamFlag = 0;
		m_PntTrnCamFlag = 0;
		m_mouseMode = MOUSE_MODE_ADD_OBJ;
	}
}

void CPhxWorldEditorView::ResetMouseMode()
{
	if( !m_PanCamFlag && !m_EdtHgtCamFlag &&
		!m_freeCamFlag && !m_PntTrnCamFlag &&
		!m_AddObjCamFlag){
		m_mouseMode = MOUSE_MODE_NONE;
	}
}

void CPhxWorldEditorView::OnToolBarFreeCam()
{
	m_freeCamFlag = !m_freeCamFlag;
	if( m_freeCamFlag )	
		UpdateToggleCam(0);
	else
		ResetMouseMode();
}

void CPhxWorldEditorView::OnToolBarPanCam()
{
	m_PanCamFlag = !m_PanCamFlag;
	if( m_PanCamFlag )
		UpdateToggleCam(1);
	else
		ResetMouseMode();
}

void CPhxWorldEditorView::OnToolBarEdtHgtCam()
{
	m_EdtHgtCamFlag = !m_EdtHgtCamFlag;
	if( m_EdtHgtCamFlag ) {
		UpdateToggleCam(2);

		CFrameWnd * pFrame = (CFrameWnd *)(AfxGetApp()->m_pMainWnd);
		CMainFrame* mainFrame = (CMainFrame*)pFrame;

		mainFrame->OnHeightEditDlg();
	}
	else {
		ResetMouseMode();
	}
}

void CPhxWorldEditorView::OnToolBarPntTrnCam()
{
	m_PntTrnCamFlag = !m_PntTrnCamFlag;
	if( m_PntTrnCamFlag ) {
		UpdateToggleCam(3);

		CFrameWnd * pFrame = (CFrameWnd *)(AfxGetApp()->m_pMainWnd);
		CMainFrame* mainFrame = (CMainFrame*)pFrame;
		mainFrame->OnTextureLayer();
		
	}else {
		ResetMouseMode();
	}
}

void CPhxWorldEditorView::OnToolBarAddObjCam()
{
	m_AddObjCamFlag = !m_AddObjCamFlag;
	if( m_AddObjCamFlag ) {
		UpdateToggleCam(4);	

		CFrameWnd * pFrame = (CFrameWnd *)(AfxGetApp()->m_pMainWnd);
		CMainFrame* mainFrame = (CMainFrame*)pFrame;
		mainFrame->OnModel3DDlg();
	}else {
		ResetMouseMode();
	}
}

void CPhxWorldEditorView::OnFreeCamUpdate(CCmdUI* pCmdUI)
{
	pCmdUI->SetCheck(m_freeCamFlag);
}

void CPhxWorldEditorView::OnPanCamUpdate(CCmdUI* pCmdUI)
{
	pCmdUI->SetCheck(m_PanCamFlag);
}

void CPhxWorldEditorView::OnEdtHgtCamUpdate(CCmdUI* pCmdUI)
{
	pCmdUI->SetCheck(m_EdtHgtCamFlag);
}

void CPhxWorldEditorView::OnPntTrnCamUpdate(CCmdUI* pCmdUI)
{
	pCmdUI->SetCheck(m_PntTrnCamFlag);
}

void CPhxWorldEditorView::OnAddObjCamUpdate(CCmdUI* pCmdUI)
{
	pCmdUI->SetCheck(m_AddObjCamFlag);
}

void CPhxWorldEditorView::AddTexture( const char* szPath )
{
	if( !m_pTerrain ) return;

	wglMakeCurrent(g_hDC, g_hrc);
	m_pTerrain->AddTexture(szPath);
	wglMakeCurrent(NULL, NULL);
}

bool CPhxWorldEditorView::LoadObjModel( const char* szPath )
{
	//...
	OBJModel* model = new OBJModel();
	std::string fullpath(szPath);

	std::string dir = _GetDir( fullpath );
	std::string fname = _GetFileName( szPath );
	
	wglMakeCurrent(g_hDC, g_hrc);
	if( !model->LoadFromFile(fname.c_str(), dir.c_str()) ) {
		return false;
	}
	
	wglMakeCurrent(NULL, NULL);
	
	m_modelList.push_back( model );

	return true;
}

void CPhxWorldEditorView::DeleteObject( int index )
{
	std::vector<Object3D*>::iterator it = m_obj3DList.begin();
	for(int i = 0; i < m_obj3DList.size(); ) {
		if( m_obj3DList[i]->GetModel() == m_modelList[index]){
			m_obj3DList[i]->SetModel3D(0);
			SAFE_DELETE(m_obj3DList[i]);
			m_obj3DList.erase(m_obj3DList.begin() + i);
		}else{
			i++;
		}
	}

	SAFE_DELETE(m_modelList[index]);
	m_modelList.erase(m_modelList.begin()+index);

}

void CPhxWorldEditorView::DrawModels()
{
	//...
	
	for( int i=0; i < m_obj3DList.size(); i++ ) {

		g_modelShader.Use();
		g_modelShader.ActivateSampler();

		glm::mat4 vp = g_projMatrix * g_camera.GetViewMatrix();
		g_modelShader.SetVPMatrix(vp);

		Object3D* obj = m_obj3DList[i];
		glm::mat4 world = obj->GetWorldMatrix();		
		g_modelShader.SetWorldMatrix(world);

		obj->Draw();

	}

}

void CPhxWorldEditorView::DrawSelected()
{
	g_gridShader.Use();
	g_gridShader.SetUniformMatrix(g_gridVP, 
				(g_projMatrix * g_camera.GetViewMatrix()));

	// Red line bounding box
	g_gridShader.SetUniformVector3(	g_lineColorObj, glm::vec3(1.0f, 0.0f, 0.0f));
	
	for( int i=0; i < m_selectedList.size(); i++ ) {
		SceneObject* obj = 	m_selectedList[i];
		if( obj->IsVisible() && obj->GetBoundingBox() ) {
					
			glm::mat4 transform = obj->GetWorldMatrix() * 
				obj->GetBoundingBox()->GetTransformMatrix();
			
			g_gridShader.SetUniformMatrix(g_WorldObj, transform);
			obj->DrawBBox();
		}

	}
}

void CPhxWorldEditorView::OnClose()
{
	SAFE_DELETE(m_pTerrain);

	for(int i = 0; i < m_modelList.size(); i++ ) {
		SAFE_DELETE(m_modelList[i]);
	}

	for(int i = 0; i < m_obj3DList.size(); i++ ) {
		SAFE_DELETE(m_obj3DList[i]);
	}
	
	 // Reset Camera
	 g_camera.LookAt( glm::vec3(0,5,5),
		glm::vec3(0,1,0),
		glm::vec3(0,0,-1));

	 m_bSave = false;

	 m_modelList.clear();
	 m_obj3DList.clear();
	 m_selectedList.clear();

	 TextureLoader::Cleanup();

	 CFrameWnd * pFrame = (CFrameWnd *)(AfxGetApp()->m_pMainWnd);
	CMainFrame* mainFrame = (CMainFrame*)pFrame;
	 mainFrame->SetWindowTextA("Untitled - PhxWorldEditor");

	 if(  mainFrame->GetModel3DDlg()->GetSafeHwnd() ) {
		mainFrame->GetModel3DDlg()->Clear();
	 }
	 if( mainFrame->GetTerrainTextureDlg()->GetSafeHwnd() ) {
		mainFrame->GetTerrainTextureDlg()->Clear();
	 }
	
}

void CPhxWorldEditorView::OnMovementSettings()
{
	//...
	m_moveMentSettingsDlg->InitValues();
	m_moveMentSettingsDlg->ShowWindow(SW_SHOW);
}

void CPhxWorldEditorView::OnEnvironmentSettings()
{
	m_envSettingsDlg->LoadValues();
	m_envSettingsDlg->ShowWindow(SW_SHOW);
}

void CPhxWorldEditorView::OnOpen()
{
	
	// TODO: Add your control notification handler code here
	static TCHAR BASED_CODE szFilter[] = _T("Phoenix Editor Files |*.phx;")
		_T("*| All Files (*.*)|*.*||");

	CFileDialog fOpenDlg(TRUE, "", "", OFN_HIDEREADONLY|OFN_FILEMUSTEXIST, szFilter, this);
  
	fOpenDlg.m_pOFN->lpstrTitle="Load Image Files";
  
	//fOpenDlg.m_pOFN->lpstrInitialDir="c:";
  	CString strFileName("");
	if(fOpenDlg.DoModal()==IDOK)
	{
		strFileName = fOpenDlg.GetPathName();
		OnClose();
	}

	if(strFileName.IsEmpty()) return; 

	char header[50];
	TerrainInitParam param={0};
	int hMapIdx = 0;
	std::vector<std::string> terrainTextureList;
	typedef std::vector<int> HeightList;
	std::vector<HeightList> tempList;

	CFrameWnd * pFrame = (CFrameWnd *)(AfxGetApp()->m_pMainWnd);
	CMainFrame* mainFrame = (CMainFrame*)pFrame;

	//
	// ZBU_TODO : We need an actual WORLD Loader class in the engine
	//---------------------------------------
	// Derive directory structures
	CString filename = PathFindFileName(strFileName);
	CString newDirName = filename + "_resource";
	newDirName.Replace(".","_");

	std::string strSaveDir = std::string(_GetDir(strFileName));
	std::string resource_dir = 	strSaveDir + "\\" + std::string(newDirName);
	std::string tex_resource_dir = 	resource_dir + "\\texture";
	std::string model_resource_dir = 	resource_dir + "\\model";
	//----------------------------------------

	wglMakeCurrent(g_hDC, g_hrc);

	FILE* fp = fopen(strFileName, "r");

	fscanf(fp, "%s", header);
	if(strcmp(header, "PHOENIX") != 0 ) {
		fclose(fp); return;
	}

	//TextureLoader::OpenSession(tex_resource_dir.c_str());

	while( 1 ) {
		 int res = fscanf(fp, "%s", header);
		 if( res == EOF ) break;

		 if( strcmp(header, "terrrain_dimensison") == 0 ) {
			 fscanf(fp,"%d %d %f \n", &param.width, &param.height, &param.tileDist);
			 param.genType = TERRAIN_GENTYPE_HMAP;
			 param.pHmap = new float[ param.width * param.height ];
		 }

		 else if( strcmp(header, "terrain_base_texture") == 0 ) {
			 char filename[100];
			 fscanf(fp,"%s \n",filename);
			 std::string realPath = tex_resource_dir + "\\" + filename;
			 param.strBaseTexture = std::string(realPath);
		 }

		 else if( strcmp(header, "terrain_repeat_rate") == 0 ) {
			 fscanf(fp,"%d \n", &param.repeatRate);
		 }

		 else if( strcmp(header, "terrain_hmap") == 0 ) {
			 float height = 0.0f;
			 fscanf(fp,"%f \n", &height);
			  param.pHmap[hMapIdx++] = height;
		 } 
		 
		 else if( strcmp(header, "terrain_texture") == 0 ) {
			  char filename[100];
			 fscanf(fp,"%s \n",filename);
			 terrainTextureList.push_back(std::string(filename));
			 
			 HeightList list;
			 list.clear();
			 tempList.push_back(list);
		 }
		 else if( strcmp(header, "terrain_alphamap") == 0 ) {
			 int index = 0;
			 int value=0;
		 	 fscanf(fp,"%d %d \n",&index, &value );
			tempList[index].push_back( value );
		 }

		 // Model loading
		 else if( strcmp(header, "mesh_obj") == 0 ) {
			char filename[100];
			fscanf(fp,"%s \n",filename);

			OBJModel* model = new OBJModel();
			model->LoadFromFile( filename, 
				model_resource_dir.c_str());			
			m_modelList.push_back( model );

			mainFrame->GetModel3DDlg()->GetListCtrl()->InsertString(-1, 
				filename);
		 }	

		else if( strcmp(header, "scene_obj") == 0 ) {
			int index = 0;
			glm::vec3 position;
			float angleX=0.0f;
			float angleY=0.0f;
			float angleZ=0.0f;
			float scaleX = 0.0f;
			float scaleY = 0.0f;
			float scaleZ = 0.0f;

			int v = fscanf(fp,"%d %f %f %f %f %f %f %f %f %f \n", 
				&index, 
				&position.x, &position.y, &position.z,
				&angleX, &angleY, &angleZ,
				&scaleX, &scaleY, &scaleZ);

			Object3D* obj = new Object3D();
			obj->SetModel3D(m_modelList[index]);
			obj->SetPosition(position);
			obj->SetAngles(angleX, angleY, angleZ);
			obj->GenBoundingVolume();
			obj->SetScale(scaleX, scaleY, scaleZ);
			m_obj3DList.push_back(obj);
		
		}
	}

	fclose(fp);

	//---------------------------------------
	// Handling of terrain

	m_pTerrain = new Terrain();
	m_pTerrain->Create( param );

	for(int t = 0; t < terrainTextureList.size(); t++ ) {
		std::string realPath = tex_resource_dir + "\\" + terrainTextureList[t];
		m_pTerrain->AddTexture( realPath.c_str() );

		TerrainTexture* texture = m_pTerrain->GetTextureAt( t );
		
		for(int c=0; c < param.width * param.height; c++ ) {
			unsigned char value = 	 tempList[t].at(c);
			texture->pAlphaData[c] =  value;
		}

		m_pTerrain->RefreshTexture(t);

		mainFrame->GetTerrainTextureDlg()->GetListCtrl()->InsertString(-1, 
			terrainTextureList[t].c_str());
	}
	//---------------------------------------

	g_camera.LookAt( glm::vec3(0,50,50),
		glm::vec3(0,1,0),
		glm::vec3(0,0,-1));

	mainFrame->SetWindowTextA(strFileName + " - PhxWorldEditor");

   wglMakeCurrent(NULL, NULL);
	SAFE_DELETE_ARRAY( param.pHmap );
}

void CPhxWorldEditorView::OnSave()
{
	
	if( 0 == m_pTerrain ) return;

	CString strFileName("");
	if( !m_bSave ) {
		// TODO: Add your control notification handler code here
		static TCHAR BASED_CODE szFilter[] = _T("Phoenix Editor Files |*.phx")
			_T("| All Files (*.*)|*.*||");

		CFileDialog fOpenDlg(FALSE, "", "", OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, szFilter, this);
  
		fOpenDlg.m_pOFN->lpstrTitle="Load Image Files";
  
		//fOpenDlg.m_pOFN->lpstrInitialDir="c:";
  
		if(fOpenDlg.DoModal()==IDOK)
		{
			strFileName = fOpenDlg.GetPathName();
		}
	}
	//...
	if( strFileName.IsEmpty() ) return;

	//---------------------------------------
	// Derive directory structures
	CString filename = PathFindFileName(strFileName);
	CString newDirName = filename + "_resource";
	newDirName.Replace(".","_");

	std::string strSaveDir = std::string(_GetDir(strFileName));
	std::string resource_dir = 	strSaveDir + "\\" + std::string(newDirName);
	std::string tex_resource_dir = 	resource_dir + "\\texture";
	std::string model_resource_dir = 	resource_dir + "\\model";
	//---------------------------------------

	//aight now create the directories
	BOOL result = CreateDirectory ( resource_dir.c_str(), NULL );
	result = CreateDirectory ( tex_resource_dir.c_str(), NULL );
	result =  CreateDirectory ( model_resource_dir.c_str(), NULL );
	DWORD error = GetLastError ();

	//TextureLoader::SaveSession( tex_resource_dir.c_str() ); --> Not used na
	FILE* fp = fopen(strFileName, "w");
	
	//Write header
	fprintf(fp,"PHOENIX\n");

	//---------------------------------------
	// Save the terrain
	m_pTerrain->SaveToFile(fp);
	m_pTerrain->SaveTextures(tex_resource_dir.c_str());

	//Lets save the objects
	for(int i =0; i < m_modelList.size(); i++) {
		Model3D* model =   m_modelList[i];

		fprintf(fp,"mesh_obj %s \n", model->GetFileName().c_str());
		model->OnSave( model_resource_dir.c_str() );

		for(int m=0; m < m_obj3DList.size(); m++) {
			Object3D* subj =  m_obj3DList[m];
			if( subj->GetModel() == model ) {
				glm::vec3 position = subj->GetPosition();
				float angleX = 0.0f;
				float angleY = 0.0f;
				float angleZ = 0.0f;
				subj->GetAngles(&angleX, &angleY, &angleZ);
				glm::vec3 scale = subj->GetScale();

				fprintf(fp,"scene_obj %d %f %f %f %f %f %f %f %f %f\n", i, 
					position.x, position.y, position.z,
					angleX, angleY, angleZ,
					scale.x, scale.y, scale.z );
			}
		}
	}

	m_bSave = true;
	//---------------------------------------

	if(fp) {					 
		fclose(fp);
	}

	CFrameWnd * pFrame = (CFrameWnd *)(AfxGetApp()->m_pMainWnd);
	CMainFrame* mainFrame = (CMainFrame*)pFrame;
	mainFrame->SetWindowTextA(strFileName + " - PhxWorldEditor");
}

void CPhxWorldEditorView::OnSettingsEnvironment()
{
	OnEnvironmentSettings();
}

// @}-;-- drulz
