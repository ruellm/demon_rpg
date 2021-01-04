#pragma once
#include "afxwin.h"


// CGenTerrainDlg dialog

class CGenTerrainDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CGenTerrainDlg)

public:
	CGenTerrainDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CGenTerrainDlg();

// Dialog Data
	enum { IDD = IDD_GEN_TERRAIN };

	void OnOK(void);

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	CEdit m_widthCtl;
	CEdit m_heightCtl;
	CEdit m_tileDistCtl;
	CEdit m_noiseSizeCtl;
	CEdit m_persistenceCtl;
	CEdit m_octaveCtl;
	CEdit m_baseTextreCtl;
	CEdit m_tiledRateCtl;
	int m_genType;
	int m_tileType;

	afx_msg void OnBnClickedRadio1();
	afx_msg void OnBnClickedRadio2();
	afx_msg void OnBnClickedRadio3();
	afx_msg void OnBnClickedRadio5();
	afx_msg void OnBnClickedRadio4();
	afx_msg BOOL OnInitDialog( );
	afx_msg void OnBnClickedButton1();
};
