
// OpenGLSDIView.h : interface of the COpenGLSDIView class
//

#pragma once


class COpenGLSDIView : public CView
{
protected: // create from serialization only
	COpenGLSDIView();
	DECLARE_DYNCREATE(COpenGLSDIView)

// Attributes
public:
	COpenGLSDIDoc* GetDocument() const;

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
	virtual ~COpenGLSDIView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg  BOOL OnEraseBkgnd(CDC* pDC) ;
	afx_msg   void OnDestroy(); 
private:
	bool SetupOGLContext();
	void DrawScene();
};

#ifndef _DEBUG  // debug version in OpenGLSDIView.cpp
inline COpenGLSDIDoc* COpenGLSDIView::GetDocument() const
   { return reinterpret_cast<COpenGLSDIDoc*>(m_pDocument); }
#endif

