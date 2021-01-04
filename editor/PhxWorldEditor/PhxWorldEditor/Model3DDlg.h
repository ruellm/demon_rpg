#pragma once

#include "resource.h"
#include "afxpanedialog.h"
// CModel3DDlg dialog

class CModel3DDlg : public CPaneDialog
{
	//DECLARE_DYNAMIC(CModel3DDlg)

public:
	CModel3DDlg(/*CWnd* pParent = NULL*/);   // standard constructor
	virtual ~CModel3DDlg();

// Dialog Data
	enum { IDD = IDD_MODEL3D_DLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedAddobject();
	afx_msg void OnUpdateButton(CCmdUI *pCmdUI);
	afx_msg void OnUpdateUseButton(CCmdUI *pCmdUI);
	afx_msg void OnBnClickedUseobject();

	int GetSelectedModel( );
	void Clear();
	 CListBox* GetListCtrl();

public:
	afx_msg void OnBnClickedCheck1();
	afx_msg void OnLbnSelchangeList1();
	afx_msg void OnBnClickedDelobject();
};
