// TerrainTextureDlg.cpp : implementation file
//

#include "stdafx.h"
#include "TerrainTextureDlg.h"
#include "afxdialogex.h"
#include "common.h"
#include "PathUtility.h"
#include "MainFrm.h"
#include "PhxWorldEditorView.h"

// CTerrainTextureDlg dialog

//IMPLEMENT_DYNAMIC(CTerrainTextureDlg, CDialogEx)

CTerrainTextureDlg::CTerrainTextureDlg(/*CWnd* pParent /*=NULL*/)  
	: m_pAdvanceDlg(NULL), m_fRadius(0.0f), 
	m_aciveIdx(-1), m_textureList(0), m_fAlpha(1.0f)
{

}

CTerrainTextureDlg::~CTerrainTextureDlg()
{
	SAFE_DELETE(m_pAdvanceDlg);
//	m_infoList.clear();
}

void CTerrainTextureDlg::DoDataExchange(CDataExchange* pDX)
{
	CPaneDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CTerrainTextureDlg, CPaneDialog)
 	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_UPDATE_COMMAND_UI(IDC_ADDTEX, &CTerrainTextureDlg::OnUpdateButton)
	ON_UPDATE_COMMAND_UI(IDC_DELTEX, &CTerrainTextureDlg::OnUpdateButton)
	ON_UPDATE_COMMAND_UI(IDC_ADVTEX, &CTerrainTextureDlg::OnUpdateButton)
	ON_UPDATE_COMMAND_UI(IDC_USETEX, &CTerrainTextureDlg::OnUpdateButton)
	ON_BN_CLICKED(IDC_ADDTEX, &CTerrainTextureDlg::OnBnClickedAddtex)
	ON_BN_CLICKED(IDC_ADVTEX, &CTerrainTextureDlg::OnBnClickedAdvtex)
	ON_BN_CLICKED(IDC_USETEX, &CTerrainTextureDlg::OnBnClickedUsetex)
	ON_BN_CLICKED(IDC_DELTEX, &CTerrainTextureDlg::OnBnClickedDeltex)
	ON_EN_CHANGE(IDC_EDIT1, &CTerrainTextureDlg::OnEnChangeEdit1)
	 ON_WM_HSCROLL() 
	 ON_EN_CHANGE(IDC_TEXALPHA, &CTerrainTextureDlg::OnEnChangeTexalpha)
END_MESSAGE_MAP()

int CTerrainTextureDlg::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CDockablePane::OnCreate(lpCreateStruct) == -1)
		return -1;
	return 0;
}

void CTerrainTextureDlg::InitControls()
{
	m_textureList = (CListBox*)GetDlgItem(IDC_TEXLIST);

 	m_pAdvanceDlg = new CAdvanceTexDlg(this);
	BOOL ret = m_pAdvanceDlg->Create(IDD_ADVANCETEX_DLG, this);

	CWnd* pEdit = GetDlgItem(IDC_PAINT_RADIUS);
	pEdit->SetWindowText("5.0");

	pEdit = GetDlgItem(IDC_TEXALPHA);
	pEdit->SetWindowText("100.0");

	CSliderCtrl* slider = (CSliderCtrl*)GetDlgItem(IDC_SLIDER1);
	slider->SetRange(0, 100);
	slider->SetTicFreq( 1 );
	slider->SetPos(100);

	return;
}

void CTerrainTextureDlg::OnSize(UINT nType, int cx, int cy)
{
	CDockablePane::OnSize(nType, cx, cy);
	AdjustLayout();
}

// CTerrainTextureDlg message handlers

void CTerrainTextureDlg::OnUpdateButton(CCmdUI *pCmdUI)
{
	 pCmdUI->Enable(TRUE);
}


void CTerrainTextureDlg::OnBnClickedAddtex()
{

	m_textureList = (CListBox*)GetDlgItem(IDC_TEXLIST);
	if( m_textureList->GetCount() >=3 ) {
		MessageBox("Maximum of 3 textures allowed (as of the moment)\n");
		return;
	}

	static TCHAR BASED_CODE szFilter[] = _T("Image Files |*.jpg; *.bmp; *.png;")
		_T("*.tga;*.dds;*.psd;*.hdr| All Files (*.*)|*.*||");

	CFileDialog fOpenDlg(TRUE, "", "", OFN_HIDEREADONLY|OFN_FILEMUSTEXIST, szFilter, this);
  
	fOpenDlg.m_pOFN->lpstrTitle="Load Image Files";
  
	//fOpenDlg.m_pOFN->lpstrInitialDir="c:";

	CMainFrame * pFrame = (CMainFrame *)(AfxGetApp()->m_pMainWnd);
	CPhxWorldEditorView* pView = (CPhxWorldEditorView*) pFrame->GetActiveView();
	if ( !pView->GetTerrain() ) return;

	if(fOpenDlg.DoModal()==IDOK)
	{
		CString path = fOpenDlg.GetPathName();
		CString file = _GetFileName( path ).c_str();

#if 0
		TerrainTextureInfo param;
		param.fullPath = path;
		param.fname = _GetFileName( path );
		CString file = param.fname.c_str();
		param.min = 0.0f;
		param.max = 0.0f;
#endif

		//m_textureList->AddString(file);
		m_textureList->InsertString(-1, file);
		//m_infoList.push_back( param);
		pView->AddTexture(path);
		
	}
}


void CTerrainTextureDlg::OnBnClickedAdvtex()
{
	 m_textureList = (CListBox*)GetDlgItem(IDC_TEXLIST);

	int idx = m_textureList->GetCurSel();
    if( idx < 0 ) return;
	 //TerrainTextureInfo* info = (TerrainTextureInfo* )&m_infoList[idx];
	 m_pAdvanceDlg->SetIndex(idx);
	m_pAdvanceDlg->ShowWindow(SW_SHOW);

}

void CTerrainTextureDlg::OnBnClickedUsetex()
{
	CWnd* pEdit = GetDlgItem(IDC_PAINT_RADIUS);
	CString value;
	pEdit->GetWindowTextA(value);
	m_fRadius = atof(value);

	pEdit = GetDlgItem(IDC_TEXALPHA);
	pEdit->GetWindowTextA(value);
	m_fAlpha = atof(value);

	m_textureList = (CListBox*)GetDlgItem(IDC_TEXLIST);

	if( m_textureList ) {
		m_aciveIdx = m_textureList->GetCurSel();
	}

}


void CTerrainTextureDlg::OnBnClickedDeltex()
{

	 m_textureList = (CListBox*)GetDlgItem(IDC_TEXLIST);
	  int idx = m_textureList->GetCurSel();
	if( idx < 0 ) return;

	  m_textureList->DeleteString( idx );
	  //m_infoList.erase(m_infoList.begin()+idx);

}


void CTerrainTextureDlg::OnEnChangeEdit1()
{
	// TODO:  If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CPaneDialog::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.

	// TODO:  Add your control notification handler code here
}

void CTerrainTextureDlg::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	CSliderCtrl* pSlider = reinterpret_cast<CSliderCtrl*>(pScrollBar); 
	CSliderCtrl* slider = (CSliderCtrl*)GetDlgItem(IDC_SLIDER1);
	if(pSlider == slider)	 {
		int pos = slider->GetPos();
		CWnd*	pEdit = GetDlgItem(IDC_TEXALPHA);
		char value[5];
		sprintf(value,"%d", pos);
		pEdit->SetWindowText( value );


	}
}


void CTerrainTextureDlg::OnEnChangeTexalpha()
{
	// TODO:  If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CPaneDialog::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.

	// TODO:  Add your control notification handler code here
	 CWnd* pEdit = GetDlgItem(IDC_TEXALPHA);
	 CString value;
	pEdit->GetWindowTextA(value);
	int pos = atoi(value);
	CSliderCtrl* slider = (CSliderCtrl*)GetDlgItem(IDC_SLIDER1);
	
	if( pos >=0 && pos <= 100 ) {
		slider->SetPos(pos);
	}

}

bool CTerrainTextureDlg::IsUnPaint()
{
	return (bool) IsDlgButtonChecked(IDC_UNPAINT);
}


CListBox* CTerrainTextureDlg::GetListCtrl()
{
	  return (CListBox*)GetDlgItem(IDC_TEXLIST);
}

void CTerrainTextureDlg::Clear()
{
	CListBox* list = (CListBox*)GetDlgItem(IDC_TEXLIST);
	if(list) {
		list->ResetContent();
	}
}