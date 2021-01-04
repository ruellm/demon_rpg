
// PhoenixDlg.h : header file
//

#pragma once

#include "OpenGLWnd.h"
#include "AnimationLstDlg.h"

// CPhoenixDlg dialog
class CPhoenixDlg : public CDialogEx
{
// Construction
public:
	CPhoenixDlg(CWnd* pParent = NULL);	// standard constructor
	~CPhoenixDlg();
// Dialog Data
	enum { IDD = IDD_PHOENIX_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support


// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	DECLARE_MESSAGE_MAP()


	void StartOpenGL();

private:
	OpenGLWnd m_OGLRenderer;
	CAnimationLstDlg* m_pAnimListDlg;
public:
	afx_msg void OnFileOpen();
	afx_msg void OnFileExit();
	afx_msg void OnFileClose32775();
	afx_msg void OnEditAnimations();
};
