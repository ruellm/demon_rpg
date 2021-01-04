
// OpenGLSDIView.cpp : implementation of the COpenGLSDIView class
//
// based from this tutorial 
// http://gear.kku.ac.th/~rujchai/ComGraphics/GLwithMFC.html
//

#include "stdafx.h"
// SHARED_HANDLERS can be defined in an ATL project implementing preview, thumbnail
// and search filter handlers and allows sharing of document code with that project.
#ifndef SHARED_HANDLERS
#include "OpenGLSDI.h"
#endif

#include "OpenGLSDIDoc.h"
#include "OpenGLSDIView.h"

#include "common.h"
HDC g_hDC;
HGLRC g_hrc;

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// COpenGLSDIView

IMPLEMENT_DYNCREATE(COpenGLSDIView, CView)

BEGIN_MESSAGE_MAP(COpenGLSDIView, CView)
	// Standard printing commands
	ON_COMMAND(ID_FILE_PRINT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, &CView::OnFilePrintPreview)
	ON_WM_CREATE()
	ON_WM_TIMER()
	ON_WM_SIZE()
	ON_WM_ERASEBKGND()
	ON_WM_DESTROY()
END_MESSAGE_MAP()

// COpenGLSDIView construction/destruction

COpenGLSDIView::COpenGLSDIView()
{
	// TODO: add construction code here

}

COpenGLSDIView::~COpenGLSDIView()
{
}

BOOL COpenGLSDIView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	// For OpenGL rendering
	cs.style |= ( WS_CLIPCHILDREN | 
    WS_CLIPSIBLINGS | CS_OWNDC ); 

	return CView::PreCreateWindow(cs);
}

// COpenGLSDIView drawing

void COpenGLSDIView::OnDraw(CDC* /*pDC*/)
{
	COpenGLSDIDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;

	// TODO: add draw code for native data here
}


// COpenGLSDIView printing

BOOL COpenGLSDIView::OnPreparePrinting(CPrintInfo* pInfo)
{
	// default preparation
	return DoPreparePrinting(pInfo);
}

void COpenGLSDIView::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: add extra initialization before printing
}

void COpenGLSDIView::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: add cleanup after printing
}


// COpenGLSDIView diagnostics

#ifdef _DEBUG
void COpenGLSDIView::AssertValid() const
{
	CView::AssertValid();
}

void COpenGLSDIView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

COpenGLSDIDoc* COpenGLSDIView::GetDocument() const // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(COpenGLSDIDoc)));
	return (COpenGLSDIDoc*)m_pDocument;
}
#endif //_DEBUG


// COpenGLSDIView message handlers


int COpenGLSDIView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CView::OnCreate(lpCreateStruct) == -1)
		return -1;

	SetupOGLContext();
	SetTimer(1.0/60.0f, 1, 0); // 60 FPS
	return 0;
}

//---------------------------------------------------------------------------
// !!OpenGL context Setup related!!
bool COpenGLSDIView::SetupOGLContext()
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

		CString str;
		str.Format(_T("OpenGL version: %s\n"),(CString)glGetString(GL_VERSION));
		TRACE(str);
		return true;
}

void COpenGLSDIView::OnTimer(UINT nIDEvent)
{
#if 0
	switch (nIDEvent)
	{
		case 1:
		{
			wglMakeCurrent(NULL, NULL);
			wglMakeCurrent(g_hDC, g_hrc);

			DrawScene();
			
			wglMakeCurrent(NULL, NULL);
			break;
		}
		default:
			break;
	}
#endif

	wglMakeCurrent(NULL, NULL);
	wglMakeCurrent(g_hDC, g_hrc);
	
	DrawScene();
			
	wglMakeCurrent(NULL, NULL);

	CWnd::OnTimer(nIDEvent);
}

void COpenGLSDIView::DrawScene()
{
	//--------------------------------
	glClearColor(0.22, 0.22, 0.22, 0.0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glEnable(GL_DEPTH_TEST);


	//--------------------------------
	glFlush();
	SwapBuffers(g_hDC);
}

 void COpenGLSDIView::OnSize(UINT nType, int cx, int cy) 
  { 
       CView::OnSize(nType, cx, cy); 

       // TODO: Add your message handler code here 
       wglMakeCurrent( g_hDC, g_hrc ); 
       glViewport( 0, 0, cx, cy); 

       wglMakeCurrent( NULL, NULL ); 
       InvalidateRect(NULL); 
  } 
  BOOL COpenGLSDIView::OnEraseBkgnd(CDC* pDC) 
  { 
       // TODO: Add your message handler code here and/or call default 

       return 1; 
  } 
  void COpenGLSDIView::OnDestroy() 
  { 
       CView::OnDestroy(); 

       // TODO: Add your message handler code here 
       wglMakeCurrent( NULL, NULL ); 
       wglDeleteContext( g_hrc ); 
       ::ReleaseDC( m_hWnd, g_hDC ); 
  } 