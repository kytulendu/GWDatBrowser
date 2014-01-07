// GWDatBrowserDlg.h : header file
//

#pragma once
#include "afxwin.h"
#include "afxcmn.h"
#include "PreviewDlg.h"
#include "CxStatic.h"

// CGWDatBrowserDlg dialog
class CGWDatBrowserDlg : public CResizableDialog
{
// Construction
public:
	CGWDatBrowserDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	enum { IDD = IDD_GWDATBROWSER_DIALOG };

// Implementation
protected:
	virtual void	DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	virtual BOOL	OnInitDialog();
	afx_msg LRESULT onFileRead(WPARAM wParam, LPARAM lParam);
	afx_msg void	OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void	OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void	OnLvnGetdispinfoList1(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void	OnLvnColumnclickList1(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void	OnLvnItemchangedList1(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void	OnNcDestroy();
	afx_msg void	OnContextMenu(CWnd* /*pWnd*/, CPoint /*point*/);
	afx_msg void	OnFilelistExportrawdata();
	afx_msg void	OnFilelistExportImage();
	afx_msg void	OnClose();
	DECLARE_MESSAGE_MAP()
protected:
	HICON			m_hIcon;
	CString			gwpath;
	GWDat			dat;

	CPreviewDlg*	previewDlg;
	CListCtrl		fileListCtrl;
	CProgressCtrl	progressBarCtrl;
	CxStatic		filesReadCtrl;
	CWinThread*		workerThread;

public:
	unsigned int*	sortedIndexToIndex;
	unsigned int*	indexToSortedIndex;
};

struct ThreadData
{
	ThreadData(GWDat* dat, CGWDatBrowserDlg* dlg): dat(dat), dlg(dlg) {};

	GWDat* dat;
	CGWDatBrowserDlg* dlg;
};
