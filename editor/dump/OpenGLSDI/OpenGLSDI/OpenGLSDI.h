
// OpenGLSDI.h : main header file for the OpenGLSDI application
//
#pragma once

#ifndef __AFXWIN_H__
	#error "include 'stdafx.h' before including this file for PCH"
#endif

#include "resource.h"       // main symbols


// COpenGLSDIApp:
// See OpenGLSDI.cpp for the implementation of this class
//

class COpenGLSDIApp : public CWinApp
{
public:
	COpenGLSDIApp();


// Overrides
public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();

// Implementation
	afx_msg void OnAppAbout();
	DECLARE_MESSAGE_MAP()
};

extern COpenGLSDIApp theApp;
