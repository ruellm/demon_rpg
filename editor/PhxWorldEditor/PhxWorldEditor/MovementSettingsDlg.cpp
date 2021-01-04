// MovementSettingsDlg.cpp : implementation file
//

#include "stdafx.h"
#include "PhxWorldEditor.h"
#include "MovementSettingsDlg.h"
#include "afxdialogex.h"


// CMovementSettingsDlg dialog

IMPLEMENT_DYNAMIC(CMovementSettingsDlg, CDialogEx)

CMovementSettingsDlg::CMovementSettingsDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CMovementSettingsDlg::IDD, pParent)
{

}

CMovementSettingsDlg::~CMovementSettingsDlg()
{
}

void CMovementSettingsDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT1, m_cameraStepCtl);
	DDX_Control(pDX, IDC_EDIT4, m_cameraDragFrictionCtl);
	DDX_Control(pDX, IDC_EDIT2, m_objectStepCtl);
	DDX_Control(pDX, IDC_EDIT3, m_objectRotateStepCtl);
	DDX_Control(pDX, IDC_EDIT5, m_panStepCtl);
	DDX_Control(pDX, IDC_SCALE_OBJ, m_objectScaleCtrl);
}


BEGIN_MESSAGE_MAP(CMovementSettingsDlg, CDialogEx)
	ON_BN_CLICKED(IDOK, &CMovementSettingsDlg::OnBnClickedOk)
END_MESSAGE_MAP()


// CMovementSettingsDlg message handlers

BOOL CMovementSettingsDlg::OnInitDialog( )
{
	CDialogEx::OnInitDialog();

	m_cameraStep = 50.0f;
	m_cameraDragFriction = 2.0f;
	m_objectStep = 1.0f;
	m_objectRotate = 180.0f;
	m_panStep = 5.0f;
	m_objectScale = 1.0f;

	InitValues();

	return TRUE;
}

void CMovementSettingsDlg::OnOK(void)
{

	CString value;
	m_cameraStepCtl.GetWindowTextA(value);
	m_cameraStep = atof(value);

	m_cameraDragFrictionCtl.GetWindowTextA(value);
	m_cameraDragFriction = atof(value);

	m_objectStepCtl.GetWindowTextA(value);
	m_objectStep = atof(value);

	m_objectRotateStepCtl.GetWindowTextA(value);
	m_objectRotate = atof(value);

	m_panStepCtl.GetWindowTextA(value);
	m_panStep = atof(value);

	m_objectScaleCtrl.GetWindowTextA(value);
	m_objectScale = atof(value);

	CDialogEx::OnOK();
}

void CMovementSettingsDlg::OnCancel(void)
{

	InitValues();

	CDialogEx::OnCancel();
}

void CMovementSettingsDlg::OnBnClickedOk()
{
	OnOK();
}

void CMovementSettingsDlg::InitValues()
{
	CString value;
	value.Format("%.2f", m_cameraStep);
	m_cameraStepCtl.SetWindowTextA(value);

	value.Format("%.2f", m_cameraDragFriction);
	m_cameraDragFrictionCtl.SetWindowTextA(value);

	value.Format("%.2f", m_objectStep);
	m_objectStepCtl.SetWindowTextA(value);

	value.Format("%.2f", m_objectRotate);
	m_objectRotateStepCtl.SetWindowTextA(value);

	value.Format("%.2f", m_panStep);
	m_panStepCtl.SetWindowTextA(value);
	
	value.Format("%.2f", m_objectScale);
	m_objectScaleCtrl.SetWindowTextA(value);
	
}