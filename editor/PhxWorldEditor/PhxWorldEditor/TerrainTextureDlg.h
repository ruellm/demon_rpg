#pragma once

#include "resource.h"
#include "afxpanedialog.h"
#include "AdvanceTexDlg.h"
#include <string>
#include <vector>

// CTerrainTextureDlg dialog
#if 0
typedef struct TerrainTextureInfo
{
	CString fullPath;
	std::string fname;
	bool hasHeight;
	float min;
	float max;
} TerrainTextureInfo;
#endif

class CTerrainTextureDlg : public CPaneDialog
{
	//DECLARE_DYNAMIC(CTerrainTextureDlg)

public:
	CTerrainTextureDlg( );   // standard constructor
	virtual ~CTerrainTextureDlg();

// Dialog Data
	enum { IDD = IDD_TERRAIN_TEXTURE_DLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()

public:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnUpdateButton(CCmdUI *pCmdUI);
	afx_msg void OnBnClickedAddtex();
	afx_msg void OnBnClickedAdvtex();

	void InitControls();
	bool IsUnPaint();
	void Clear();
	CListBox* GetListCtrl();

	inline float GetAlpha() {
		return m_fAlpha;
	}

	inline float GetPaintRadius() {
		return m_fRadius;
	}
	inline int GetActiveTexture( ) {
		return m_aciveIdx;
	}
	
private:
	CListBox* m_textureList;
	CAdvanceTexDlg* m_pAdvanceDlg;
	float m_fRadius;
	int m_aciveIdx;
	float m_fAlpha;
	//std::vector<TerrainTextureInfo> m_infoList;
public:
	afx_msg void OnBnClickedUsetex();
	afx_msg void OnBnClickedDeltex();
	afx_msg void OnEnChangeEdit1();
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnEnChangeTexalpha();
};
