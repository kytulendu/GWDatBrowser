#ifndef NiceSlider_h
#define NiceSlider_h

// Class that provides better mouse click handling for the standard windows slider control
class CNiceSliderCtrl : public CSliderCtrl
{
public:
	DECLARE_DYNAMIC(CNiceSliderCtrl)
		CNiceSliderCtrl() : m_bDragging(false), m_bDragChanged(false) {}
protected:
	//{{AFX_MSG(CRoundSliderCtrl)
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
private:
	bool SetThumb(const CPoint& pt);
	void PostMessageToParent(const int nTBCode) const;
	bool m_bDragging;
	bool m_bDragChanged;
};

#endif // NiceSlider_h