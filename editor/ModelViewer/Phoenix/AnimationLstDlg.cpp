// AnimationLstDlg.cpp : implementation file
//

#include "stdafx.h"
#include "AnimationLstDlg.h"
#include "afxdialogex.h"

// CAnimationLstDlg dialog

IMPLEMENT_DYNAMIC(CAnimationLstDlg, CDialogEx)

CAnimationLstDlg::CAnimationLstDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CAnimationLstDlg::IDD, pParent), m_pOpenGLRef( NULL )
{

}

CAnimationLstDlg::~CAnimationLstDlg()
{
}

void CAnimationLstDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST1, m_animFnameList);
}


BEGIN_MESSAGE_MAP(CAnimationLstDlg, CDialogEx)
	ON_BN_CLICKED(IDC_BUTTON2, &CAnimationLstDlg::OnBnClickedAdd)
	ON_LBN_SELCHANGE(IDC_LIST1, &CAnimationLstDlg::OnLbnSelchangeList1)
END_MESSAGE_MAP()


// CAnimationLstDlg message handlers


void CAnimationLstDlg::OnBnClickedAdd()
{
	static TCHAR BASED_CODE szFilter[] = 
		_T("MD5 Animation Files (*.md5anim)|*.md5anim| All Files (*.*)|*.*||");

	CFileDialog fOpenDlg(TRUE, "", "", OFN_HIDEREADONLY|OFN_FILEMUSTEXIST, szFilter, this);
  
	fOpenDlg.m_pOFN->lpstrTitle="Load MD5 Animation File";
  
	//fOpenDlg.m_pOFN->lpstrInitialDir="c:";
  
	if(fOpenDlg.DoModal()==IDOK)
	{
		//...
		std::string fname = fOpenDlg.GetFileName();
			std::string file(fOpenDlg.GetPathName());
		if( m_pOpenGLRef ) {
			if( m_pOpenGLRef->AddMD5Animation( file.c_str() ) )
				m_animFnameList.AddString( fname.c_str() );
				m_pOpenGLRef->SetMD5Animation(m_animFnameList.GetCount()-1);
		}
	}
}

void CAnimationLstDlg::ClearList()
{
	m_animFnameList.ResetContent();
}

void CAnimationLstDlg::OnLbnSelchangeList1()
{
	 int idx = m_animFnameList.GetCurSel();
	 m_pOpenGLRef->SetMD5Animation(idx);
}
