#pragma once

enum BACKGROUND {WHITE, BLACK, GREY, CHECK, NONE};

// CImagePrieviewStatic
class CImagePreviewStatic :	public CWnd
{
public:
					CImagePreviewStatic();
	virtual			~CImagePreviewStatic();

	//Create
	virtual BOOL	Create(const RECT& rect, CWnd* pParentWnd, UINT nID, DWORD dwStyle = WS_CHILD | WS_BORDER | WS_TABSTOP | WS_VISIBLE);

	void			SetImage(Image* image);
	void			setBackground(int back);

protected:
	Image			*m_img;

	SolidBrush*		grey;
	SolidBrush*		black;
	SolidBrush*		white;
	TextureBrush*	check;

	Brush*			background;

	CScrollBar		scrollBarV;
	CScrollBar		scrollBarH;


	int				imageX;
	int				imageY;

	int				width;
	int				height;

	BOOL RegisterWindowClass();

	afx_msg void OnPaint();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);

	DECLARE_MESSAGE_MAP()
};


