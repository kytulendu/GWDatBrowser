#include "stdafx.h"
#include "NiceSlider.h"

IMPLEMENT_DYNAMIC(CNiceSliderCtrl, CSliderCtrl)


BEGIN_MESSAGE_MAP(CNiceSliderCtrl, CSliderCtrl)
	//{{AFX_MSG_MAP(CNiceSliderCtrl)
	ON_WM_LBUTTONDOWN()
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONUP()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


void CNiceSliderCtrl::OnLButtonDown(UINT nFlags, CPoint point) 
{
	m_bDragging = true;
	m_bDragChanged = false;
	SetCapture();
	SetFocus();
	if (SetThumb(point))
	{
		m_bDragChanged = true;
		PostMessageToParent(TB_THUMBTRACK);
	}
}


void CNiceSliderCtrl::OnMouseMove(UINT nFlags, CPoint point) 
{
	if (m_bDragging)
	{
		if (SetThumb(point))
		{
			m_bDragChanged = true;
			PostMessageToParent(TB_THUMBTRACK);
		}
	}
	else
	{
		CSliderCtrl::OnMouseMove(nFlags, point);
	}
}

void CNiceSliderCtrl::OnLButtonUp(UINT nFlags, CPoint point) 
{
	if(m_bDragging)
	{
		m_bDragging = false;
		::ReleaseCapture();
		//if (SetThumb(point))
		{
			PostMessageToParent(TB_ENDTRACK);
			m_bDragChanged = true;
		}
		if (m_bDragChanged)
		{
			PostMessageToParent(TB_THUMBPOSITION);
			m_bDragChanged = false;
		}
	}
	else
	{
		CSliderCtrl::OnLButtonUp(nFlags, point);
	}
}


bool CNiceSliderCtrl::SetThumb(const CPoint& point)
{
	const int nMin = GetRangeMin();
	const int nMax = GetRangeMax()+1;
	CRect rc;
	GetChannelRect(rc);
	double dPos;
	double dCorrectionFactor = 0.0;
	if (GetStyle() & TBS_VERT) 
	{
		// note: there is a bug in GetChannelRect, it gets the orientation of the rectangle mixed up
		dPos = (double)(point.y - rc.left)/(rc.right - rc.left);
	}
	else
	{
		dPos = (double)(point.x - rc.left)/(rc.right - rc.left);
	}
	// This correction factor is needed when you click in between tick marks
	// so that the thumb will move to the nearest one
	dCorrectionFactor = 0.5 *(1-dPos) - 0.5 *dPos;
	int nNewPos = (int)(nMin + (nMax-nMin)*dPos + dCorrectionFactor);
	const bool bChanged = (nNewPos != GetPos());
	if(bChanged)
	{
		SetPos(nNewPos);
	}
	return bChanged;
}

void CNiceSliderCtrl::PostMessageToParent(const int nTBCode) const
{
	CWnd* pWnd = GetParent();
	if (pWnd)
	{
		if (GetStyle() & TBS_VERT)
			pWnd->PostMessage(WM_VSCROLL, (WPARAM)( nTBCode), (LPARAM)GetSafeHwnd());
		else
			pWnd->PostMessage(WM_HSCROLL, (WPARAM)(nTBCode), (LPARAM)GetSafeHwnd());
	}
}


