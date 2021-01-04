#pragma once

#include "resource.h"
#include "OpenGLWnd.h"
#include "afxwin.h"
// CAnimationLstDlg dialog

class CAnimationLstDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CAnimationLstDlg)

public:
	CAnimationLstDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CAnimationLstDlg();

// Dialog Data
	enum { IDD = IDD_ANIMATION_DIALOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedAdd();

	void ClearList();

	OpenGLWnd* m_pOpenGLRef;
	CListBox m_animFnameList;
	afx_msg void OnLbnSelchangeList1();
};
