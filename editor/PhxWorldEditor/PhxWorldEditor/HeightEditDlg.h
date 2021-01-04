#pragma once

#include "resource.h"
#include "afxpanedialog.h"

// CHeightEditDlg dialog

enum EDIT_MODE {
	 EDIT_MODE_CIRCLE,
	 EDIT_MODE_RECTANGLE
};

typedef struct HeightEditParam
{

	enum EDIT_MODE mode;
	float max_height;
	float step_size;
	float radius;
	int width;
	int height;

} HeightEditParam;


class CHeightEditDlg : public CPaneDialog
{
	//DECLARE_DYNAMIC(CHeightEditDlg)

public:
	CHeightEditDlg( /*CWnd* pParent = NULL*/ );   // standard constructor
	virtual ~CHeightEditDlg();

	void InitValues();

// Dialog Data
	enum { IDD = IDD_HEIGHTEDIT_DLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);	 

private:
	int m_editType;
	int m_raiseType;
	HeightEditParam param;

public:
	afx_msg void OnBnClickedRadio1();
	afx_msg void OnBnClickedRadio2();
	afx_msg void OnBnClickedRadio4();
	afx_msg void OnBnClickedRadio5();
	afx_msg void OnBnClickedApplyHgtset();
	afx_msg void OnUpdateButton5(CCmdUI *pCmdUI);

	inline HeightEditParam* GetHeightEditParam() {
		return &param;
	}
};
