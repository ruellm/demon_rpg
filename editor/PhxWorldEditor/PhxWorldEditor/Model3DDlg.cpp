// Model3DDlg.cpp : implementation file
//

#include "stdafx.h"
#include "PhxWorldEditor.h"
#include "Model3DDlg.h"
#include "afxdialogex.h"
#include "MainFrm.h"
#include "PhxWorldEditorView.h"

// CModel3DDlg dialog

//IMPLEMENT_DYNAMIC(CModel3DDlg, CDialogEx)

CModel3DDlg::CModel3DDlg(/*CWnd* pParent /*=NULL*/)
	//: CDialogEx(CModel3DDlg::IDD, pParent)
{

}

CModel3DDlg::~CModel3DDlg()
{
}

void CModel3DDlg::DoDataExchange(CDataExchange* pDX)
{
	CPaneDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CModel3DDlg, CPaneDialog)
	ON_BN_CLICKED(IDC_ADDOBJECT, &CModel3DDlg::OnBnClickedAddobject)
	ON_UPDATE_COMMAND_UI(IDC_ADDOBJECT, &CModel3DDlg::OnUpdateButton)
	ON_UPDATE_COMMAND_UI(IDC_ADDMD5, &CModel3DDlg::OnUpdateButton)
	ON_UPDATE_COMMAND_UI(IDC_DELOBJECT, &CModel3DDlg::OnUpdateButton)
	ON_BN_CLICKED(IDC_CHECK1, &CModel3DDlg::OnBnClickedCheck1)
	ON_LBN_SELCHANGE(IDC_LIST1, &CModel3DDlg::OnLbnSelchangeList1)
	ON_BN_CLICKED(IDC_DELOBJECT, &CModel3DDlg::OnBnClickedDelobject)
END_MESSAGE_MAP()


// CModel3DDlg message handlers
void CModel3DDlg::OnUpdateButton(CCmdUI *pCmdUI)
{
	pCmdUI->Enable(TRUE);
}


void CModel3DDlg::OnBnClickedAddobject()
{
	static TCHAR BASED_CODE szFilter[] = _T("Wavefront OBJ Files (*.obj)|*.obj| All Files (*.*)|*.*||");

	CFileDialog fOpenDlg(TRUE, "", "", OFN_HIDEREADONLY|OFN_FILEMUSTEXIST, szFilter, this);
  
	fOpenDlg.m_pOFN->lpstrTitle="Load 3D Model";
	CListBox* modelList = (CListBox*)GetDlgItem(IDC_LIST1);

	if(fOpenDlg.DoModal()==IDOK)
	{
		CMainFrame * pFrame = (CMainFrame *)(AfxGetApp()->m_pMainWnd);
		CPhxWorldEditorView* pView = (CPhxWorldEditorView*) pFrame->GetActiveView();
		if( pView->LoadObjModel(fOpenDlg.GetPathName()) ) {

			std::string file= _GetFileName( fOpenDlg.GetPathName() );
			//modelList->AddString( file.c_str() );
			modelList->InsertString(-1, file.c_str());
		}
	}

}

int CModel3DDlg::GetSelectedModel( )
{

	CListBox* modelList = (CListBox*)GetDlgItem(IDC_LIST1);

	if( !IsDlgButtonChecked(IDC_CHECK1 ) ) return -1;
	int idx = modelList->GetCurSel();
	if( idx < 0 ) return -1 ;

	return idx;
}

void CModel3DDlg::OnBnClickedCheck1()
{
	// TODO: Add your control notification handler code here
	//...
	

}


void CModel3DDlg::OnLbnSelchangeList1()
{
	CheckDlgButton(IDC_CHECK1, 0);
}

CListBox* CModel3DDlg::GetListCtrl()
{
	  return (CListBox*)GetDlgItem(IDC_LIST1);
}

void CModel3DDlg::Clear()
{
	//...
	CListBox* list = (CListBox*)GetDlgItem(IDC_LIST1);
	if(list) {
		list->ResetContent();
	}
}

void CModel3DDlg::OnBnClickedDelobject()
{
	// TODO: Add your control notification handler code here
	 CListBox* list = (CListBox*)GetDlgItem(IDC_LIST1);
	  int idx = list->GetCurSel();
	 if( idx < 0 ) return;

	  list->DeleteString( idx );
	  //
	CMainFrame * pFrame = (CMainFrame *)(AfxGetApp()->m_pMainWnd);
	CPhxWorldEditorView* pView = (CPhxWorldEditorView*) pFrame->GetActiveView();

	pView->DeleteObject(idx);
}
