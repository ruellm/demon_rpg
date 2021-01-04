#pragma once

#include "common.h"
#include "Camera.h"
#include "GLShaderFx.h"

// OpenGLWnd

class OpenGLWnd : public CWnd
{
public:
	OpenGLWnd();
	virtual ~OpenGLWnd();
	bool CreateGLContext(CRect rect, CWnd *parent);
	void PrepareScene();				// Scene preparation
	void Reshape(UINT nType, int w, int h); //maps to OnSize()
	void DrawScene();				// Draw scene
	void DestroyScene();            // Cleanup
	
	void SetViewPortSize(int ViewPortWidth, int ViewPortHeight) { 
		m_iViewPortWidth = ViewPortWidth; m_iViewPortHeight = ViewPortHeight; };

	CRect &GetWinRect() { return m_WindowOrig; };
	int &GetViewPortWidth() { return m_iViewPortWidth; };
	int &GetViewPortHeight() { return m_iViewPortHeight; };

	UINT_PTR m_unpTimer;

	void LoadModel(const char* szFname, const char* szPath);
	bool AddMD5Animation( const char* szFname );
	void SetMD5Animation(int index);
	void CloseModel();

protected:

	bool InitContext();					 // Creates OpenGL Rendering Context

	//----------------------------------------------------------------------
	// MFC Events
	afx_msg void OnSize(UINT nType, int cx, int cy);  // Changing viewport
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnLButtonDown(UINT flags, CPoint loc);
	afx_msg void OnLButtonUp(UINT flags, CPoint loc);
	afx_msg void OnMouseMove(UINT flags, CPoint loc);
	afx_msg BOOL OnMouseWheel(UINT flags, short zDelta, CPoint pt);
	afx_msg LRESULT OnMouseHover(WPARAM wparam, LPARAM lparam);
	afx_msg LRESULT OnMouseLeave(WPARAM wparam, LPARAM lparam);
	//----------------------------------------------------------------------

	HGLRC m_hrc;                        // OpenGL Rendering Context 
	HDC m_hdc;

	CRect m_rect;
	CRect m_WindowOld;
	CRect m_WindowOrig;

	int m_iWidth, m_iHeight;
	int	m_iViewPortWidth, m_iViewPortHeight;

	DECLARE_MESSAGE_MAP()

private:
	GLShaderFx m_globalFx;
	GLShaderFx m_gridShader;

	glm::mat4 m_projMatrix;
	GLuint m_WVPObject;
	GLuint m_GridObject;
	Camera m_camera;
	float m_elapsed;
	unsigned long m_lastTime;;

	// For camera-mouse movement
	bool m_mouseLBtnDown;
	CPoint m_mouseLoc;

	void Update(); 
	void DetectKeyInput();
	
	//------------------------------------------
	// Grid line related declaraitons
	GLuint grid_vba;
	GLuint grid_vbo;
	int grid_vert_count;
	void GenerateGrid(int w, int h);


};
