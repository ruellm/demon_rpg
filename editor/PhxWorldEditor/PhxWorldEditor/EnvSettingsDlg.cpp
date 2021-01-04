// EnvSettingsDlg.cpp : implementation file
//

#include "stdafx.h"
#include "EnvSettingsDlg.h"
#include "afxdialogex.h"


// CEnvSettingsDlg dialog

IMPLEMENT_DYNAMIC(CEnvSettingsDlg, CDialogEx)

CEnvSettingsDlg::CEnvSettingsDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CEnvSettingsDlg::IDD, pParent)
{

}

CEnvSettingsDlg::~CEnvSettingsDlg()
{
}

void CEnvSettingsDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT1, m_ambCtrl_R);
	DDX_Control(pDX, IDC_EDIT2, m_ambCtrl_G);
	DDX_Control(pDX, IDC_EDIT3, m_ambCtrl_B);
	DDX_Control(pDX, IDC_EDIT4, m_ambCtrl_Intensity);
	DDX_Control(pDX, IDC_EDIT10, m_diffuseCtrl_R);
	DDX_Control(pDX, IDC_EDIT11, m_diffuseCtrl_G);
	DDX_Control(pDX, IDC_EDIT9, m_diffuseCtrl_B);
	DDX_Control(pDX, IDC_EDIT13, m_dirVecCtrl_X);
	DDX_Control(pDX, IDC_EDIT14, m_dirVecCtrl_Y);
	DDX_Control(pDX, IDC_EDIT15, m_dirVecCtrl_Z);
	DDX_Control(pDX, IDC_EDIT12, m_dirLight_Intensity);
}


BEGIN_MESSAGE_MAP(CEnvSettingsDlg, CDialogEx)
END_MESSAGE_MAP()


// CEnvSettingsDlg message handlers
BOOL CEnvSettingsDlg::OnInitDialog( )
{
	CDialogEx::OnInitDialog();

	m_ambient.color = glm::vec3(1.0f, 1.0f, 1.0f);
	m_ambient.intensity = 0.5f;

	m_directionalLight.color = glm::vec3( 0.8f, 0.80f, 0.8f);
	m_directionalLight.intensity = 0.10f;

	glm::vec3 lookat = glm::vec3(0,0,-1) - glm::vec3(0,5,5);
	m_directionalLight.direction = lookat;

	LoadValues();

	return TRUE;
}

void CEnvSettingsDlg::LoadValues()
{
	//...
	CString value("");

	// Ambient light values
	value.Format("%.2f", m_ambient.color.r * 255.0f);
	m_ambCtrl_R.SetWindowTextA(value);

	value.Format("%.2f", m_ambient.color.g * 255.0f);
	m_ambCtrl_G.SetWindowTextA(value);

	value.Format("%.2f", m_ambient.color.b * 255.0f);
	m_ambCtrl_B.SetWindowTextA(value);

	value.Format("%.2f", m_ambient.intensity * 100.0f);
	m_ambCtrl_Intensity.SetWindowTextA(value);

	// Directional Light Color
	value.Format("%.2f", m_directionalLight.color.r * 255.0f);
	m_diffuseCtrl_R.SetWindowTextA(value);

	value.Format("%.2f", m_directionalLight.color.g * 255.0f);
	m_diffuseCtrl_G.SetWindowTextA(value);

	value.Format("%.2f", m_directionalLight.color.b * 255.0f);
	m_diffuseCtrl_B.SetWindowTextA(value);

	// Directional Light Vector
	value.Format("%.2f", m_directionalLight.direction.x);
	m_dirVecCtrl_X.SetWindowTextA(value);

	value.Format("%.2f", m_directionalLight.direction.y);
	m_dirVecCtrl_Y.SetWindowTextA(value);

	value.Format("%.2f", m_directionalLight.direction.z);
	m_dirVecCtrl_Z.SetWindowTextA(value);

	value.Format("%.2f", m_directionalLight.intensity * 100.0f);
	m_dirLight_Intensity.SetWindowTextA(value);
}

void CEnvSettingsDlg::OnOK(void)
{
	CString value;
	try{

		//Ambient light values
		m_ambCtrl_R.GetWindowTextA(value);
		m_ambient.color.r = atof(value) / 255.0f;

		m_ambCtrl_G.GetWindowTextA(value);
		m_ambient.color.g = atof(value) / 255.0f;

		m_ambCtrl_B.GetWindowTextA(value);
		m_ambient.color.b = atof(value) / 255.0f;

		m_ambCtrl_Intensity.GetWindowTextA(value);
		m_ambient.intensity = atof(value) / 100.0f;

		// Directional Light values
		m_diffuseCtrl_R.GetWindowTextA(value);
		m_directionalLight.color.r = atof(value) / 255.0f;

		m_diffuseCtrl_G.GetWindowTextA(value);
		m_directionalLight.color.g = atof(value) / 255.0f;

		m_diffuseCtrl_B.GetWindowTextA(value);
		m_directionalLight.color.b = atof(value) / 255.0f;

		// Directional Vector
		m_dirVecCtrl_X.GetWindowTextA(value);
		m_directionalLight.direction.x = atof(value);

		m_dirVecCtrl_Y.GetWindowTextA(value);
		m_directionalLight.direction.y = atof(value);

		m_dirVecCtrl_Z.GetWindowTextA(value);
		m_directionalLight.direction.z = atof(value);
		
		m_dirLight_Intensity.GetWindowTextA(value);
		m_directionalLight.intensity  = atof(value) / 100.0f;

	}catch(...){
		MessageBox("Error in Saving values, please check input!");
	}


	CDialogEx::OnOK();
}