// AdvanceTexDlg.cpp : implementation file
//

#include "stdafx.h"
#include "AdvanceTexDlg.h"
#include "afxdialogex.h"
#include "TerrainTextureDlg.h"
#include "MainFrm.h"
#include "PhxWorldEditorView.h"

// CAdvanceTexDlg dialog

IMPLEMENT_DYNAMIC(CAdvanceTexDlg, CDialogEx)

CAdvanceTexDlg::CAdvanceTexDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CAdvanceTexDlg::IDD, pParent), 
	m_activeIdx(0)
{
	//...
}

CAdvanceTexDlg::~CAdvanceTexDlg()
{
}

void CAdvanceTexDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CAdvanceTexDlg, CDialogEx)
END_MESSAGE_MAP()


// CAdvanceTexDlg message handlers
BOOL CAdvanceTexDlg::OnInitDialog( )
{
	CWnd* wnd = GetDlgItem(IDC_FROM);
	wnd->SetWindowTextA("0.0");

	wnd = GetDlgItem(IDC_TO);
	wnd->SetWindowTextA("0.0");

	return TRUE;
}

void CAdvanceTexDlg::OnOK(void)
{
	CWnd* wnd = GetDlgItem(IDC_FROM);
	CString value;
	wnd->GetWindowTextA( value );
	float min = atof( value );

	wnd = GetDlgItem(IDC_TO);
	wnd->GetWindowTextA( value );
	float max = atof( value );

	CMainFrame * pFrame = (CMainFrame *)(AfxGetApp()->m_pMainWnd);
	CPhxWorldEditorView* pView = (CPhxWorldEditorView*) pFrame->GetActiveView();
	if( pView->GetTerrain() ) {
		pView->GetTerrain()->SetTextureHeight( m_activeIdx, min, max);
	}

	CDialog::OnOK();

}

void CAdvanceTexDlg::SetIndex(int index ) {
	m_activeIdx = index;

	CMainFrame * pFrame = (CMainFrame *)(AfxGetApp()->m_pMainWnd);
	CPhxWorldEditorView* pView = (CPhxWorldEditorView*) pFrame->GetActiveView();
	if( pView->GetTerrain() ) {
		TerrainTexture* texture =  pView->GetTerrain()->GetTextureAt(index);
		if( texture == NULL ) return;

		CString value;
		value.Format("%2.f", texture->from);
		CWnd* wnd = GetDlgItem(IDC_FROM);
		wnd->SetWindowTextA(value);

		value.Format("%2.f", texture->to);
		wnd = GetDlgItem(IDC_TO);
		wnd->SetWindowTextA(value);
	}
}