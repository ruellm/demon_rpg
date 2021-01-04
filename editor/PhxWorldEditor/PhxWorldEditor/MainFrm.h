
// MainFrm.h : interface of the CMainFrame class
//

#pragma once
#include "FileView.h"
#include "OutputWnd.h"
#include "PropertiesWnd.h"
#include "HeightEditDlg.h"
#include "TerrainTextureDlg.h"
#include "Model3DDlg.h"
#include "NodeListDlg.h"

class CMainFrame : public CFrameWndEx
{
	
protected: // create from serialization only
	CMainFrame();
	DECLARE_DYNCREATE(CMainFrame)

// Attributes
public:

// Operations
public:

// Overrides
public:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	virtual BOOL LoadFrame(UINT nIDResource, 
		DWORD dwDefaultStyle = WS_OVERLAPPEDWINDOW | FWS_ADDTOTITLE, 
		CWnd* pParentWnd = NULL, CCreateContext* pContext = NULL);

// Implementation
public:
	virtual ~CMainFrame();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:  // control bar embedded members
	CMFCMenuBar       m_wndMenuBar;
	CMFCToolBar       m_wndToolBar;
	CMFCStatusBar     m_wndStatusBar;
	CMFCToolBarImages m_UserImages;
	CFileView         m_wndFileView;
	COutputWnd        m_wndOutput;
	CPropertiesWnd    m_wndProperties;
	CHeightEditDlg	  m_heightEditDlg;
	CTerrainTextureDlg	m_terrainTextureDlg;
	CMFCToolBar			m_cameraTBr;
	CModel3DDlg			m_model3DDlg;
	CNodeListDlg		m_nodeList;

// Generated message map functions
protected:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnViewCustomize();
	afx_msg LRESULT OnToolbarCreateNew(WPARAM wp, LPARAM lp);
	afx_msg void OnApplicationLook(UINT id);
	afx_msg void OnUpdateApplicationLook(CCmdUI* pCmdUI);
	afx_msg void OnSettingChange(UINT uFlags, LPCTSTR lpszSection);
		afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	DECLARE_MESSAGE_MAP()

	BOOL CreateDockingWindows();
	void SetDockingWindowIcons(BOOL bHiColorIcons);
public:
	afx_msg void OnTerrainEditheight();
	afx_msg void OnTextureLayer();
	afx_msg void OnTerrainTexturelayer();
	afx_msg void OnBnClickedApplyHgtset();
	 afx_msg void OnHeightEditDlg();
	 afx_msg void OnModel3DDlg();
	 afx_msg void OnNodeList();

	// Getter/Setters
	inline 	CHeightEditDlg* GetHEditDlg( ) {
		return 	&m_heightEditDlg;
	}

	inline CTerrainTextureDlg* GetTerrainTextureDlg( ) {
		return &m_terrainTextureDlg;
	}

	inline CModel3DDlg* GetModel3DDlg( ) {
		return &m_model3DDlg;
	}
	inline CNodeListDlg* GetNodeListDlg( ) { 
		return &m_nodeList;
	}
};


