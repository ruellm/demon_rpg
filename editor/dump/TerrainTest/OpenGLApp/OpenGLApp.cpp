// OpenGLApp.cpp : Defines the entry point for the application.
//

#include "stdafx.h"
#include "OpenGLApp.h"

#include "common.h"
#include "Camera.h"
#include "GLShaderFx.h"

#include "Terrain.h"



#define MAX_LOADSTRING 100

// Global Variables:
HINSTANCE hInst;								// current instance
TCHAR szTitle[MAX_LOADSTRING];					// The title bar text
TCHAR szWindowClass[MAX_LOADSTRING];			// the main window class name

// Forward declarations of functions included in this code module:
ATOM				MyRegisterClass(HINSTANCE hInstance);
BOOL				InitInstance(HINSTANCE, int);
LRESULT CALLBACK	WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK	About(HWND, UINT, WPARAM, LPARAM);
													 

// Main window Device Context
HDC g_hDC = NULL;
HGLRC g_hrc;                        // OpenGL Rendering Context 
Camera g_camera;
GLShaderFx shader;
GLuint m_WVPObject;

bool model_loaded = false;
float g_angleX = 0.0f;
float g_angleY = 0.0f;
float g_angleZ = 0.0f;
GLuint model_world_object = 0;
GLuint gBaseSampler = 0;

glm::mat4 m_modelMatx = glm::mat4(1.0f);
glm::mat4 project = glm::mat4(1.0f);

bool m_mouseLBtnDown = false;
POINT m_mouseLoc;
Terrain* terrain = NULL;
float g_elapsed = 0.0f;

GLuint texSampler0 = 0;
GLuint texAlpha0 = 0;

//---------------------------------------------------------------------------
// !!OpenGL context Setup related!!
bool SetupOGLContext()
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
		return true;
}

void DestroyOGLContext()
{
	wglMakeCurrent(NULL, NULL);
	if(g_hrc)
	{
		wglDeleteContext(g_hrc);
		g_hrc = NULL;
	}

	delete terrain;
}
//---------------------------------------------------------------------------

void Init();
void Update(float elapsed);
void Draw();

int APIENTRY _tWinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPTSTR    lpCmdLine,
                     int       nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

 	// TODO: Place code here.
	MSG msg;
	HACCEL hAccelTable;

	// Initialize global strings
	LoadString(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
	LoadString(hInstance, IDC_OPENGLAPP, szWindowClass, MAX_LOADSTRING);
	MyRegisterClass(hInstance);

	// Perform application initialization:
	if (!InitInstance (hInstance, nCmdShow))
	{
		return FALSE;
	}

	hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_OPENGLAPP));

	//Initialize OpenGL context
	SetupOGLContext();

	ZeroMemory( &msg, sizeof( MSG ) );
	DWORD lastTime = timeGetTime(); 

	// initialize scene
	Init();

	while( msg.message != WM_QUIT )	
	{
		if( PeekMessage( &msg, NULL, 0, 0, PM_REMOVE ) )
		{
			TranslateMessage( &msg );
			DispatchMessage( &msg );
		}
		else{
			DWORD currTime = timeGetTime();
			g_elapsed = ((float)currTime - (float)lastTime) /1000.0;
			lastTime = currTime;
			
			wglMakeCurrent(g_hDC, g_hrc);

			Update(g_elapsed);
			Draw();

			wglMakeCurrent(NULL, NULL);
		}
	}

	DestroyOGLContext();
	return (int) msg.wParam;
}



//
//  FUNCTION: MyRegisterClass()
//
//  PURPOSE: Registers the window class.
//
//  COMMENTS:
//
//    This function and its usage are only necessary if you want this code
//    to be compatible with Win32 systems prior to the 'RegisterClassEx'
//    function that was added to Windows 95. It is important to call this function
//    so that the application will get 'well formed' small icons associated
//    with it.
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
	WNDCLASSEX wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.style			= CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc	= WndProc;
	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= 0;
	wcex.hInstance		= hInstance;
	wcex.hIcon			= LoadIcon(hInstance, MAKEINTRESOURCE(IDI_OPENGLAPP));
	wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground	= (HBRUSH)(COLOR_WINDOW+1);
	wcex.lpszMenuName	= MAKEINTRESOURCE(IDC_OPENGLAPP);
	wcex.lpszClassName	= szWindowClass;
	wcex.hIconSm		= LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

	return RegisterClassEx(&wcex);
}

//
//   FUNCTION: InitInstance(HINSTANCE, int)
//
//   PURPOSE: Saves instance handle and creates main window
//
//   COMMENTS:
//
//        In this function, we save the instance handle in a global variable and
//        create and display the main program window.
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   HWND hWnd;

   hInst = hInstance; // Store instance handle in our global variable

   hWnd = CreateWindow(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
      CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, NULL, NULL, hInstance, NULL);

   if (!hWnd)
   {
      return FALSE;
   }

   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);

	// get Device Context
	g_hDC = ::GetDC(hWnd);


   return TRUE;
}

//
//  FUNCTION: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  PURPOSE:  Processes messages for the main window.
//
//  WM_COMMAND	- process the application menu
//  WM_PAINT	- Paint the main window
//  WM_DESTROY	- post a quit message and return
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	int wmId, wmEvent;
	PAINTSTRUCT ps;
	HDC hdc;

	switch (message)
	{
	case WM_COMMAND:
		wmId    = LOWORD(wParam);
		wmEvent = HIWORD(wParam);
		// Parse the menu selections:
		switch (wmId)
		{
		case IDM_ABOUT:
			DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
			break;
		case IDM_EXIT:
			DestroyWindow(hWnd);
			break;
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
		}
		break;
	case WM_PAINT:
		hdc = BeginPaint(hWnd, &ps);
		// TODO: Add any drawing code here...
		EndPaint(hWnd, &ps);
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	
	case WM_LBUTTONDOWN:  { 

		 //Do Picking in here!
		 POINT screenPos;
		 screenPos.x =  LOWORD(lParam); 
		 screenPos.y =  HIWORD (lParam);

		 RECT rect;
		 RECT clirect;

		// ::GetWindowRect(hWnd, &rect);
		 ::GetClientRect(hWnd, &rect);

		 int HEIGHT =  rect.bottom - rect.top;
		  glm::vec4 viewport = glm::vec4(0,0, rect.right, rect.bottom );

		  // why do we need to invert screen Y coordinates?
		 glm::vec3 start = glm::unProject(
			 glm::vec3(screenPos.x, rect.bottom - screenPos.y, 0.0), 
			 g_camera.GetViewMatrix(),  project, viewport);

		  glm::vec3 end = glm::unProject(
			glm::vec3(screenPos.x, rect.bottom - screenPos.y, 1.0), 
			 g_camera.GetViewMatrix(),  project, viewport);

		glm::vec3 origin = g_camera.GetPosition();
		glm::vec3 direction = glm::normalize( end -start );

		TerrainHitInfo result;
		bool hit = terrain->MousePick(start, direction, &result);
		
		if( hit ) {

			wglMakeCurrent(g_hDC, g_hrc);
			if( GetKeyState(VK_CONTROL) & 0x8000 ) {
				terrain->ModifyVertex( result, 5.0f, 50.0f);
				//terrain->ModifyRect(result, 1.0f, 10, 10);
			}
			else if ( GetKeyState(VK_SHIFT) & 0x8000) {
				//.. Lets paint the terrain!
				terrain->PaintTexture( result, 0, 50.0f);

			}
			
			wglMakeCurrent(NULL, NULL);
		}
	}
    break;
	case WM_LBUTTONUP: {  } break;
	case WM_MOUSEMOVE:{}break;
	case WM_MOUSEWHEEL:	  {
			short zDelta = GET_WHEEL_DELTA_WPARAM(wParam);
			if( zDelta > 0 ) {
				g_camera.Walk( 5  );
			} else {
				g_camera.Walk( -5 );
			}
		}
		break;

	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}

// Message handler for about box.
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(lParam);
	switch (message)
	{
	case WM_INITDIALOG:
		return (INT_PTR)TRUE;

	case WM_COMMAND:
		if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
		{
			EndDialog(hDlg, LOWORD(wParam));
			return (INT_PTR)TRUE;
		}
		break;
	}
	return (INT_PTR)FALSE;
}

void Init()
{
	//...
	shader.Init();
	shader.LoadVertexShaderFromFile("shaders/terrain_vshader.txt");
	shader.LoadFragmentShaderFromFile("shaders/terrain_fshader.txt");
	shader.Build();
	
	shader.Use();
	m_WVPObject = shader.GetUniformLocation("gWVP");
	if(  0xFFFFFFFF == m_WVPObject) {
		::OutputDebugString("Error getting gWVP object");
	}/**/

	gBaseSampler = shader.GetUniformLocation("gBaseSampler");
	if(  0xFFFFFFFF == m_WVPObject) {
		::OutputDebugString("Error getting gBaseSampler object");
	}/**/
	

	GLuint dircolor = shader.GetUniformLocation("gDirectionalLight.Color");
	if(  0xFFFFFFFF == dircolor) {
		::OutputDebugString("Error getting dircolor object");
	}/**/
	
	texSampler0 = shader.GetUniformLocation("gTextureMap[0]");
	texAlpha0 = shader.GetUniformLocation("gAlphaMap[0]");

	GLuint ambient_intensity = 	 shader.GetUniformLocation("gDirectionalLight.AmbientIntensity");
	GLuint lightdir = 	shader.GetUniformLocation("gDirectionalLight.Direction");
	GLuint diffIntensity = 	shader.GetUniformLocation("gDirectionalLight.DiffuseIntensity");

	GLenum error = glGetError();
	
	glUniform3f(dircolor, 0.8f, 0.80f, 0.8f);    
	glUniform1f( ambient_intensity, 0.5f);
	
	glm::vec3 lookat = glm::vec3(0,0,-1) - glm::vec3(0,5,5);
	glm::vec3 dir = glm::normalize( glm::vec3(0,0,-1));

    glUniform3f(lightdir, dir.x, dir.y, dir.z);
    glUniform1f(diffIntensity, 0.5f);

	GLuint repeatRate = shader.GetUniformLocation("repeatRate");
	glUniform1f(repeatRate, 5.0f);
	error = glGetError();

	//setup camera
	g_camera.LookAt( glm::vec3(0,5,5),
		glm::vec3(0,1,0),
		glm::vec3(0,0,-1));

	TerrainInitParam param = {0};
	terrain = new Terrain();
	
	param.width = 100;
	param.height = 100;
	param.strBaseTexture = "resource/grass.jpg";
	param.repeatRate = 5;

	terrain->Create( param );
	terrain->AddTexture("resource/sand.jpg");

	//terrain->Create( 100, 100);
	//terrain->CreateRandom(100,100);

}

void Update(float elapsed)
{
	if( (GetKeyState(VK_LEFT) & 0x8000) ) {
			g_camera.Strafe(-100 * elapsed);
	} else if ( GetKeyState(VK_RIGHT) & 0x8000 ) {
		g_camera.Strafe(100 * elapsed);
	} else if (GetKeyState(VK_UP) & 0x8000) {
		g_camera.Walk(100 * elapsed);
	} else if (GetKeyState(VK_DOWN) & 0x8000) {
		g_camera.Walk(-100 * elapsed);
	}
	
	if( GetKeyState('A') & 0x8000 )	{
		g_camera.Yaw( 100 * elapsed);
	}else if ( GetKeyState('D') & 0x8000 ) {
		g_camera.Yaw( -(100 * elapsed));
	} else if( GetKeyState('W') & 0x8000 )	{
		g_camera.Pitch( 100 * elapsed);
	} else if ( GetKeyState('S') & 0x8000 ) {
		g_camera.Pitch( -(100 * elapsed));
	}

	g_camera.Update();

	shader.Use();

	// Compute model world matrix
	m_modelMatx = glm::mat4(1.0f);
	project = glm::perspective(45.0f, (4.0f / 3.0f), 0.1f, 1000.0f);
	glm::mat4 view = g_camera.GetViewMatrix();
	shader.SetUniformMatrix(m_WVPObject, (project * view * m_modelMatx));


}

void Draw()
{
	glClearColor(0.22, 0.22, 0.22, 0.0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glEnable(GL_DEPTH_TEST);
	glPolygonMode(GL_FRONT, GL_LINE);
	glPolygonMode(GL_BACK, GL_LINE);

	shader.Use();
	glUniform1i(gBaseSampler, 0); 
	glUniform1i(texSampler0, 1); 
	glUniform1i(texAlpha0, 2); 

	terrain->Draw();

	glFlush();
	SwapBuffers(g_hDC);
}

