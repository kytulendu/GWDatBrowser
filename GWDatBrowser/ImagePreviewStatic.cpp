#include "stdafx.h"
#include "ImagePreviewStatic.h"

// CImagePrieviewStatic

CImagePreviewStatic::CImagePreviewStatic()
{
	m_img = (Image *) NULL;

	black = new SolidBrush(Color(0,0,0));
	white = new SolidBrush (Color(255,255,255));
	grey = new SolidBrush(Color(128, 128, 128));

	SolidBrush lightgrey(Color(200,200,200));

	Bitmap b(16, 16);
	Graphics g(&b);

	g.FillRectangle(&lightgrey, 0, 0, 16 , 16);
	g.FillRectangle(white, 0, 0, 8, 8);
	g.FillRectangle(white, 8, 8, 16, 16);

	check = new TextureBrush (&b);

	background = check;

	imageX = 0;
	imageY = 0;

	RegisterWindowClass();
}

BOOL CImagePreviewStatic::RegisterWindowClass()
{
	WNDCLASS wndcls;
	HINSTANCE hInst = AfxGetInstanceHandle();

	if (!(::GetClassInfo(hInst, L"ImgPrvClass", &wndcls)))
	{
		// otherwise we need to register a new class
		wndcls.style            = CS_DBLCLKS | CS_HREDRAW | CS_VREDRAW;
		wndcls.lpfnWndProc      = ::DefWindowProc;
		wndcls.cbClsExtra       = wndcls.cbWndExtra = 0;
		wndcls.hInstance        = hInst;
		wndcls.hIcon            = NULL;
		wndcls.hCursor          = AfxGetApp()->LoadStandardCursor(IDC_ARROW);
		wndcls.hbrBackground    = (HBRUSH) (COLOR_3DFACE + 1);
		wndcls.lpszMenuName     = NULL;
		wndcls.lpszClassName    = L"ImgPrvClass";

		if (!AfxRegisterClass(&wndcls))
		{
			AfxThrowResourceException();
			return FALSE;
		}
	}

	return TRUE;
}

CImagePreviewStatic::~CImagePreviewStatic()
{
	delete black;
	delete white;
	delete grey;
	delete check;
}

BOOL CImagePreviewStatic::Create(const RECT& rect, CWnd* pParentWnd, UINT nID, DWORD dwStyle /* = WS_CHILD | WS_BORDER | WS_TABSTOP | WS_VISIBLE */) 
{
	ASSERT(pParentWnd->GetSafeHwnd());

	if (!CWnd::Create(L"ImgPrvClass", NULL, dwStyle, rect, pParentWnd, nID))
		return FALSE;

	scrollBarV.Create (WS_CHILD | WS_VISIBLE | SBS_VERT | SBS_RIGHTALIGN, CRect (0, 0, 100, 100), this, 100);
	scrollBarH.Create (WS_CHILD | WS_VISIBLE | SBS_HORZ | SBS_BOTTOMALIGN, CRect (0, 0, 100, 100), this, 100);

	return TRUE;
}

void CImagePreviewStatic::SetImage(Image* image)
{
	m_img =	image;
	Invalidate();
}

BEGIN_MESSAGE_MAP(CImagePreviewStatic, CWnd)
	ON_WM_PAINT()
	ON_WM_ERASEBKGND()
	ON_WM_VSCROLL()
	ON_WM_HSCROLL()
END_MESSAGE_MAP()

void CImagePreviewStatic::setBackground( int back)
{
	switch(back)
	{
	case WHITE:
		background = white;
		break;
	case GREY:
		background = grey;
		break;
	case BLACK:
		background = black;
	    break;
	case CHECK:
		background = check;
	    break;
	default:
		background = NULL;
	    break;
	}

	Invalidate();
}

void CImagePreviewStatic::OnPaint()
{
	CPaintDC dc(this); // device context for painting
	CRect rect;

	if (m_img != NULL)
	{
		GetClientRect(&rect);

		int scrollBarWidth = ::GetSystemMetrics(SM_CXVSCROLL);
		int scrollBarHeight = ::GetSystemMetrics(SM_CXHSCROLL);

		Graphics graphics(GetSafeHwnd());
		Color c;
		c.SetFromCOLORREF(::GetSysColor(COLOR_BTNFACE));
		SolidBrush systemColor(c);

		if ((unsigned int) rect.Height() >= m_img->GetHeight() && (unsigned int)rect.Width() >= m_img->GetWidth())
		{
			scrollBarV.ShowScrollBar(FALSE);
			scrollBarH.ShowScrollBar(FALSE);
		}
		else if ((unsigned int)rect.Height() < m_img->GetHeight() && (unsigned int)rect.Width() >= m_img->GetWidth() + scrollBarWidth)
		{
			scrollBarV.ShowScrollBar();
			scrollBarH.ShowScrollBar(FALSE);
			CRect ScrollRect = rect;
			ScrollRect.left = ScrollRect.right - scrollBarWidth;
			scrollBarV.MoveWindow(ScrollRect);
			rect.right -=scrollBarWidth;
		}
		else if ((unsigned int)rect.Width() < m_img->GetWidth() && (unsigned int)rect.Height() >= m_img->GetHeight() + scrollBarHeight)
		{
			scrollBarH.ShowScrollBar();
			scrollBarV.ShowScrollBar(FALSE);
			CRect ScrollRect = rect;
			ScrollRect.top = ScrollRect.bottom - scrollBarHeight;
			scrollBarH.MoveWindow(ScrollRect);
			rect.bottom -=scrollBarHeight;
		}
		else
		{
			scrollBarV.ShowScrollBar();
			CRect ScrollRect = rect;
			ScrollRect.left = ScrollRect.right - scrollBarWidth;
			ScrollRect.bottom -= scrollBarHeight;
			scrollBarV.MoveWindow(ScrollRect);
			rect.right -=scrollBarWidth;

			scrollBarH.ShowScrollBar();
			ScrollRect = rect;
			ScrollRect.top = ScrollRect.bottom - scrollBarHeight;
			scrollBarH.MoveWindow(ScrollRect);
			rect.bottom -= scrollBarHeight;

			//Have to paint over bottom right corner
			graphics.FillRectangle(&systemColor, rect. right, rect.bottom, scrollBarWidth, scrollBarHeight);
		}

		width = rect.Width();
		height = rect.Height();

		if (imageY > m_img->GetHeight() - rect.Height())
			imageY = m_img->GetHeight() - rect.Height();

		SCROLLINFO info;
		info.cbSize = sizeof(SCROLLINFO);     
		info.fMask = SIF_ALL;     
		info.nMin = 0;     
		info.nMax = m_img->GetHeight(); 
		info.nPage = rect.Height();     
		info.nPos = imageY;    
		scrollBarV.SetScrollInfo(&info);

		if (imageX > m_img->GetWidth() - rect.Width())
			imageX = m_img->GetWidth() - rect.Width();

		info.nMax = m_img->GetWidth(); 
		info.nPage = rect.Width();     
		info.nPos = imageX;    
		scrollBarH.SetScrollInfo(&info);
		
		Bitmap backBuffer(rect.Width(), rect.Height());
		Graphics backBufferG(&backBuffer);
		backBufferG.FillRectangle(&systemColor, 0 , 0, rect.Width(), rect.Height());
		
		if (background)
			backBufferG.FillRectangle(background, rect.left, rect.top, m_img->GetWidth(), m_img->GetHeight());
		
		backBufferG.DrawImage(m_img, rect.left, rect.top, imageX, imageY, rect.Width(), rect.Height(), UnitPixel);
		graphics.DrawImage(&backBuffer, 0 ,0);
	}
}

BOOL CImagePreviewStatic::OnEraseBkgnd( CDC* pDC )
{
	return TRUE;
}

void CImagePreviewStatic::OnVScroll( UINT nSBCode, UINT nPos, CScrollBar* pScrollBar )
{
	// TODO: Add your message handler code here and/or call default
	int nMax;
	int nMin;
	int nNow = scrollBarV.GetScrollPos();
	scrollBarV.GetScrollRange(&nMin, &nMax);
	switch (nSBCode)
	{
	case SB_TOP: 
		nNow = nMin; 
		break;
	case SB_BOTTOM: 
		nNow = nMax; 
		break;
	case SB_LINEDOWN: 
		nNow ++; 
		break;
	case SB_LINEUP: 
		nNow --; 
		break;
	case SB_PAGEDOWN: 
		nNow += height;
		break;
	case SB_PAGEUP: 
		nNow -= height;
		break;
		//	case SB_THUMBPOSITION: 
	case SB_THUMBTRACK: 
		//Added 
		SCROLLINFO si;
		ZeroMemory (&si, sizeof(SCROLLINFO));
		si.cbSize = sizeof(SCROLLINFO);
		si.fMask = SIF_TRACKPOS;
		// Call GetScrollInfo to get current tracking 
		// position in si.nTrackPos
		if (!scrollBarV.GetScrollInfo (&si, SIF_TRACKPOS))
			return; // GetScrollInfo failed
		nNow = si.nTrackPos ;
		break;
	}

	if (nNow > nMax)
		nNow = nMax;
	else if (nNow < nMin)
		nNow = nMin;

	scrollBarV.SetScrollPos(nNow, true);

	if (nNow != imageY)
	{
		imageY = nNow;
		Invalidate(FALSE);
	}

	CWnd::OnVScroll(nSBCode, nPos, pScrollBar);
}

void CImagePreviewStatic::OnHScroll( UINT nSBCode, UINT nPos, CScrollBar* pScrollBar )
{
	// TODO: Add your message handler code here and/or call default
	int nMax;
	int nMin;
	int nNow = scrollBarH.GetScrollPos();
	scrollBarH.GetScrollRange(&nMin, &nMax);
	switch (nSBCode)
	{
	case SB_TOP: 
		nNow = nMin; 
		break;
	case SB_BOTTOM: 
		nNow = nMax; 
		break;
	case SB_LINEDOWN: 
		nNow ++; 
		break;
	case SB_LINEUP: 
		nNow --; 
		break;
	case SB_PAGEDOWN: 
		nNow += width;
		break;
	case SB_PAGEUP: 
		nNow -= width; 
		break;
		//	case SB_THUMBPOSITION: 
	case SB_THUMBTRACK: 
		//Added 
		SCROLLINFO si;
		ZeroMemory (&si, sizeof(SCROLLINFO));
		si.cbSize = sizeof(SCROLLINFO);
		si.fMask = SIF_TRACKPOS;
		// Call GetScrollInfo to get current tracking 
		// position in si.nTrackPos
		if (!scrollBarH.GetScrollInfo (&si, SIF_TRACKPOS))
			return; // GetScrollInfo failed
		nNow = si.nTrackPos ;
		break;
	}

	if (nNow > nMax)
		nNow = nMax;
	else if (nNow < nMin)
		nNow = nMin;

	scrollBarH.SetScrollPos(nNow, true);

	if (nNow != imageX)
	{
		imageX = nNow;
		Invalidate(FALSE);
	}

	CWnd::OnHScroll(nSBCode, nPos, pScrollBar);
}
// CImagePrieviewStatic message handlers
