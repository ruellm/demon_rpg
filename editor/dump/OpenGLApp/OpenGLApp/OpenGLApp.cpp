// OpenGLApp.cpp : Defines the entry point for the application.
//

#include "stdafx.h"
#include "OpenGLApp.h"

#include "common.h"
#include "Camera.h"
#include "GLShaderFx.h"
#include "MD5Model.h"

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
GLuint g_sampler = 0;

glm::mat4 m_modelMatx = glm::mat4(1.0f);

bool m_mouseLBtnDown = false;
POINT m_mouseLoc;

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
			float elapsed = ((float)currTime - (float)lastTime) /1000.0;
			lastTime = currTime;
			
			wglMakeCurrent(g_hDC, g_hrc);

			Update(elapsed);
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
	
	case WM_LBUTTONDOWN:   
         m_mouseLBtnDown = true;                    
    break;
	case WM_LBUTTONUP:    
         m_mouseLBtnDown = false;
		 m_mouseLoc.x =  LOWORD(lParam); 
		 m_mouseLoc.y =  HIWORD (lParam);
    break;
	case WM_MOUSEMOVE:

		POINT loc;

		POINT result;
		result.x=  loc.x - m_mouseLoc.x;
		result.y=  loc.x - m_mouseLoc.y;

		float angleX = glm::radians( glm::abs((float)result.x) );
		float angleY = glm::radians( glm::abs((float)result.y) );

		//... perform drag effect
		float friction = 2.0f;
		if( result.x > 0 ) {
			g_camera.Yaw( -(angleX * friction) );
		} else {
			g_camera.Yaw(  angleX * friction );	
		}

		m_mouseLoc = loc;

		friction = 5.0f;
		if( result.y > 0 ){
			g_camera.Pitch( -(angleY * friction));
		} else {
			g_camera.Pitch(  angleY * friction );
		}

		break;
	case WM_MOUSEWHEEL:
		short zDelta = GET_WHEEL_DELTA_WPARAM(wParam);
		if( zDelta > 0 ) {
			g_camera.Walk( 5  );
		} else {
			g_camera.Walk( -5 );
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
	shader.LoadVertexShaderFromFile("shaders/vshader.txt");
	shader.LoadFragmentShaderFromFile("shaders/fshader.txt");
	shader.Build();
	
	m_WVPObject = shader.GetUniformLocation("gWVP");
	if(  0xFFFFFFFF == m_WVPObject) {
		::OutputDebugString("Error getting gWVP object");
	}/**/

	g_sampler = shader.GetUniformLocation("gSampler");
	if(  0xFFFFFFFF == m_WVPObject) {
		::OutputDebugString("Error getting gSampler object");
	}/**/
	

	//setup camera
	g_camera.LookAt( glm::vec3(0,0,100),
		glm::vec3(0,1,0),
		glm::vec3(0,0,-1));


}

void Update(float elapsed)
{
	if( (GetKeyState(VK_LEFT) & 0x8000) ) {
			g_camera.Strafe(-2 * elapsed);
	} else if ( GetKeyState(VK_RIGHT) & 0x8000 ) {
		g_camera.Strafe(2 * elapsed);
	} else if (GetKeyState(VK_UP) & 0x8000) {
		g_camera.Walk(10 * elapsed);
	} else if (GetKeyState(VK_DOWN) & 0x8000) {
		g_camera.Walk(-10 * elapsed);
	}
	
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

	g_camera.Update();

	shader.Use();

	// Compute model world matrix
	m_modelMatx = glm::mat4(1.0f);
	glm::mat4 rotateX = glm::rotate( g_angleX, glm::vec3(1.0f, 0.0f, 0.0f) );
	glm::mat4 rotateY = glm::rotate( g_angleY, glm::vec3(0.0f, 1.0f, 0.0f) );
	glm::mat4 rotateZ = glm::rotate( g_angleZ, glm::vec3(0.0f, 0.0f, 1.0f) );
	m_modelMatx = rotateZ * rotateY * rotateX ;

	glm::mat4 project = glm::perspective(45.0f, (4.0f / 3.0f), 0.1f, 1000.0f);
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
	glUniform1i(g_sampler, 0);	//map sampler to texture unit 0


	glFlush();
	SwapBuffers(g_hDC);
}

