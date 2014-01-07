#if !defined(AFX_PPDUMPCTRL_H__33EAE942_55D0_11D6_98A4_00C026A7402A__INCLUDED_)
#define AFX_PPDUMPCTRL_H__33EAE942_55D0_11D6_98A4_00C026A7402A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// PPDumpCtrl.h : header file
//
#include "PPNumEdit.h"
#include <afxtempl.h>

#define PPDUMPCTRL_CLASSNAME    _T("MFCDumpCtrl")  // Window class name


#define UDM_PPDUMPCTRL_FIRST		 (WM_USER + 100)
#define UDM_PPDUMPCTRL_CHANGE_DATA	 (UDM_PPDUMPCTRL_FIRST) //User was changed the data
#define UDM_PPDUMPCTRL_BEGIN_ADDR	 (UDM_PPDUMPCTRL_FIRST + 1) //User was changed the first address on the screen
#define UDM_PPDUMPCTRL_MENU_CALLBACK (UDM_PPDUMPCTRL_FIRST + 2) //User want displaying the menu
#define UDM_PPDUMPCTRL_SELECTION	 (UDM_PPDUMPCTRL_FIRST + 3) //User want displaying the menu

//Flags of the style

//The available fields of the data
#define PPDUMP_FIELD_ADDRESS	0x00000001	// Address field is exist
#define PPDUMP_FIELD_HEX		0x00000002	// DEC field is exist
#define PPDUMP_FIELD_DEC    	0x00000004	// HEX field is exist
#define PPDUMP_FIELD_BIN    	0x00000008	// BIN field is exist
#define PPDUMP_FIELD_OCT		0x00000010	// OCT field is exist
#define PPDUMP_FIELD_ASCII  	0x00000020	// ASCII field is exist
//------------------------------------------------------------
#define PPDUMP_FIELD_ALL		0x0000003F	// All fields is exist

//The available fields of the control bar
#define PPDUMP_BAR_ADDRESS		0x00000100	// Address field on the bar is exist
#define PPDUMP_BAR_HEX			0x00000200	// DEC field on the bar is exist
#define PPDUMP_BAR_DEC    		0x00000400	// HEX field on the bar is exist
#define PPDUMP_BAR_BIN    		0x00000800	// BIN field on the bar is exist
#define PPDUMP_BAR_OCT			0x00001000	// OCT field on the bar is exist
#define PPDUMP_BAR_ASCII  		0x00002000	// ASCII field on the bar is exist
//------------------------------------------------------------
#define PPDUMP_BAR_ALL			0x00003F00	// The control bar have all fields 

//The additional styles
#define PPDUMP_SEPARATOR_LINES	0x00010000	// The separators is drawn
#define PPDUMP_WORD_DATA      	0x00020000	// 16-bit data
#define PPDUMP_NAMED_FIELDS   	0x00040000	// The names of the fields of the data is drawn
#define PPDUMP_READ_ONLY      	0x00080000	// Data for read only
#define PPDUMP_SELECTING_DATA  	0x00100000	// Enable selecting data
#define PPDUMP_DATA_LOW_HIGH	0x00200000  // The direction of the data in word data (High->Low or Low->High)
#define PPDUMP_TRACK_MOUSE_MOVE	0x00400000  // Enables draw the track rectangle around the mouse

// This structure sent to PPDumpCtrl parent in a WM_NOTIFY message
typedef struct tagNM_PPDUMP_CTRL {
    NMHDR hdr;
	int   iAddress;
    UINT  iValue;
} NM_PPDUMP_CTRL;

// This structure sent to PPDumpCtrl parent in a WM_NOTIFY message
typedef struct tagNM_PPDUMP_SEL {
    NMHDR hdr;
	int   iFirstAddr;
    int   iLastAddr;
} NM_PPDUMP_SEL;

// This structure sent to PPDumpCtrl parent in a WM_NOTIFY message
typedef struct tagNM_PPDUMP_MENU {
    NMHDR hdr;
	HMENU hMenu;
    int   iArea;
	int   iAddress;
} NM_PPDUMP_MENU;

/////////////////////////////////////////////////////////////////////////////
// CPPDumpCtrl window

class CPPDumpCtrl : public CWnd
{
// Construction
public:
	enum {	PPDUMP_BAR_AREA_ADDRESS = 0, //The index of the fields
			PPDUMP_BAR_AREA_HEX,
			PPDUMP_BAR_AREA_DEC,
			PPDUMP_BAR_AREA_BIN,
			PPDUMP_BAR_AREA_OCT,
			PPDUMP_BAR_AREA_ASCII,

			PPDUMP_BAR_MAX_AREAS
		};

	enum {	PPDUMP_COLOR_DATA_FG = 0, //The index of the colors
			PPDUMP_COLOR_DATA_BK,
			PPDUMP_COLOR_DATA_CHANGE_FG,
			PPDUMP_COLOR_EDIT_FG,
			PPDUMP_COLOR_EDIT_BK,
			PPDUMP_COLOR_EDIT_ERR_FG,
			PPDUMP_COLOR_EDIT_ERR_BK,
			PPDUMP_COLOR_CARET_BK,
			PPDUMP_COLOR_ADDRESS_FG,
			PPDUMP_COLOR_SEPARATORS,
			PPDUMP_COLOR_TEXT_HEADER,
			PPDUMP_COLOR_MOUSE_TRACK,

			PPDUMP_COLOR_MAX
		};

	CPPDumpCtrl();

protected:
		enum {	PPDUMP_CANCEL_DATA = 0, //The events
				PPDUMP_ENTER_DATA,

				PPDUMP_MOVE_FIRST_DATA,
				PPDUMP_MOVE_LAST_DATA,
				PPDUMP_MOVE_BEGIN_LINE,
				PPDUMP_MOVE_END_LINE,
				PPDUMP_MOVE_LEFT,
				PPDUMP_MOVE_RIGHT,
				PPDUMP_MOVE_UP,
				PPDUMP_MOVE_DOWN,
				PPDUMP_MOVE_NEXT_FIELD,
				PPDUMP_MOVE_PREV_FIELD,
				PPDUMP_MOVE_PAGE_UP,
				PPDUMP_MOVE_PAGE_DOWN,

				PPDUMP_HOTKEY_HEX,
				PPDUMP_HOTKEY_DEC,
				PPDUMP_HOTKEY_BIN,
				PPDUMP_HOTKEY_OCT,
				PPDUMP_HOTKEY_ASCII,
				
				PPDUMP_MAX_EVENTS
		};

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPPDumpCtrl)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	protected:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	virtual void PreSubclassWindow();
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CPPDumpCtrl();

	// Generated message map functions
protected:
	COLORREF    m_crColor [PPDUMP_COLOR_MAX]; //The colors
	COLORREF    m_crDisableFg; //Color for disable foreground
	COLORREF	m_crDisableBk; //Color for disable background
	
	UINT		m_nStyle; //Current style of the control
	
	LPBYTE		m_pNewData;	//Pointer to the new array
	LPBYTE		m_pOldData;	//Pointer to the old array
	
	int			m_nRealLengthData; //Real lenght of the data of the array
	int			m_nLengthData; //Work lenght of the data array
	int			m_nOffsetAddress; //Offset viewing address
	int			m_nDataInLines; //how much data in the line
	int			m_nMaxDataOnScreen; //how much data may be on the screen

	int			m_nBeginAddress;  //The first address on the screen
	int			m_nEditedAddress; //The address of the editing data
	int			m_nCurrentAddr;   //The address under cursor
	int         m_nCaretAddrBegin;  //The current address of the caret
	int         m_nCaretAddrEnd;  //The current address of the caret
	int         m_nCaretAddrFirst; //The address which started selection data
	int			m_nAddressToolTip; //The address which tooltip show last
	
	int         m_nEditedArea; //The area where data is editing
	int			m_nCurArea; //The area under cursor
	
	TCHAR		m_chSpecCharView; //The char which will change special characters (0 - 31) 
	BOOL        m_bMouseOverCtrl; //the mouse over control
	CString		m_sFormatToolTip; //the format string for tooltip

	BOOL		m_bFocused; //The control was focused
	BOOL		m_bPressedLButton;

	CFont	m_font;	//font
	int		m_nWidthFont; //Average width of the characters
	int		m_nHeightFont; //Height one line

	BYTE	m_nCharsData [PPDUMP_BAR_MAX_AREAS]; //how much chars in the every data fields

	CRect	m_rLastTrackRect; //last track rect of the cursor if exist, else EMPTY
	CRect   m_rBarArea [PPDUMP_BAR_MAX_AREAS + 1]; //The rect of the fields in the control bar
	CRect   m_rFieldArea [PPDUMP_BAR_MAX_AREAS + 1]; //The rect of the data fields

	CScrollBar   m_pVScroll;    // The object of the vertical scrollbar
	CPPNumEdit * m_pEdit;     // Editor of the Value
	CToolTipCtrl m_pToolTip;	// Tooltip

	HMENU		m_hMenu;			// Handle to associated menu
	HWND	    m_hParentWnd; // Handle to window for notification about change data

	//procedures
	void  SetCharsInData(); //Sets much chars in the data of the fields
	DWORD GetDataFromCurrentAddress(DWORD nAddress, BOOL bNewData = TRUE, int nDir = 0, int nByte = 0); //Gets the data from the current address
	int   CalculateControlBar(CRect rect); //Calculates the rect of the data on the control bar

	void VerticalSplitClientArea(CDC * pDC, CRect rect); 
	int  DrawControlBar(CDC * pDC, CRect rect, BOOL bDisable = FALSE); //Draws the control bar
	int  DrawStatusBar(CDC * pDC, CRect rect, BOOL bDisable = FALSE); //Draws the status bar
	void UpdateControlBar(CDC * pDC = NULL, BOOL bDisable = FALSE, int nNewAddr = -1); //Update the control bar
	void RecalculateWorkData(DWORD nNewStyle); //Recalculate work length data array

	void DrawAddressField(CDC * pDC, BOOL bDisable); //Draws the address field
	void DrawHexField(CDC * pDC, BOOL bDisable); //Draws the address field
	void DrawDecField(CDC * pDC, BOOL bDisable); //Draws the address field
	void DrawBinField(CDC * pDC, BOOL bDisable); //Draws the address field
	void DrawOctField(CDC * pDC, BOOL bDisable); //Draws the address field
	void DrawASCIIField(CDC * pDC, BOOL bDisable); //Draws the address field
	
	//-------------------------------------------------------------------------
	int DrawAddressValue(CDC * pDC, CRect rect, UINT nAddress, BOOL bBkgnd = FALSE, BOOL bDisable = FALSE); //Draws Address 
	int DrawHexValue(CDC * pDC, CRect rect, UINT nValue, BOOL bBkgnd = FALSE, BOOL bCaret = FALSE, BOOL bDisable = FALSE); //Draws Hex Value
	int DrawDecValue(CDC * pDC, CRect rect, UINT nValue, BOOL bBkgnd = FALSE, BOOL bCaret = FALSE, BOOL bDisable = FALSE); //Draws Dec Value
	int DrawOctValue(CDC * pDC, CRect rect, UINT nValue, BOOL bBkgnd = FALSE, BOOL bCaret = FALSE, BOOL bDisable = FALSE); //Draws Oct Value
	int DrawBinValue(CDC * pDC, CRect rect, UINT nValue, BOOL bBkgnd = FALSE, BOOL bCaret = FALSE, BOOL bDisable = FALSE); //Draws Bin Value
	int DrawAsciiValue(CDC * pDC, CRect rect, UINT nValue, BOOL bBkgnd = FALSE, BOOL bCaret = FALSE, BOOL bDisable = FALSE); //Draw ASCII

	int DrawStringValue(CDC * pDC, CRect rect, CString str, BOOL bBkgnd = FALSE, BOOL bCaret = FALSE, BOOL bDisable = FALSE); //Draws the string of the value
	
	//-------------------------------------------------------------------------
	BOOL TrackDataField(UINT nIndex, CPoint point); //Track the data of the field
	BOOL TrackDataAsciiField(CPoint point); //Track the data of the ascii field

	BOOL CompleteEditValue(UINT nValue, BOOL bDelete = TRUE);
	void SetEditedValue(int nAddr = -1, int nArea = -1);

	int  GetNextField(int nArea);
	int  GetPrevField(int nArea);
	BOOL IsExistField(int nArea);

	CRect GetRectAddress(int nAddress, int nArea); //Gets the rect of the data 
	int   GetMaxDataOnScreen(); //How much data on the screen
	int   GetAddressForControlBar(); //Gets address of the data in the control bar
	BOOL  KillEdit(); //If exist edit the kill him

	LRESULT SendNotify(UINT uNotifyCode, UINT nAddress, UINT nValue); //Send notify to the parent
	BOOL    HandleHotkeys(UINT uNotifyCode, BOOL bFromEdit = FALSE, UINT nValue = 0); //Handled the pressed hotkeys
	
	void TrackMouseMove(BOOL bDrawn = FALSE, CDC * pDC = NULL); //Draws the mouse track
	void MoveCaretAddress(int nIndex, BOOL bEdited = FALSE, UINT nValue = 0); //Moving the caret
	BOOL SetVisibleAddress (int nAddress);

	void    InitToolTip();
	CString GetToolTipString(int nAddress);
	CString FormatingString(int nIndex, int nValue, int nLength = 0);
	int     GetDataUnderCursor(CPoint pt, LPINT nAddress);

	BOOL RegisterWindowClass();
	BOOL Initialise();

public:
	//Create
	BOOL Create(const RECT& rect, CWnd* pParentWnd, UINT nID, DWORD dwStyle = WS_CHILD | WS_BORDER | WS_TABSTOP | WS_VISIBLE);

	//Functions for the fonts
	BOOL  SetFont(CFont & font, BOOL bRedraw = TRUE); //set font
	BOOL  SetFont(LOGFONT & lf, BOOL bRedraw = TRUE); //set font
	BOOL  SetFont(LPCTSTR lpszFaceName, int nSizePoints = 8,
									BOOL bUnderline = FALSE, BOOL bBold = FALSE,
									BOOL bStrikeOut = FALSE, BOOL bItalic = FALSE, BOOL bRedraw = TRUE); //set font
	void  SetDefaultFont(BOOL bRedraw = TRUE); //set default fonts

	//Functions for the colors
	COLORREF SetColor(int nIndex, COLORREF crColor, BOOL bRedraw = TRUE); //Change color
	COLORREF GetColor(int nIndex); //Gets color
	void	 SetDefaultColors(BOOL bRedraw = TRUE); //Sets default colors

	//Functions for the data arrays
	void  SetPointerData(DWORD nLength, LPBYTE pNewData, LPBYTE pOldData = NULL, BOOL bRedraw = TRUE); 
	void  SetOffsetViewAddress(int nAddress = 0, BOOL bRedraw = TRUE); //Set o
	
	void  SetBeginAddress(int nAddress = 0, BOOL bRedraw = TRUE); //Sets the begin address
	int   GetBeginAddress(); //Gets the begin address 

	//Functions for the styles
	void  SetStyles(DWORD nStyle, BOOL bRedraw = TRUE); //Set New Style
	void  ModifyStyles(DWORD nAddStyle, DWORD nRemoveStyle, BOOL bRedraw = TRUE); //Modify styles
	DWORD GetStyles(); //Get current Styles
	void  SetDefaultStyles(BOOL bRedraw = TRUE); //Set default styles

	void  SetSpecialCharView(TCHAR chSymbol = NULL, BOOL bRedraw = TRUE);

	void  SetReadOnly(BOOL bReadOnly = TRUE);
	BOOL  IsReadOnly();
	
	void  SetSelectRange(int nBegin = 0, int nEnd = -1, BOOL bVisible = TRUE); //Sets the address of the caret
	void  GetSelectRange(LPINT nBegin, LPINT nEnd); //Gets the address of the caret
	BOOL  IsAddressSelected(int nAddress);	
	void  EnableSelect(BOOL bEnable = TRUE);
	BOOL  IsEnableSelect();
	
	BOOL  SetMenu(UINT nMenu, BOOL bRedraw = TRUE);

	void  SetNotify(HWND hWnd);
	void  SetNotify(BOOL bNotify = TRUE);
	BOOL  GetNotify(); //Is enabled notification

	void  SetTooltipText(int nText, BOOL bActivate = TRUE);
	void  SetTooltipText(CString sFormatTip, BOOL bActivate = TRUE);
	void  ActivateTooltip(BOOL bActivate = TRUE);
	CToolTipCtrl * GetTooltip();

	void  SetTrackMouseMove(BOOL bTrack = TRUE, BOOL bRedraw = TRUE);
	BOOL  IsTrackMouseMove();

	//{{AFX_MSG(CPPDumpCtrl)
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnPaint();
	afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnSetFocus(CWnd* pOldWnd);
	afx_msg void OnKillFocus(CWnd* pNewWnd);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnEnable(BOOL bEnable);
	//}}AFX_MSG
	LRESULT OnMouseLeave(WPARAM wParam, LPARAM lParam);
    afx_msg void NotifyEditEnter(NMHDR * pNMHDR, LRESULT * result);
	afx_msg void NotifyEditCancel(NMHDR * pNMHDR, LRESULT * result);
	afx_msg void NotifyEditMove(NMHDR * pNMHDR, LRESULT * result);
	afx_msg void NotifyEditHotKeys(NMHDR * pNMHDR, LRESULT * result);
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PPDUMPCTRL_H__33EAE942_55D0_11D6_98A4_00C026A7402A__INCLUDED_)
