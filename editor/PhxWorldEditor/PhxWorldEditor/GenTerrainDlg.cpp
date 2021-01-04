// GenTerrainDlg.cpp : implementation file
//

#include "stdafx.h"
#include "PhxWorldEditor.h"
#include "GenTerrainDlg.h"
#include "afxdialogex.h"
#include <string>
#include "Terrain.h"
#include "PhxWorldEditorView.h"

// CGenTerrainDlg dialog

IMPLEMENT_DYNAMIC(CGenTerrainDlg, CDialogEx)

CGenTerrainDlg::CGenTerrainDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CGenTerrainDlg::IDD, pParent)
	, m_genType(0), m_tileType(0)
{
	//...
}

CGenTerrainDlg::~CGenTerrainDlg()
{
}

void CGenTerrainDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT3, m_widthCtl);
	DDX_Control(pDX, IDC_EDIT2, m_heightCtl);
	DDX_Control(pDX, IDC_EDIT1, m_tileDistCtl);
	DDX_Control(pDX, IDC_EDIT6, m_noiseSizeCtl);
	DDX_Control(pDX, IDC_EDIT7, m_persistenceCtl);
	DDX_Control(pDX, IDC_EDIT8, m_octaveCtl);
	DDX_Control(pDX, IDC_EDIT4, m_baseTextreCtl);
	DDX_Control(pDX, IDC_EDIT5, m_tiledRateCtl);	
}


BEGIN_MESSAGE_MAP(CGenTerrainDlg, CDialogEx)
	ON_BN_CLICKED(IDC_RADIO1, &CGenTerrainDlg::OnBnClickedRadio1)
	ON_BN_CLICKED(IDC_RADIO2, &CGenTerrainDlg::OnBnClickedRadio2)
	ON_BN_CLICKED(IDC_RADIO3, &CGenTerrainDlg::OnBnClickedRadio3)
	ON_BN_CLICKED(IDC_RADIO5, &CGenTerrainDlg::OnBnClickedRadio5)
	ON_BN_CLICKED(IDC_RADIO4, &CGenTerrainDlg::OnBnClickedRadio4)
	ON_BN_CLICKED(IDC_BUTTON1, &CGenTerrainDlg::OnBnClickedButton1)
END_MESSAGE_MAP()

BOOL CGenTerrainDlg::OnInitDialog( )
{
	CDialogEx::OnInitDialog();

	m_widthCtl.SetWindowTextA("100");
	m_heightCtl.SetWindowTextA("100");
	m_tileDistCtl.SetWindowTextA("1.0");
	m_noiseSizeCtl.SetWindowTextA("2.0");;
	m_persistenceCtl.SetWindowTextA("0.1");
	m_octaveCtl.SetWindowTextA("8");
	m_baseTextreCtl.SetWindowTextA("");
	m_tiledRateCtl.SetWindowTextA("5");

	CButton* pTmp = (CButton*)this->GetDlgItem(IDC_RADIO1);
	pTmp->SetCheck(true);

	pTmp = (CButton*)this->GetDlgItem(IDC_RADIO4);
	pTmp->SetCheck(true);

	m_noiseSizeCtl.EnableWindow(FALSE);
	m_persistenceCtl.EnableWindow(FALSE);
	m_octaveCtl.EnableWindow(FALSE);

	return TRUE;
}

void CGenTerrainDlg::OnOK(void)
{
	TerrainInitParam param = {0};

	//... Check parameters
	CPhxWorldEditorView* view = 
		(CPhxWorldEditorView*) m_pParentWnd;

	if( !view ) return;

	try{
		CString value;
		m_widthCtl.GetWindowTextA(value);
		param.width = atoi(value);

		m_heightCtl.GetWindowTextA(value);
		param.height = atoi(value);
		param.genType = (TERRAIN_GENTYPE)m_genType;

		m_baseTextreCtl.GetWindowTextA(value);
		param.strBaseTexture = value;
		
		m_tiledRateCtl.GetWindowTextA(value);
		param.repeatRate = atoi(value);
		
		m_tileDistCtl.GetWindowTextA(value);
		param.tileDist = atof(value);

		if( param.genType  ==  TERRAIN_GENTYTPE_RANDOM ) {
			// read random parameters
			m_noiseSizeCtl.GetWindowTextA(value);
			param.randParam.noiseSize = atof(value);

			 m_persistenceCtl.GetWindowTextA(value);
			param.randParam.persistence = atof(value);

			 m_octaveCtl.GetWindowTextA(value);
			param.randParam.octaves = atoi(value);
		}

	}catch(...){
		return;
	}

	view->GoGenerateTerrain( param );
	CDialog::OnOK();
}
// CGenTerrainDlg message handlers


void CGenTerrainDlg::OnBnClickedRadio1()
{
	// TODO: Add your control notification handler code here
	m_genType = 0;

	m_noiseSizeCtl.EnableWindow(FALSE);
	m_persistenceCtl.EnableWindow(FALSE);
	m_octaveCtl.EnableWindow(FALSE);
}


void CGenTerrainDlg::OnBnClickedRadio2()
{
	// TODO: Add your control notification handler code here
	m_genType = 1;

	m_noiseSizeCtl.EnableWindow(TRUE);
	m_persistenceCtl.EnableWindow(TRUE);
	m_octaveCtl.EnableWindow(TRUE);
}


void CGenTerrainDlg::OnBnClickedRadio3()
{
	// TODO: Add your control notification handler code here
	m_genType = 2;

	m_noiseSizeCtl.EnableWindow(FALSE);
	m_persistenceCtl.EnableWindow(FALSE);
	m_octaveCtl.EnableWindow(FALSE);
}




void CGenTerrainDlg::OnBnClickedRadio5()
{
	// TODO: Add your control notification handler code here
	m_tileType = 0;
	m_tiledRateCtl.EnableWindow(FALSE);

}


void CGenTerrainDlg::OnBnClickedRadio4()
{
	// TODO: Add your control notification handler code here
	m_tileType = 1;

	m_tiledRateCtl.EnableWindow(TRUE);
}


void CGenTerrainDlg::OnBnClickedButton1()
{
	// TODO: Add your control notification handler code here
	static TCHAR BASED_CODE szFilter[] = _T("Image Files |*.jpg; *.bmp; *.png;")
		_T("*.tga;*.dds;*.psd;*.hdr| All Files (*.*)|*.*||");

	CFileDialog fOpenDlg(TRUE, "", "", OFN_HIDEREADONLY|OFN_FILEMUSTEXIST, szFilter, this);
  
	fOpenDlg.m_pOFN->lpstrTitle="Load Image Files";
  
	//fOpenDlg.m_pOFN->lpstrInitialDir="c:";
  
	if(fOpenDlg.DoModal()==IDOK)
	{
		m_baseTextreCtl.SetWindowTextA( fOpenDlg.GetPathName() );
	}
}

