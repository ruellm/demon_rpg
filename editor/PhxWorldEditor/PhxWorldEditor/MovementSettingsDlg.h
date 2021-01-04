#pragma once
#include "afxwin.h"


// CMovementSettingsDlg dialog

class CMovementSettingsDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CMovementSettingsDlg)

public:
	CMovementSettingsDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CMovementSettingsDlg();

// Dialog Data
	enum { IDD = IDD_MOVEMENT_DLG };

	afx_msg  BOOL OnInitDialog( );
	afx_msg void OnOK(void);
	afx_msg void OnCancel(void);
	afx_msg void OnBnClickedOk();

	void InitValues();

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	CEdit m_cameraStepCtl;
	CEdit m_cameraDragFrictionCtl;
	CEdit m_objectStepCtl;
	CEdit m_objectRotateStepCtl;
	CEdit m_panStepCtl;

	float m_cameraStep;
	float m_cameraDragFriction;
	float m_objectStep;
	float m_objectRotate;
	float m_objectScale;
	float m_panStep;

	inline float GetCameraStep( ) {
		return m_cameraStep;
	}

	inline float GetCameraDragFriction( ) {
		return m_cameraDragFriction;
	}

	inline float GetObjectStep( ) {
		return m_objectStep;
	}

	inline float GetObjectRotate( ) {
		return m_objectRotate;
	}

	inline float GetObjectScale( )	{ 
		return m_objectScale;
	}

	inline float GetPanStep() {
		return m_panStep;
	}

	CEdit m_objectScaleCtrl;
};
