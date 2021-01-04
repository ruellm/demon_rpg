
// PhoenixDlg.cpp : implementation file
//

#include "stdafx.h"
#include "Phoenix.h"
#include "PhoenixDlg.h"
#include "afxdialogex.h"
#include "OBJModel.h"
#include "PathUtility.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#include "Log.h"

// CAboutDlg dialog used for App About

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// Dialog Data
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

// Implementation
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CPhoenixDlg dialog



CPhoenixDlg::CPhoenixDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CPhoenixDlg::IDD, pParent), 
	m_pAnimListDlg(NULL)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

CPhoenixDlg::~CPhoenixDlg()
{
	delete m_pAnimListDlg;

#ifndef _DEBUG
	Log::Close();
#endif
}

void CPhoenixDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CPhoenixDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_WM_SIZE()
	ON_COMMAND(ID_FILE_OPEN, &CPhoenixDlg::OnFileOpen)
	ON_COMMAND(ID_FILE_EXIT, &CPhoenixDlg::OnFileExit)
	ON_COMMAND(ID_FILE_CLOSE32775, &CPhoenixDlg::OnFileClose32775)
	ON_COMMAND(ID_EDIT_ANIMATIONS, &CPhoenixDlg::OnEditAnimations)
END_MESSAGE_MAP()


// CPhoenixDlg message handlers

BOOL CPhoenixDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

#ifndef _DEBUG
	Log::SetLogMode(LOG_MODE_FILE);
	Log::SetLogFile("Log.txt");
#endif

	// TODO: Add extra initialization here
	StartOpenGL();
	//ShowWindow(SW_SHOWMAXIMIZED);
	m_pAnimListDlg = new CAnimationLstDlg();

	BOOL ret = m_pAnimListDlg->Create(IDD_ANIMATION_DIALOG, this);

	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CPhoenixDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CPhoenixDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CPhoenixDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CPhoenixDlg::StartOpenGL()
{

	HWND dialogWnd = GetSafeHwnd();
	CRect rect;

	GetWindowRect(&rect);
	ScreenToClient(&rect);
	
	m_OGLRenderer.CreateGLContext(rect, this);
	m_OGLRenderer.PrepareScene(); //will show without this but as black & white.

	// Setup the OpenGL Window's timer to render
	m_OGLRenderer.m_unpTimer = m_OGLRenderer.SetTimer(1, 1, 0);

}

void CPhoenixDlg::OnSize(UINT nType, int cx, int cy)
{
	CDialog::OnSize(nType, cx, cy);

	if (0 >= cx || 0 >= cy || nType == SIZE_MINIMIZED)
		return;

	switch (nType)
	{
		case SIZE_RESTORED:
		case SIZE_MAXIMIZED:
		{
			//if(bOpenGLWindowsExists)
			{
				//Reshape here is just maps to OnSize();
				m_OGLRenderer.Reshape(nType,m_OGLRenderer.GetViewPortWidth(), 
					m_OGLRenderer.GetViewPortHeight()); //viewport WxH input here
				
				//sizeable
				CRect rect = m_OGLRenderer.GetWinRect();
				m_OGLRenderer.SetViewPortSize(cx-rect.left, cy-rect.top);
				m_OGLRenderer.Reshape(nType,m_OGLRenderer.GetViewPortWidth(), 
					m_OGLRenderer.GetViewPortHeight());

				
			}
			break;
		}

		default:
			break;
	}
}

void CPhoenixDlg::OnFileOpen()
{
	static TCHAR BASED_CODE szFilter[] = _T("Wavefront OBJ Files (*.obj)|*.obj|")
		_T("Quake MD5 files (*.md5mesh)|*.md5mesh| All Files (*.*)|*.*||");

	CFileDialog fOpenDlg(TRUE, "", "", OFN_HIDEREADONLY|OFN_FILEMUSTEXIST, szFilter, this);
  
	fOpenDlg.m_pOFN->lpstrTitle="Load 3D Model";
  
	//fOpenDlg.m_pOFN->lpstrInitialDir="c:";
  
	if(fOpenDlg.DoModal()==IDOK)
	{
		//fOpenDlg.GetPathName()
		//Testing for OBJ loader
		std::string file(fOpenDlg.GetPathName());
		std::string path = _GetDir(file);
		//OBJModel model;
		m_OGLRenderer.CloseModel();
		m_OGLRenderer.LoadModel(fOpenDlg.GetFileName(), path.c_str());
	}
}




void CPhoenixDlg::OnFileExit()
{
	// same as double-clicking on main window close box
	ASSERT(AfxGetMainWnd() != NULL);
	AfxGetMainWnd()->SendMessage(WM_CLOSE);
}


void CPhoenixDlg::OnFileClose32775()
{
	 m_OGLRenderer.CloseModel();
	m_pAnimListDlg->ClearList();
}


void CPhoenixDlg::OnEditAnimations()
{
	m_pAnimListDlg->m_pOpenGLRef = &m_OGLRenderer;
	m_pAnimListDlg->ShowWindow(SW_SHOW);
}
