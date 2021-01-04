#pragma once

#include "resource.h"

// CAdvanceTexDlg dialog

class CAdvanceTexDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CAdvanceTexDlg)

public:
	CAdvanceTexDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CAdvanceTexDlg();

// Dialog Data
	enum { IDD = IDD_ADVANCETEX_DLG };

	afx_msg BOOL OnInitDialog( );
	void OnOK(void);

	void SetIndex(int index );

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()

	int m_activeIdx;
};
