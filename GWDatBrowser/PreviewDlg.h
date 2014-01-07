#pragma once
#include "imagepreviewstatic.h"
#include "PPDumpCtrl.h"
#include "bass.h"
#include "afxcmn.h"
#include "NiceSlider.h"
#include "cxstatic.h"
#include "afxwin.h"

// CPreviewDlg dialog
class CPreviewDlg : public CResizableDialog
{
public:
	CPreviewDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CPreviewDlg();

// Dialog Data
	enum { IDD = IDD_PREVIEW };

	void					setImage(Picture p, const MFTEntry& file, unsigned int index);
	void					setHexData(unsigned char* data, const MFTEntry& file, unsigned int index);
	void					setSoundData(unsigned char* data, const MFTEntry& file, unsigned int index);
protected:
	void					resetDialog(unsigned char* data, const MFTEntry& file, unsigned int index);
	void					updateTime();
	void					setTimer();
	void					killTimer();
	CString					secondsToReadable(int seconds);
	virtual void			DoDataExchange(CDataExchange* pDX); 

	Bitmap*					bitmap;
	BYTE*					data;
	HSTREAM					stream;
	MFTEntry				file;
	unsigned int			fileNumber;

	CPPDumpCtrl*			m_hex_ctrl;
	CImagePreviewStatic*	imageCtrl;
	CNiceSliderCtrl			slider;
	CxStatic				timeCtrl;	
	CStatic					infoCtrl;
	CButton					repeatCtrl;
	CButton					stopCtrl;
	CButton					pauseCtrl;
	CButton					playCtrl;

	bool					fieldAddress;
	bool					fieldHex;
	bool					fieldDec;
	bool					fieldBin;
	bool					fieldOct;
	bool					fieldAscii;
	bool					headerAddress;
	bool					headerHex;
	bool					headerDec;
	bool					headerBin;
	bool					headerOct;
	bool					headerAscii;
	bool					showHeader;
	int						background;

	BOOL					bassWorking;
	UINT_PTR				soundTimer;
	DECLARE_MESSAGE_MAP()
protected:
	afx_msg void OnContextMenu(CWnd* /*pWnd*/, CPoint /*point*/);
	afx_msg void OnBackgroundBlack();
	afx_msg void OnBackgroundCheck();
	afx_msg void OnBackgroundGrey();
	afx_msg void OnBackgroundWhite();
	afx_msg void OnBackgroundNone();
	afx_msg void OnColumnsShowAddress();
	afx_msg void OnColumnsShowHex();
	afx_msg void OnColumnsShowDec();
	afx_msg void OnColumnsShowBinary();
	afx_msg void OnColumnsShowOctal();
	afx_msg void OnColumnsShowAscii();
	afx_msg void OnColumnsShowheader();
	afx_msg void OnHeaderShowAddress();
	afx_msg void OnHeaderShowHex();
	afx_msg void OnHeaderShowDecimal();
	afx_msg void OnHeaderShowBinary();
	afx_msg void OnHeaderShowOctal();
	afx_msg void OnHeaderShowAscii();
	afx_msg void OnBnClickedRepeat();
	afx_msg void OnBnClickedPlay();
	afx_msg void OnBnClickedPause();
	afx_msg void OnBnClickedStop();
	afx_msg void OnSoundExportSound();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	virtual BOOL OnInitDialog();
public:
	afx_msg void OnImageExportImage();
	afx_msg void OnHexExportRawData();
};
