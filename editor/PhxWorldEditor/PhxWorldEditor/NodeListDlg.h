#pragma once

#include "resource.h"
#include "afxpanedialog.h"
// CNodeListDlg dialog

class CNodeListDlg : public CPaneDialog
{
	//DECLARE_DYNAMIC(CNodeListDlg)

public:
	CNodeListDlg(/*CWnd* pParent = NULL*/);   // standard constructor
	virtual ~CNodeListDlg();

// Dialog Data
	enum { IDD = IDD_NODELIST };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
};
