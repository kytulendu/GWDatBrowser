/*===========================================================================
====                                                                     ====
====    File name           :  CxStatic.cpp                              ====
====    Creation date       :  09/2004                                   ====
====    Author(s)           :  Vincent Richomme                          ====
====    Thanks to norm.net and Chen-Cha Hsu                              ====
===========================================================================*/

#include "stdafx.h"
#include "CxStatic.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


// CxStatic
BEGIN_MESSAGE_MAP(CxStatic, CStatic)
	//ON_WM_PAINT()
	ON_WM_DRAWITEM_REFLECT()
	ON_WM_ERASEBKGND()
	ON_MESSAGE(WM_SETTEXT, OnSetText)
	ON_MESSAGE(WM_SETFONT, OnSetFont)
	ON_WM_MOUSEMOVE()
	//ON_MESSAGE(WM_DROPFILES)
	ON_WM_DROPFILES()
	ON_MESSAGE(WM_MOUSELEAVE, OnMouseLeave)
	ON_MESSAGE(WM_MOUSEHOVER, OnMouseHover)
	ON_WM_RBUTTONDOWN()
	ON_WM_SIZE()
	
	//ON_WM_CTLCOLOR_REFLECT()
END_MESSAGE_MAP()

///////////////////////////////////////////////////////////////////////////////
// ctor
CxStatic::CxStatic()
{
	hinst_msimg32	  = NULL;
	m_strText		  = _T("");
	m_bTransparentBk  = FALSE;
	m_bAutoWrapping	  = TRUE;
	m_bAutoAdjustFont = FALSE;
	m_bNotifyParent   =	FALSE;
	m_bRounded		  = TRUE;
	m_rgbText		  = ::GetSysColor(COLOR_WINDOWTEXT);
	m_rgbBkgnd		  = ::GetSysColor(COLOR_BTNFACE);
	m_cr3DHiliteColor =	RGB(0,0,255);
	m_rgbTransparent  = 0xFF000000;
	m_EDispMode		  = 0;
	m_pBrush          = new CBrush(m_rgbBkgnd);
	m_fillmode		  = Normal;
	m_crHiColor		  =	m_rgbBkgnd;
	m_crLoColor		  =	m_rgbBkgnd;
	m_nFontSize		  = m_nFontSizeVar = 8;
	m_csFontFamilly	  = _T("");
	m_bFont3d		  = FALSE;
	m_hBitmap		  = NULL;
	m_bBitmap		  = FALSE;
	m_nResourceID	  = -1;
	m_bHover		  = FALSE;
	m_bTracking		  = FALSE;
	m_bAllowMove	  = FALSE;
	m_dwTxtFlags	  = 0;

	m_strResourceName.Empty();

	// LIBRARY TO DRAW COLOR GRADIENTS
	hinst_msimg32 = LoadLibrary( L"msimg32.dll" );
	if( hinst_msimg32 ){
		dllfunc_GradientFill = ((LPFNDLLFUNC) GetProcAddress( hinst_msimg32, "GradientFill" ));
	}
}

CxStatic::~CxStatic()
{
	//TRACE(_T("in CxStatic::~CxStatic\n"));

	// Clean up
	if (m_pBrush){
		delete m_pBrush;
		m_pBrush = NULL;
	}
	if ( m_hBitmap )
		::DeleteObject(m_hBitmap);

	if ( hinst_msimg32 )
		::FreeLibrary( hinst_msimg32 );
}

/////////////////////////////////////////////////////////////////////////////////
//// PreSubclassWindow
void CxStatic::PreSubclassWindow()
{
	//TRACE(_T("in CxStatic::PreSubclassWindow\n"));
	
	m_dwTxtFlags = GetStyle();
	ModifyStyle(SS_TYPEMASK, SS_OWNERDRAW);
	// get current font
	CFont* pFont = GetFont();
	if (!pFont)
	{
		HFONT hFont = (HFONT)GetStockObject(DEFAULT_GUI_FONT);
		if (hFont == NULL)
			hFont = (HFONT) GetStockObject(ANSI_VAR_FONT);
		if (hFont)
			pFont = CFont::FromHandle(hFont);
	}
	ASSERT(pFont);
	ASSERT(pFont->GetSafeHandle());

	// create the font for this control
	LOGFONT lf;
	pFont->GetLogFont(&lf);
	m_font.CreateFontIndirect(&lf);

	this->GetWindowText(m_strText);

	DragAcceptFiles(TRUE);

	Invalidate();
}

void CxStatic::SetMoveable(BOOL moveAble)
{
	ModifyStyle(0, SS_NOTIFY);
	m_bAllowMove = moveAble;
}

///////////////////////////////////////////////////////////////////////////////
// SetBackgroundColor
void CxStatic::SetBkColor(COLORREF rgbBkgnd, COLORREF rgbBkgndHigh, BackFillMode mode)
{
	m_crLoColor = rgbBkgnd;
	m_crHiColor = rgbBkgndHigh;
	m_fillmode = mode;

	m_rgbBkgnd = rgbBkgnd;
	if (m_pBrush){
		delete m_pBrush;
		m_pBrush = new CBrush(m_rgbBkgnd);
	}
	else
		m_pBrush = new CBrush(m_rgbBkgnd);

	RedrawWindow();
}

/////////////////////////////////////////////////////////////////////////////////
//// SetTransparent
void CxStatic::SetTransparent(BOOL bTranspMode)
{
	m_bTransparentBk = bTranspMode;
	ModifyStyleEx(0,WS_EX_TRANSPARENT);
	RedrawWindow();
}

void CxStatic::SetTextColor(COLORREF col)
{
	m_rgbText = col;
	RedrawWindow();
}

void CxStatic::SetAutoAdjustFont(BOOL bAutoAdjustFont)
{
	m_bAutoAdjustFont = bAutoAdjustFont;
}

void CxStatic::SetRounded(BOOL bRounded){
	m_bRounded = bRounded;
}

/////////////////////////////////////////////////////////////////////////////////
//// FONT

//void CxStatic::ReconstructFont()
//{
//	m_pFont->DeleteObject();
//	BOOL bCreated = m_pFont->CreateFontIndirect(&m_lf);
//	ASSERT(bCreated);
//
//	RedrawWindow();
//}

void CxStatic::SetFont(const CString& strFont,int nPointSize, int nWeight,
					   BOOL bRedraw /*= TRUE*/)
{
	LOGFONT lf;
	memset(&lf, 0, sizeof(LOGFONT));       // Zero out the structure.

	_tcscpy(lf.lfFaceName,strFont);			// Set Font Familly
	lf.lfHeight = nPointSize;// Set Height
	lf.lfWeight = nWeight;					// Set Weight

	SetFont(&lf, bRedraw);

	//m_csFontFamilly = strFont;
	//m_nFontSizeVar = m_nFontSize = nPointSize;
}

void CxStatic::SetFont(LOGFONT *pLogFont, BOOL bRedraw /*= TRUE*/)
{
	ASSERT(pLogFont);
	if (!pLogFont)
		return;

	if (m_font.GetSafeHandle())
		m_font.DeleteObject();

	LOGFONT lf = *pLogFont;

	m_font.CreateFontIndirect(&lf);

	if (bRedraw)
		RedrawWindow();
}

void CxStatic::SetFont(CFont *pFont, BOOL bRedraw /*= TRUE*/)
{
	ASSERT(pFont);
	if (!pFont)
		return;

	LOGFONT lf;
	memset(&lf, 0, sizeof(lf));

	pFont->GetLogFont(&lf);

	SetFont(&lf, bRedraw);
}

LRESULT CxStatic::OnSetFont(WPARAM wParam, LPARAM lParam)
{
	LRESULT lrReturn(Default());

	SetFont(CFont::FromHandle((HFONT)wParam) );

	RedrawWindow();

	return lrReturn;
}


void CxStatic::SetFont3D(BOOL bFont3D, Type3D type)
{
	m_bFont3d = bFont3D;
	m_3dType = type;

	RedrawWindow();
}


LRESULT CxStatic::OnSetText(WPARAM wParam, LPARAM lParam)
{
	LRESULT lrReturn(Default());

	this->GetWindowText(m_strText);

	return lrReturn;
}

BOOL CxStatic::SetBitmap(HBITMAP hBitmap, ImageSize Emode, COLORREF rgbTransparent)
{
	m_bBitmap = TRUE;
	::DeleteObject(m_hBitmap);
	m_hBitmap = hBitmap;

	if (Emode == OriginalSize){
		CRect Rect;
		GetWindowRect(&Rect); // x,y -> screen
		ScreenToClient(&Rect); // screen -> to client ( view ou dialog)
		Rect.InflateRect(Rect.Width(),Rect.Height());
		SetWindowPos( NULL,0,0,Rect.Width(),Rect.Height(),SWP_NOMOVE | SWP_NOZORDER); //
	}

	Invalidate();

	return ::GetObject(m_hBitmap, sizeof(BITMAP), &m_bmInfo);
}

BOOL CxStatic::SetBitmap(UINT nIDResource, ImageSize Emode, COLORREF rgbTransparent)
{
	m_nResourceID = nIDResource;
	m_strResourceName.Empty();
	m_bTransparentBk = FALSE;

	HBITMAP hBmp = (HBITMAP)::LoadImage(AfxGetInstanceHandle(),
		MAKEINTRESOURCE(nIDResource),
		IMAGE_BITMAP,
		0,0,
		LR_LOADMAP3DCOLORS);
	if (!hBmp) return FALSE;
	return CxStatic::SetBitmap(hBmp, Emode, rgbTransparent);
}

BOOL CxStatic::SetBitmap(LPCTSTR lpszResourceName, ImageSize Emode, COLORREF rgbTransparent)
{
	m_nResourceID = -1;
	m_strResourceName = lpszResourceName;
	
	HBITMAP hBmp = (HBITMAP)::LoadImage(AfxGetInstanceHandle(),
		lpszResourceName,
		IMAGE_BITMAP,
		0,0,
		LR_LOADFROMFILE);
	if (!hBmp) return FALSE;
	return CxStatic::SetBitmap(hBmp, Emode, rgbTransparent);
}



//void CxStatic::BlinkBmp(UINT nIDBmp1, UINT nIDBmp2, UINT Timeout)
//{
//	SetTimer(0, Timeout);
//
//
//
//}

///////////////////////////////////////////////////////////////////////////////
// GetFontPointSize()
int CxStatic::GetFontPointSize(int nHeight)
{
	HDC hdc = ::CreateDC(_T("DISPLAY"), NULL, NULL, NULL);
	ASSERT(hdc);
	int cyPixelsPerInch = ::GetDeviceCaps(hdc, LOGPIXELSY);
	::DeleteDC(hdc);

	int nPointSize = MulDiv(nHeight, 72, cyPixelsPerInch);
	if (nPointSize < 0)
		nPointSize = -nPointSize;

	return nPointSize;
}

///////////////////////////////////////////////////////////////////////////////
// GetFontHeight()
int CxStatic::GetFontHeight(int nPointSize)
{
	HDC hdc = ::CreateDC(_T("DISPLAY"), NULL, NULL, NULL);
	ASSERT(hdc);
	int cyPixelsPerInch = ::GetDeviceCaps(hdc, LOGPIXELSY);
	::DeleteDC(hdc);

	int nHeight = -MulDiv(nPointSize, cyPixelsPerInch, 72);

	return nHeight;
}

void CxStatic::SetWindowText( LPCTSTR strText)
{
	m_strText = strText;

	CRect Rect;
	GetClientRect(&Rect);

	if ( m_bTransparentBk ){
		ClientToScreen(&Rect);
		Rect.InflateRect(1,1,1,1);
		CWnd *Parent = GetParent();
		Parent->ScreenToClient(&Rect);
		Parent->InvalidateRect(&Rect);
		Parent->UpdateWindow();
		//SendMessage(WM_ERASEBKGND);
	}
	else
		RedrawWindow();
}

BOOL CxStatic::RedrawWindow()
{
	Invalidate();
	return TRUE;
}

void CxStatic::DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct)
{
	ASSERT(lpDrawItemStruct != NULL);

	CBitmap		bmp;
	CBitmap*	pOldBitmap = NULL;

	CDC*  pDC     = CDC::FromHandle(lpDrawItemStruct->hDC);
    CRect rect    = lpDrawItemStruct->rcItem;

	//Double Buffering - Modify MemDC for transparent background but doesn't work
	// because I have to invalidate parent when SetWindowText - Need to find a fix!!!
	pDC->SetBkColor(m_rgbBkgnd);
	CMemDC_KR MemDC(pDC, &rect/*, m_bTransparentBk*/);


	// PAINT SOLID BACKGROUND IF NO BITMAP
	if ( !m_bBitmap){
		if ( !m_bTransparentBk ){
			if (m_fillmode == Normal)
				MemDC.FillRect(&rect, m_pBrush);
			else
				DrawGradientFill(&MemDC, &rect, m_fillmode);
		}
	}
	else{// DISPLAY BACKGROUND BITMAP
		DrawBitmap(&MemDC, &rect);
	}

	// TEXT RENDERING
	CFont* pOldFont  = MemDC.SelectObject( &m_font );
	COLORREF rgbText = MemDC.SetTextColor(m_rgbText);
	DrawText(&MemDC, &rect, m_strText);


	// Restore DC's State
	MemDC.SelectObject(pOldFont);
	MemDC.SetTextColor(rgbText);
}

//void CxStatic::OnPaint()
//{
//	CPaintDC	dc(this); // device context for painting
//	CBitmap		bmp;
//	CBitmap*	pOldBitmap = NULL;
//
//	// GET Client area
//	CRect rc;
//	GetClientRect(rc);
//	m_rc = rc;
//
//	//Double Buffering - Modify MemDC for transparent background but doesn't work
//	// because I have to invalidate parent when SetWindowText - Need to find a fix!!!
//	dc.SetBkColor(m_rgbBkgnd);
//	CMemDC_KR MemDC(&dc, &rc, m_bTransparentBk);
//
//	// ROUNDED_RECT
//	/*if (m_bRounded){
//	CBrush *oldbrush;
//	CPoint roundpoint(30,30);
//
//	oldbrush = MemDC.SelectObject( m_pBrush );
//	MemDC.RoundRect(&rc, roundpoint);
//	MemDC.SelectObject(oldbrush);
//
//	}*/
//
//	// PAINT SOLID BACKGROUND IF NO BITMAP
//	if ( !m_bBitmap){
//		if ( !m_bTransparentBk ){
//			if (m_fillmode == Normal)
//				MemDC.FillRect(&rc, m_pBrush);
//			else
//				DrawGradientFill(&MemDC, &rc, m_fillmode);
//		}
//	}
//	else{// DISPLAY BACKGROUND BITMAP
//		DrawBitmap(&MemDC, &rc);
//	}
//
//
//	// TEXT RENDERING
//	CFont* pOldFont  = MemDC.SelectObject( &m_font );
//	COLORREF rgbText = MemDC.SetTextColor(m_rgbText);
//	DrawText(&MemDC, &rc, m_strText);
//
//
//	// Restore DC's State
//	MemDC.SelectObject(pOldFont);
//	MemDC.SetTextColor(rgbText);
//}



void CxStatic::DrawText(CDC* pDCMem, CRect* pRect, CString csText)
{
	DWORD dwStyle = m_dwTxtFlags;
	DWORD dwFlags = 0;

	// Map "Static Styles" to "Text Styles" - WARNING MACROS
#define MAP_STYLE(src, dest) if(dwStyle & (src)) dwFlags |= (dest)
#define NMAP_STYLE(src, dest) if(!(dwStyle & (src))) dwFlags |= (dest)

	MAP_STYLE(	SS_RIGHT,			DT_RIGHT   					);
	MAP_STYLE(	SS_CENTER,			DT_CENTER					);
	MAP_STYLE(	SS_LEFT,			DT_LEFT					    );
	//MAP_STYLE(	SS_CENTERIMAGE,		DT_VCENTER | DT_SINGLELINE	);
	MAP_STYLE(	SS_NOPREFIX,		DT_NOPREFIX					);
	MAP_STYLE(	SS_WORDELLIPSIS,	DT_WORD_ELLIPSIS			);
	MAP_STYLE(	SS_ENDELLIPSIS,		DT_END_ELLIPSIS				);
	MAP_STYLE(	SS_PATHELLIPSIS,	DT_PATH_ELLIPSIS			);

	// TAb expansion
	if (csText.Find( _T('\t') ) != -1)
		dwFlags |= DT_EXPANDTABS;

	//csText.Replace(
	// TEXT WRAPPING - Inspired by Chen-Cha Hsu and improved
	CRect		newRC;
	TEXTMETRIC	tag;
	CSize		sz;
	::GetTextExtentPoint32(pDCMem->GetSafeHdc(), csText,csText.GetLength(), &sz);
	CStringArray	asText;
	int				nLine = 0;
	CString			s2;
	int				nX = 0;
	int				nId = 0;
	TCHAR			nCR = 0;

	// Autowrapping mode enabled
	if ( m_bAutoWrapping ){
		for (int i = 1; i <= csText.GetLength(); i++){
			s2 = csText.Left(i);
			//CARRIAGE RETURN not recognised with SS_CENTERIMAGE - manual handling
			if (csText.GetAt(i-1) == '\r' || csText.GetAt(i-1) == '\n'){
				if (nCR == 0){
					nCR = csText.GetAt(i-1);
				}
				else if (nCR != csText.GetAt(i-1)){ // "\r\n" or "\n\r"
					s2 = csText.Left(i-2);
					asText.Add(s2);
					csText = csText.Mid(i);
					i = nCR = 0;
				}
			}
			::GetTextExtentPoint32(pDCMem->GetSafeHdc(), s2, s2.GetLength(), &sz);
			if ( sz.cx > pRect->Width() ){// We found how many letters fits
				s2 = csText.Left(i-1);
				if ( IsASingleWord(s2) ){
					asText.Add(s2);
					csText = csText.Mid(i-1);
					i = 0;
				}
				else{ // Do not break a word
					nId = s2.ReverseFind(' ');
					s2 = s2.Left(nId);
					asText.Add(s2);
					csText = csText.Mid(nId + 1);
					i = 0;
				}
			}
		}
		if ( ! csText.IsEmpty() )
			asText.Add(csText);
	}
	else{// Standard CStatic behaviour
		asText.SetAtGrow(0, csText);
	}

	nLine = (int)asText.GetSize();
	pDCMem->GetTextMetrics(&tag);
	newRC = *pRect;

	LONG nDiff = pRect->bottom - pRect->top - tag.tmHeight * nLine;
	if (dwStyle & SS_CENTERIMAGE)
		pRect->top = nDiff/2;

	//TRACE( "The value of nDiff is %d\n", nDiff );
	if (m_bAutoAdjustFont){
		if (nDiff < 0){
			m_nFontSizeVar--;
			SetFont( m_lf.lfFaceName, m_nFontSizeVar, m_lf.lfWeight );
		}
		//pDCMem->SelectObject( m_pFont ); TODO CHECK WITH AUTOADJUST
		//RedrawWindow();
	}
	for (int j = 0; j < nLine; j++){
		newRC.top = pRect->top + tag.tmHeight * j;
		pDCMem->DrawText(asText[j], &newRC,dwFlags);
		if (m_bFont3d){
			if (m_3dType == Raised)
				newRC.OffsetRect(-1,-1);
			else
				newRC.OffsetRect(1,1);
			pDCMem->DrawText(asText[j], &newRC,dwFlags);
		}
	}
}


BOOL CxStatic::IsASingleWord(const CString & csText)
{
	TCHAR	cEnd = 0;

	cEnd = csText.GetAt(csText.GetLength() - 1);
	if ( ( csText.Find(L" ") == -1 ) || (cEnd == ' ') )
		return TRUE;
	else
		return FALSE;
}

void CxStatic::DrawBitmap(CDC* pDCMem, CRect* pRect)
{
	CDC dcMem;
	CRect rect;
    GetClientRect( rect );

	CRect child_rect;
	GetWindowRect( child_rect );

	CRect parent_rect;
	GetParent()->GetWindowRect(parent_rect);

	if (!m_hBitmap)
		return;

	VERIFY( dcMem.CreateCompatibleDC(pDCMem) );
	m_bTransparentBk = FALSE;

	// Select bitmap into memory DC.
	HBITMAP* pBmpOld = (HBITMAP*) ::SelectObject(dcMem.m_hDC, m_hBitmap);


	pDCMem->StretchBlt(rect.left, rect.top, rect.Width(), rect.Height(),
		&dcMem, 0, 0, m_bmInfo.bmWidth-1, m_bmInfo.bmHeight-1,
		SRCCOPY);
}



void CxStatic::DrawGradientFill(CDC* pDCMem, CRect* pRect, BackFillMode FillMode)
{
	TRIVERTEX rcVertex[2];
	rcVertex[0].x=pRect->left;
	rcVertex[0].y=pRect->top;
	rcVertex[0].Red=GetRValue(m_crLoColor)<<8;
	rcVertex[0].Green=GetGValue(m_crLoColor)<<8;
	rcVertex[0].Blue=GetBValue(m_crLoColor)<<8;
	rcVertex[0].Alpha=0x0000;
	rcVertex[1].x=pRect->right;
	rcVertex[1].y=pRect->bottom;
	rcVertex[1].Red=GetRValue(m_crHiColor)<<8;
	rcVertex[1].Green=GetGValue(m_crHiColor)<<8;
	rcVertex[1].Blue=GetBValue(m_crHiColor)<<8;
	rcVertex[1].Alpha=0;

	GRADIENT_RECT grect;
	grect.UpperLeft=0;
	grect.LowerRight=1;

	dllfunc_GradientFill( *pDCMem ,rcVertex,2,&grect,1,
		FillMode == HGradient ? GRADIENT_FILL_RECT_H :  GRADIENT_FILL_RECT_V);
}


///////////////////////////////////////////////////////////////////////////////
// OnEraseBkgnd
BOOL CxStatic::OnEraseBkgnd(CDC* pDC)
{
	return TRUE;  //return CStatic::OnEraseBkgnd(pDC);
}

void CxStatic::OnSize(UINT nType, int cx, int cy)
{
  CStatic::OnSize(nType, cx, cy);
  Invalidate();
}


//HBRUSH CxStatic::CtlColor(CDC* pDC, UINT nCtlColor)
//{
//	pDC->SetBkMode(TRANSPARENT);
//	HBRUSH brush = (HBRUSH)GetStockObject(HOLLOW_BRUSH);
//	return brush;
//}

//Credits goes to Aquiruse http://www.thecodeproject.com/buttonctrl/hoverbuttonex.asp
void CxStatic::OnMouseMove(UINT nFlags, CPoint point)
{
	TRACE("OnMouseMove : %d %d\r\n", point.x, point.y);

	if (!m_bTracking){
		/*TRACKMOUSEEVENT tme;
		tme.cbSize = sizeof(tme);
		tme.hwndTrack = m_hWnd;
		tme.dwFlags = TME_LEAVE|TME_HOVER;
		tme.dwHoverTime = 1;
		m_bTracking = _TrackMouseEvent(&tme);
		m_point = point;*/
	}
	CStatic::OnMouseMove(nFlags, point);
}

LRESULT CxStatic::OnMouseHover(WPARAM wparam, LPARAM lparam)
{
	m_bHover=TRUE;
	::SetActiveWindow(GetParent()->GetSafeHwnd());
	Invalidate();

	return 0;
}

LRESULT CxStatic::OnMouseLeave(WPARAM wparam, LPARAM lparam)
{
	m_bTracking = FALSE;
	m_bHover=FALSE;
	Invalidate();

	return 0;
}

void CxStatic::OnDropFiles(HDROP hDropInfo)
{
	int		nFiles = 0;
	TCHAR	szFile[MAX_PATH];
	TCHAR*	ext;


	nFiles = DragQueryFile(hDropInfo, 0xFFFFFFFF, NULL, 0);

	if (nFiles != 1)
		return;

	DragQueryFile(hDropInfo, 0, szFile, sizeof(szFile));
	if (ext = _tcsrchr(szFile, '.'))	{
		if (_tcsicmp(ext, _T(".bmp")) == 0){
			SetBitmap(szFile);
		}
	}

	DragFinish(hDropInfo);

}

void CxStatic::OnRButtonDown(UINT nFlags, CPoint point)
{
	if (IsMoveable() == TRUE)
	{
		m_point = point;
		CRect rect;
		GetWindowRect( rect );
		ScreenToClient( rect );
		m_pTrack = new CRectTracker(&rect, CRectTracker::dottedLine |
								CRectTracker::resizeInside |
								CRectTracker::hatchedBorder);
		// RIGHT-CLICK + CONTROL
		if (nFlags & MK_CONTROL){
			GetWindowRect(rect);
			GetParent()->ScreenToClient(rect);
			m_pTrack->TrackRubberBand(GetParent(), rect.TopLeft());
			m_pTrack->m_rect.NormalizeRect();
		}
		else {
			m_pTrack->Track(GetParent(), point);
		}
		rect = LPRECT(m_pTrack->m_rect);
		MoveWindow(&rect,TRUE);//Move Window to our new position
		
		// Clean our mess
		if (m_pTrack)
			delete m_pTrack;

		rect = NULL;
	}

	CStatic::OnRButtonDown(nFlags, point);
}

