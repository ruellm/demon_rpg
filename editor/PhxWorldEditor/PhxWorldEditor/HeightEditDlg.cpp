// HeightEditDlg.cpp : implementation file
//

//#include "stdafx.h"
#include "HeightEditDlg.h"
#include "afxdialogex.h"


// CHeightEditDlg dialog

//IMPLEMENT_DYNAMIC(CHeightEditDlg, CDialogEx)

CHeightEditDlg::CHeightEditDlg(/*CWnd* pParent /*=NULL*/) :
	//: CDialogEx(CHeightEditDlg::IDD, pParent)
	m_editType(0), m_raiseType(0)
{

}

CHeightEditDlg::~CHeightEditDlg()
{
}

void CHeightEditDlg::DoDataExchange(CDataExchange* pDX)
{
	CPaneDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CHeightEditDlg, CPaneDialog)
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_WM_SHOWWINDOW()
	ON_BN_CLICKED(IDC_RADIO1, &CHeightEditDlg::OnBnClickedRadio1)
	ON_BN_CLICKED(IDC_RADIO2, &CHeightEditDlg::OnBnClickedRadio2)
	ON_BN_CLICKED(IDC_RADIO6, &CHeightEditDlg::OnBnClickedRadio4)
	ON_BN_CLICKED(IDC_RADIO7, &CHeightEditDlg::OnBnClickedRadio5)
	ON_BN_CLICKED(IDC_APPLY_HGTSET, &CHeightEditDlg::OnBnClickedApplyHgtset)
	ON_UPDATE_COMMAND_UI(IDC_APPLY_HGTSET, &CHeightEditDlg::OnUpdateButton5)
END_MESSAGE_MAP()


int CHeightEditDlg::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CDockablePane::OnCreate(lpCreateStruct) == -1)
		return -1;

	return 0;
}

void CHeightEditDlg::InitValues()
{
	CWnd* wnd = GetDlgItem(IDC_MAXHEIGHT);
	wnd->SetWindowTextA(_T("10.0"));		  // set Max height to 100

	wnd = GetDlgItem(IDC_HEDT_STEPSIZE);
	wnd->SetWindowTextA(_T("5.0"));		  // set Step Size

	wnd = GetDlgItem(IDC_RADIUS);
	wnd->SetWindowTextA(_T("10"));		  // set Circle Radius
	
	wnd = GetDlgItem(IDC_RECT_WIDTH);
	wnd->SetWindowTextA(_T("10"));
	wnd->EnableWindow(FALSE);
	
	wnd = GetDlgItem(IDC_RECT_HEIGHT);
	wnd->SetWindowTextA(_T("10"));	
	wnd->EnableWindow(FALSE);

	CButton* pTmp = (CButton*)GetDlgItem(IDC_RADIO1);
	pTmp->SetCheck(true);

	pTmp = (CButton*)GetDlgItem(IDC_RADIO6);
	pTmp->SetCheck(true);

	param.mode = EDIT_MODE_CIRCLE;
	param.max_height = 10.0f;
	param.radius  = 10.0f;
	param.step_size  = 5.0f;
	param.width = 10;
	param.height = 10;

	// Enable the button
	pTmp = (CButton*)GetDlgItem(IDC_APPLY_HGTSET);
	pTmp->EnableWindow(TRUE);
}

void CHeightEditDlg::OnSize(UINT nType, int cx, int cy)
{
	CDockablePane::OnSize(nType, cx, cy);
	AdjustLayout();
}

// CHeightEditDlg message handlers


void CHeightEditDlg::OnBnClickedRadio1()
{
	m_editType = 0;

	CWnd* wnd = GetDlgItem(IDC_RECT_WIDTH);
	wnd->EnableWindow(FALSE);
	
	wnd = GetDlgItem(IDC_RECT_HEIGHT);
	wnd->EnableWindow(FALSE);

	 wnd = GetDlgItem(IDC_RADIUS);
	wnd->EnableWindow(TRUE);
}


void CHeightEditDlg::OnBnClickedRadio2()
{
	m_editType = 1;

	CWnd* wnd = GetDlgItem(IDC_RECT_WIDTH);
	wnd->EnableWindow(TRUE);
	
	wnd = GetDlgItem(IDC_RECT_HEIGHT);
	wnd->EnableWindow(TRUE);

	 wnd = GetDlgItem(IDC_RADIUS);
	wnd->EnableWindow(FALSE);
}


void CHeightEditDlg::OnBnClickedRadio4()
{
	m_raiseType = 0;
}


void CHeightEditDlg::OnBnClickedRadio5()
{
	m_raiseType = 1;
}

void CHeightEditDlg::OnUpdateButton5(CCmdUI *pCmdUI)
{
	pCmdUI->Enable(TRUE);
}


void CHeightEditDlg::OnBnClickedApplyHgtset()
{
	try{
		param.mode = (EDIT_MODE)m_editType;
		
		CWnd* wnd = GetDlgItem(IDC_MAXHEIGHT);
		CString value;
		wnd->GetWindowTextA(value);
		param.max_height = atof(value);

		wnd = GetDlgItem(IDC_HEDT_STEPSIZE);
		wnd->GetWindowTextA(value);
		param.step_size = atof(value);

		wnd = GetDlgItem(IDC_RADIUS);
		wnd->GetWindowTextA(value);
		param.radius = atof(value);

		wnd = GetDlgItem(IDC_RECT_WIDTH);
		wnd->GetWindowTextA(value);
		param.width = atoi(value);
	
		wnd = GetDlgItem(IDC_RECT_HEIGHT);
		wnd->GetWindowTextA(value);
		param.height = atoi(value);

		//we are going to lower the terrain
		if( m_raiseType ) {
		   param.step_size = -param.step_size;
		}

	}catch(...){
		return;
	}
}

