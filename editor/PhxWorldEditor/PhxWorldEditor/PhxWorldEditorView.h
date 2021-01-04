
// PhxWorldEditorView.h : interface of the CPhxWorldEditorView class
//

#pragma once
#include "PhxWorldEditorDoc.h"
#include "Terrain.h"
#include "SceneObject.h"
#include "OBJModel.h"
#include "MovementSettingsDlg.h"
#include "EnvSettingsDlg.h"

typedef enum MOUSE_MODE {
	MOUSE_MODE_NONE,
	MOUSE_MODE_FREE,
	MOUSE_MODE_PAN,
	MOUSE_MODE_EDIT_TERRAIN,
	MOUSE_MODE_PAINT_TERRAIN,
	MOUSE_MODE_ADD_OBJ
	//TODO: Selecting and moving object mouse mode
} MOUSE_MODE;

class CPhxWorldEditorView : public CView
{
protected: // create from serialization only
	CPhxWorldEditorView();
	DECLARE_DYNCREATE(CPhxWorldEditorView)

// Attributes
public:
	CPhxWorldEditorDoc* GetDocument() const;

// Operations
public:

// Overrides
public:
	virtual void OnDraw(CDC* pDC);  // overridden to draw this view
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
protected:
	virtual BOOL OnPreparePrinting(CPrintInfo* pInfo);
	virtual void OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnEndPrinting(CDC* pDC, CPrintInfo* pInfo);

// Implementation
public:
	virtual ~CPhxWorldEditorView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
	afx_msg void OnFilePrintPreview();
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	DECLARE_MESSAGE_MAP()

	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg  BOOL OnEraseBkgnd(CDC* pDC) ;
	afx_msg  void OnDestroy(); 
	afx_msg BOOL OnMouseWheel(UINT flags, short zDelta, CPoint pt);
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnLButtonDown(UINT flags, CPoint loc);
	afx_msg void OnLButtonUp(UINT flags, CPoint loc);
	afx_msg void OnMouseMove(UINT flags, CPoint loc);
	afx_msg LRESULT OnMouseHover(WPARAM wparam, LPARAM lparam);
	afx_msg LRESULT OnMouseLeave(WPARAM wparam, LPARAM lparam);

	void OnToolBarFreeCam();
	void OnToolBarPanCam();
	void OnToolBarEdtHgtCam();
	void OnToolBarPntTrnCam();
	void OnToolBarAddObjCam();
private:
	bool SetupOGLContext();
	void Init();
	void DestroyObjects();
	void LoadShaders();
	void Update(float elapsed=1.0f);
	void DrawScene();
	void GenerateGrid(int cellcnt_w, int cellcnt_h);
	void ResizeProjection();
	void DrawTerrain();
	void DrawModels();
	void DrawSelected();
	void OnFreeCamUpdate(CCmdUI* pCmdUI);
	void OnPanCamUpdate(CCmdUI* pCmdUI);
	void OnEdtHgtCamUpdate(CCmdUI* pCmdUI);
	void OnPntTrnCamUpdate(CCmdUI* pCmdUI);
	void OnAddObjCamUpdate(CCmdUI* pCmdUI);
	void UpdateToggleCam(int from);
	void ResetMouseMode();
	void BuildRay( int x, int y, glm::vec3& start, glm::vec3& dir);
	
	int m_width;
	int m_height;
	bool m_bInit;;
	bool m_bSave;

	CDialogEx* m_genTerrainDlg;
	CMovementSettingsDlg* m_moveMentSettingsDlg;
	CEnvSettingsDlg* m_envSettingsDlg;
	Terrain*	m_pTerrain;

	// camera toggle flags
	int m_freeCamFlag;
	int m_PanCamFlag;
	int m_EdtHgtCamFlag;
	int m_PntTrnCamFlag;
	int m_AddObjCamFlag;

	// For camera-mouse movement
	bool m_mouseLBtnDown;
	CPoint m_mouseLoc;
	MOUSE_MODE m_mouseMode;

	// for scene object rendering
	std::vector<Model3D*> m_modelList;
	std::vector<Object3D*> m_obj3DList;
	std::vector<SceneObject*> m_selectedList;

public:
	afx_msg void OnTerrainGenerate();
	afx_msg void OnClose();
	afx_msg void OnMovementSettings();
	afx_msg void OnEnvironmentSettings();

	void OnOpen();
	void OnSave();

	void AddTexture( const char* szPath );
	void GoGenerateTerrain(const TerrainInitParam& param);
	bool LoadObjModel( const char* szPath );
	void DeleteObject( int index );
	inline Terrain* GetTerrain( ) {
		return m_pTerrain;
	}

	inline Model3D* GetModelIdx( int idx ) {
		if( idx > m_modelList.size() ) return 0;
		return m_modelList[idx];
	}
	afx_msg void OnSettingsEnvironment();
};

#ifndef _DEBUG  // debug version in PhxWorldEditorView.cpp
inline CPhxWorldEditorDoc* CPhxWorldEditorView::GetDocument() const
   { return reinterpret_cast<CPhxWorldEditorDoc*>(m_pDocument); }
#endif

