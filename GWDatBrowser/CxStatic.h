/*===========================================================================
====    Project : CxStatic (www.codeproject.com)                         ====
====    File name           :  CxStatic.h                                ====
====    Creation date       :  09/2004                                   ====
====    Author(s)           :  Vincent Richomme                          ====
====    Thanks to norm.net and Chen-Cha Hsu                              ====
===========================================================================*/

#ifndef CSTATICTEST_H
#define CSTATICTEST_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "MemDC.h"

typedef UINT (CALLBACK* LPFNDLLFUNC)(HDC,CONST PTRIVERTEX,DWORD,CONST PVOID,DWORD,DWORD);

class CxStatic : public CStatic
{
public:

	static enum ImageSize{ OriginalSize, FitControl};
	static enum LinkStyle { LinkNone, HyperLink, MailLink };
	static enum Type3D { Raised, Sunken};
	static enum BackFillMode { Normal, HGradient, VGradient };

	CxStatic();
	virtual ~CxStatic();
	
	BOOL	IsMoveable() { return m_bAllowMove; }
	void	SetMoveable(BOOL moveAble = TRUE); 
	void	BlinkBmp(UINT nIDBmp1, UINT nIDBmp2, DWORD dwTimeout);
	void	SetBkColor(COLORREF rgb, COLORREF crBkgndHigh = 0, BackFillMode mode = Normal);
	void	SetTransparent(BOOL bTranspMode);
	void	SetTextColor(COLORREF col);
	void	SetFont(const CString& strFont, int nPointSize,int nWeight, BOOL bRedraw = TRUE);
	void	SetFont(LOGFONT *pLogFont, BOOL bRedraw = TRUE);
	void	SetFont(CFont *pFont, BOOL bRedraw = TRUE);
	void	SetFont3D(BOOL bFont3D, Type3D type);
	
	void	SetAutoAdjustFont(BOOL bAutoAdjustFont);
	
	BOOL	SetBitmap(HBITMAP hBitmap, ImageSize Emode = FitControl, 
				COLORREF rgbTransparent =  0xFF000000);
	BOOL	SetBitmap(UINT nIDResource, ImageSize Emode = FitControl,
				COLORREF rgbTransparent =  0xFF000000);	// Loads bitmap from resource ID
	BOOL	SetBitmap(LPCTSTR lpszResName, ImageSize Emode = FitControl,
				COLORREF rgbTransparent = 0xFF000000);	// Loads bitmap from resource name

	void	SetRounded(BOOL bRounded);

	void	SetWindowText(LPCTSTR strText);

protected:
	CString			m_strText;
	BOOL			m_bNotifyParent;
	BOOL			m_bTransparentBk;
	BOOL			m_bAutoAdjustFont;
	BOOL			m_bAutoWrapping;
	BOOL			m_bRounded;
	COLORREF		m_rgbText;
	BackFillMode	m_fillmode;
	COLORREF		m_rgbBkgnd;
	COLORREF		m_crHiColor;
	COLORREF		m_crLoColor;
	COLORREF		m_cr3DHiliteColor;
	CBrush *		m_pBrush;
	CRect			m_rc;
	DWORD			m_dwTxtFlags;

	HINSTANCE		hinst_msimg32;
	LPFNDLLFUNC		dllfunc_GradientFill;

	// BITMAP ATTRIBUTES
	COLORREF		m_rgbTransparent;
	int				m_EDispMode;
	BOOL			m_bBitmap;
	int				m_nResourceID;
	HBITMAP			m_hBitmap;
    BITMAP			m_bmInfo;
    CString			m_strResourceName;

	// FONT ATTRIBUTES
	Type3D			m_3dType;
	BOOL			m_bFont3d;
	LOGFONT			m_lf;
	CFont			m_font;
	int				m_nFontSize;
	int				m_nFontSizeVar;
	CString			m_csFontFamilly;
	
	// MODIFIABLE ATTRIBUTES
	BOOL			m_bAllowMove;
	BOOL			m_bHover;
	BOOL			m_bTracking;
	CPoint			m_point;
	CRectTracker*	m_pTrack;

	// FONT METHODS
	void	ReconstructFont();
	int		GetFontPointSize(int nHeight);
	int		GetFontHeight(int nPointSize);
	
	// TEXT METHODS
	void	DrawText(CDC* pDC, CRect* pRect, CString csText);
	BOOL	IsASingleWord(const CString & csText);
	void	DrawGradientFill(CDC* pDC, CRect* pRect, BackFillMode enumFillMode);

	// IMAGE METHODS
	void	DrawBitmap(CDC* pDC, CRect* pRect);

	BOOL	RedrawWindow();

	// MESSAGE HANDLER
	virtual void PreSubclassWindow();
	virtual void DrawItem( LPDRAWITEMSTRUCT lpDrawItemStruct );

	//afx_msg void OnPaint();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg LRESULT	OnSetText(WPARAM wParam, LPARAM lParam); // WM_SETTEXT for UpdateData
	afx_msg LRESULT	OnSetFont(WPARAM wParam, LPARAM lParam); // WM_SETFONT
	afx_msg void	OnDropFiles(HDROP hDropInfo); // WM_DROPFILES
	afx_msg void	OnSize(UINT nType, int cx, int cy);
	//afx_msg HBRUSH CtlColor(CDC* pDC, UINT nCtlColor);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg LRESULT OnMouseLeave(WPARAM wparam, LPARAM lparam);
	afx_msg LRESULT OnMouseHover(WPARAM wparam, LPARAM lparam) ;
	afx_msg void OnRButtonDown( UINT nFlags, CPoint point );
	
	DECLARE_MESSAGE_MAP()
};

#endif // CXSTATIC_H
