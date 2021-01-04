#pragma once
#include "resource.h"
#include "common.h"
#include "Lights.h"
#include "afxwin.h"

// CEnvSettingsDlg dialog

class CEnvSettingsDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CEnvSettingsDlg)

public:
	CEnvSettingsDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CEnvSettingsDlg();

// Dialog Data
	enum { IDD = IDD_ENVIRONMENT_DLG };

	afx_msg  BOOL OnInitDialog( );
	afx_msg void OnOK(void);

	void LoadValues();
	
	inline AmbientLight GetAmbientLight() {
		return m_ambient;
	}

	inline DirectionalLight GetDirectionalLight() {
		return m_directionalLight;
	}

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()

private:
	AmbientLight m_ambient;
	DirectionalLight m_directionalLight;

public:
	CEdit m_ambCtrl_R;
	CEdit m_ambCtrl_G;
	CEdit m_ambCtrl_B;
	CEdit m_ambCtrl_Intensity;
	CEdit m_diffuseCtrl_R;
	CEdit m_diffuseCtrl_G;
	CEdit m_diffuseCtrl_B;
	CEdit m_dirVecCtrl_X;
	CEdit m_dirVecCtrl_Y;
	CEdit m_dirVecCtrl_Z;
	CEdit m_dirLight_Intensity;
};
