// PreviewDlg.cpp : implementation file
//

#include "stdafx.h"
#include "GWDatBrowser.h"
#include "PreviewDlg.h"

// CPreviewDlg dialog
CPreviewDlg::CPreviewDlg(CWnd* pParent /*=NULL*/)
	: CResizableDialog(CPreviewDlg::IDD, pParent),
	bitmap(NULL),
	data(NULL),
	m_hex_ctrl(NULL),
	background(CHECK)
{
	fieldAddress = true;
	fieldHex = true;
	fieldDec = false;
	fieldBin = false;
	fieldOct = false;
	fieldAscii = true;

	headerAddress = true;
	headerHex = true;
	headerDec = false;
	headerBin = false;
	headerOct = false;
	headerAscii = false;

	showHeader = false;
	bassWorking = false;
	soundTimer = 0;
}

CPreviewDlg::~CPreviewDlg()
{
	delete m_hex_ctrl;
	delete imageCtrl;
	delete bitmap;
	delete data;

	if(bassWorking)
		BASS_Free();
}

BEGIN_MESSAGE_MAP(CPreviewDlg, CResizableDialog)
	ON_WM_CONTEXTMENU()
	ON_COMMAND(ID_BACKGROUND_BLACK, &CPreviewDlg::OnBackgroundBlack)
	ON_COMMAND(ID_BACKGROUND_CHECK, &CPreviewDlg::OnBackgroundCheck)
	ON_COMMAND(ID_BACKGROUND_GREY, &CPreviewDlg::OnBackgroundGrey)
	ON_COMMAND(ID_BACKGROUND_WHITE, &CPreviewDlg::OnBackgroundWhite)
	ON_COMMAND(ID_BACKGROUND_NONE, &CPreviewDlg::OnBackgroundNone)
	ON_COMMAND(ID_IMAGE_EXPORTIMAGE, &CPreviewDlg::OnImageExportImage)
	ON_COMMAND(ID_HEX_EXPORTRAWDATA, &CPreviewDlg::OnHexExportRawData)
	ON_COMMAND(ID_COLUMNS_SHOWADDRESS, &CPreviewDlg::OnColumnsShowAddress)
	ON_COMMAND(ID_COLUMNS_SHOWHEX, &CPreviewDlg::OnColumnsShowHex)
	ON_COMMAND(ID_COLUMNS_SHOWDEC, &CPreviewDlg::OnColumnsShowDec)
	ON_COMMAND(ID_COLUMNS_SHOWBINARY, &CPreviewDlg::OnColumnsShowBinary)
	ON_COMMAND(ID_COLUMNS_SHOWOCTAL, &CPreviewDlg::OnColumnsShowOctal)
	ON_COMMAND(ID_COLUMNS_SHOWASCII, &CPreviewDlg::OnColumnsShowAscii)
	ON_COMMAND(ID_COLUMNS_SHOWHEADER, &CPreviewDlg::OnColumnsShowheader)
	ON_COMMAND(ID_HEADER_SHOWADDRESS, &CPreviewDlg::OnHeaderShowAddress)
	ON_COMMAND(ID_HEADER_SHOWHEX, &CPreviewDlg::OnHeaderShowHex)
	ON_COMMAND(ID_HEADER_SHOWDECIMAL, &CPreviewDlg::OnHeaderShowDecimal)
	ON_COMMAND(ID_HEADER_SHOWBINARY, &CPreviewDlg::OnHeaderShowBinary)
	ON_COMMAND(ID_HEADER_SHOWOCTAL, &CPreviewDlg::OnHeaderShowOctal)
	ON_COMMAND(ID_HEADER_SHOWASCII, &CPreviewDlg::OnHeaderShowAscii)
	ON_WM_TIMER()
	ON_WM_HSCROLL()
	ON_BN_CLICKED(IDC_CHECK1, &CPreviewDlg::OnBnClickedRepeat)
	ON_BN_CLICKED(IDC_BUTTON1, &CPreviewDlg::OnBnClickedPlay)
	ON_BN_CLICKED(IDC_BUTTON2, &CPreviewDlg::OnBnClickedPause)
	ON_BN_CLICKED(IDC_BUTTON3, &CPreviewDlg::OnBnClickedStop)
	ON_COMMAND(ID_SOUND_EXPORTSOUND, &CPreviewDlg::OnSoundExportSound)
END_MESSAGE_MAP()

void CPreviewDlg::DoDataExchange(CDataExchange* pDX)
{
	CResizableDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_SLIDER1, slider);
	DDX_Control(pDX, IDC_TIME, timeCtrl);
	DDX_Control(pDX, IDC_INFO, infoCtrl);
	DDX_Control(pDX, IDC_CHECK1, repeatCtrl);
	DDX_Control(pDX, IDC_BUTTON3, stopCtrl);
	DDX_Control(pDX, IDC_BUTTON2, pauseCtrl);
	DDX_Control(pDX, IDC_BUTTON1, playCtrl);
}

#pragma region Implementation

void CPreviewDlg::setImage(Picture p, const MFTEntry& file, unsigned int index)
{
	resetDialog(p.imageData, file, index);
	bitmap = p.bitmap;

	imageCtrl->ShowWindow(SW_SHOW);
	imageCtrl->SetImage(bitmap);
}

void CPreviewDlg::setHexData(unsigned char* hexData, const MFTEntry& file, unsigned int index)
{
	resetDialog(hexData, file, index);
	m_hex_ctrl->ShowWindow(SW_SHOW);
	m_hex_ctrl->SetPointerData(file.uncompressedSize, data);
}

void CPreviewDlg::setSoundData( unsigned char* data, const MFTEntry& file, unsigned int index)
{
	resetDialog(data, file, index);

	if (bassWorking)
	{
		stream = BASS_StreamCreateFile(TRUE, data, 0, file.uncompressedSize, BASS_STREAM_PRESCAN);
		
		if (repeatCtrl.GetCheck() == BST_CHECKED)
			BASS_ChannelFlags(stream, BASS_SAMPLE_LOOP, -1);

		float time=BASS_ChannelBytes2Seconds(stream,BASS_ChannelGetLength(stream, BASS_POS_BYTE)); // playback duration
		DWORD len=BASS_StreamGetFilePosition(stream,BASS_FILEPOS_END); // file length
		DWORD bitrate=(DWORD)(len/(125*time)+0.5); // bitrate (Kbps)

		BASS_CHANNELINFO info; 
		BASS_ChannelGetInfo(stream, &info);

		CString s;
		s.Format(L"Bitrate: \t\t%d\nFrequency: \t%d kHz\nChannels: \t%s\nFormat: \t%s", bitrate, info.freq/1000, info.chans == 1 ? L"mono" : L"Stereo", info.ctype == BASS_CTYPE_STREAM_MP3 ? L"mp3" : L"unknown");
		infoCtrl.SetWindowText(s);		

		slider.SetRangeMax((int)(time * 100));
		slider.SetPos(0);

		setTimer();

		BASS_ChannelPlay(stream,TRUE);

		infoCtrl.ShowWindow(SW_SHOW);
		timeCtrl.ShowWindow(SW_SHOW);
		slider.ShowWindow(SW_SHOW);
		playCtrl.ShowWindow(SW_SHOW);
		pauseCtrl.ShowWindow(SW_SHOW);
		stopCtrl.ShowWindow(SW_SHOW);
		repeatCtrl.ShowWindow(SW_SHOW);
	}
}

void CPreviewDlg::resetDialog(unsigned char* data, const MFTEntry& file, unsigned int index)
{
	imageCtrl->ShowWindow(SW_HIDE);
	m_hex_ctrl->ShowWindow(SW_HIDE);
	infoCtrl.ShowWindow(SW_HIDE);
	timeCtrl.ShowWindow(SW_HIDE);
	slider.ShowWindow(SW_HIDE);
	playCtrl.ShowWindow(SW_HIDE);
	pauseCtrl.ShowWindow(SW_HIDE);
	stopCtrl.ShowWindow(SW_HIDE);
	repeatCtrl.ShowWindow(SW_HIDE);

	killTimer();

	if (stream != 0)
	{
		BASS_ChannelStop(stream);
		BASS_StreamFree(stream);
		stream = 0;
	}

	this->file = file;

	delete[] this->data;
	this->data = data;

	delete bitmap;
	bitmap = NULL;

	fileNumber = index;

	CString s;
	s.Format(L"Preview (# %d Type: %s Size: %d)", fileNumber, typeToString(file.type), file.Size);
	SetWindowText(s);

	ShowWindow(SW_SHOW);
	ActivateTopParent();
}

void CPreviewDlg::updateTime()
{
	CString s;
	s.Format(L"Length: \t%s / %s", secondsToReadable(slider.GetPos()/100), secondsToReadable(slider.GetRangeMax()/100));
	timeCtrl.SetWindowText(s);
}

CString CPreviewDlg::secondsToReadable(int seconds)
{
	CString s;
	if (seconds < 60)
		s.Format(L"00:%.2d", seconds);
	else if (seconds < 3600)
		s.Format(L"%.2d:%.2d", seconds / 60, seconds % 60);
	else
		s.Format(L"%.2d:%.2d:%.2d", seconds / 3600, seconds % 3600 / 60, seconds % 60);
	return s;
}

void CPreviewDlg::killTimer()
{
	if (soundTimer != 0)
	{
		KillTimer(1);
		soundTimer = 0;
	}
}

void CPreviewDlg::setTimer()
{
	if (soundTimer == 0)
		soundTimer = SetTimer(1,200,NULL);
}

#pragma endregion

#pragma region Message Handlers

void CPreviewDlg::OnTimer(UINT_PTR nIDEvent)
{
	if (nIDEvent == 1)
	{
		if (stream != 0)
		{
			QWORD pos = BASS_ChannelGetPosition(stream, BASS_POS_BYTE);
			float time=BASS_ChannelBytes2Seconds(stream, pos); // the time length

			if (pos == BASS_ChannelGetLength(stream, BASS_POS_BYTE) && repeatCtrl.GetCheck() == BST_UNCHECKED)
			{
				killTimer();
				slider.SetPos(0);
			}
			else
				 slider.SetPos((int)(time * 100));

			updateTime();
		}
	}

	CResizableDialog::OnTimer(nIDEvent);
}


BOOL CPreviewDlg::OnInitDialog()
{
	CResizableDialog::OnInitDialog();

	CRect rec;
	GetClientRect(&rec);
	imageCtrl = new CImagePreviewStatic;
	imageCtrl->Create(rec, this, 100);
	imageCtrl->setBackground(background);

	// Create the CPPDumpCtrl object
	m_hex_ctrl = new CPPDumpCtrl;
	m_hex_ctrl->Create(rec, this, 100);
	m_hex_ctrl->SetSpecialCharView(L'.', FALSE);
	m_hex_ctrl->SetStyles(((m_hex_ctrl->GetStyles() | PPDUMP_READ_ONLY | PPDUMP_SEPARATOR_LINES) ^ PPDUMP_BAR_ASCII ^ PPDUMP_BAR_BIN ^ PPDUMP_BAR_DEC ), FALSE);
	CString str = _T("Address: %R Hex%n------ Data -------%nHex:%t%H%nDec:%t%D%nOct:%t%O%nBin:%t%B%nAscii:%t%A"); //Format string
	m_hex_ctrl->SetTooltipText(str); //Sets format string

	AddAnchor(*m_hex_ctrl, TOP_LEFT, BOTTOM_RIGHT);
	AddAnchor(*imageCtrl, TOP_LEFT, BOTTOM_RIGHT);

	AddAnchor(infoCtrl, BOTTOM_LEFT, BOTTOM_RIGHT);
	AddAnchor(timeCtrl, BOTTOM_LEFT, BOTTOM_RIGHT);
	AddAnchor(slider, BOTTOM_LEFT, BOTTOM_RIGHT);
	AddAnchor(playCtrl, BOTTOM_LEFT, BOTTOM_LEFT);
	AddAnchor(pauseCtrl, BOTTOM_LEFT, BOTTOM_LEFT);
	AddAnchor(stopCtrl, BOTTOM_LEFT, BOTTOM_LEFT);
	AddAnchor(repeatCtrl, BOTTOM_RIGHT, BOTTOM_RIGHT);

	repeatCtrl.SetCheck(BST_CHECKED);

	ClientToScreen(&rec);
	rec.bottom = rec.top + 258;
	rec.right = rec.left + 258;
	CalcWindowRect(&rec);
	MoveWindow(&rec);

	SetMinTrackSize(CSize(rec.Width(), rec.Height()));

	bassWorking = BASS_Init(-1,44100,0,GetSafeHwnd(),NULL);
	if (!bassWorking)
		MessageBox(L"BASS_Init() failed!");

	return TRUE;
}

void CPreviewDlg::OnContextMenu(CWnd* /*pWnd*/, CPoint point)
{
	CMenu contextMenu; 
	CMenu* tracker = NULL; 

	//at first we'll load our menu
	contextMenu.LoadMenu(IDR_CONTEXT); 

	MENUITEMINFO m;
	m.cbSize = sizeof(m);
	m.fMask = MIIM_STATE;
	m.fState = MFS_CHECKED;

	if (imageCtrl->IsWindowVisible())
	{
		tracker = contextMenu.GetSubMenu(0); 

		switch(background)
		{
		case BLACK:
			tracker->SetMenuItemInfo(ID_BACKGROUND_BLACK,&m);
			break;
		case GREY:
			tracker->SetMenuItemInfo(ID_BACKGROUND_GREY,&m);
			break;
		case WHITE:
			tracker->SetMenuItemInfo(ID_BACKGROUND_WHITE,&m);
			break;
		case CHECK:
			tracker->SetMenuItemInfo(ID_BACKGROUND_CHECK,&m);
			break;
		default:
			tracker->SetMenuItemInfo(ID_BACKGROUND_NONE,&m);
			break;
		}
	}
	else if (m_hex_ctrl->IsWindowVisible())
	{
		tracker = contextMenu.GetSubMenu(1); 

		if (fieldAddress)
			tracker->SetMenuItemInfo(ID_COLUMNS_SHOWADDRESS,&m);
		if (fieldHex)
			tracker->SetMenuItemInfo(ID_COLUMNS_SHOWHEX,&m);
		if (fieldDec)
			tracker->SetMenuItemInfo(ID_COLUMNS_SHOWDEC,&m);
		if (fieldBin)
			tracker->SetMenuItemInfo(ID_COLUMNS_SHOWBINARY,&m);
		if (fieldOct)
			tracker->SetMenuItemInfo(ID_COLUMNS_SHOWOCTAL,&m);
		if (fieldAscii)
			tracker->SetMenuItemInfo(ID_COLUMNS_SHOWASCII,&m);
		if (showHeader)
			tracker->SetMenuItemInfo(ID_COLUMNS_SHOWHEADER,&m);

		if (headerAddress)
			tracker->SetMenuItemInfo(ID_HEADER_SHOWADDRESS,&m);
		if (headerHex)
			tracker->SetMenuItemInfo(ID_HEADER_SHOWHEX,&m);
		if (headerDec)
			tracker->SetMenuItemInfo(ID_HEADER_SHOWDECIMAL,&m);
		if (headerBin)
			tracker->SetMenuItemInfo(ID_HEADER_SHOWBINARY,&m);
		if (headerOct)
			tracker->SetMenuItemInfo(ID_HEADER_SHOWOCTAL,&m);
		if (headerAscii)
			tracker->SetMenuItemInfo(ID_HEADER_SHOWASCII,&m);
	}
	else
		tracker = contextMenu.GetSubMenu(3); 

	//show the context menu
	tracker->TrackPopupMenu(TPM_LEFTALIGN | TPM_LEFTBUTTON | TPM_RIGHTBUTTON, point.x , point.y , this); 
}
void CPreviewDlg::OnBnClickedPlay()
{
	if (stream)
	{
		BASS_ChannelPlay(stream, FALSE);
		setTimer();
	}
}

void CPreviewDlg::OnBnClickedPause()
{
	if (stream)
	{
		BASS_ChannelPause(stream);
		killTimer();
	}
}

void CPreviewDlg::OnBnClickedStop()
{
	if (stream)
	{
		BASS_ChannelStop(stream);
		BASS_ChannelSetPosition(stream, 0, BASS_POS_BYTE);
		slider.SetPos(0);
		killTimer();
	}
}

void CPreviewDlg::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	// TODO: Add your message handler code here and/or call default
	if (stream)
	{
		if (nSBCode == TB_THUMBTRACK)
			killTimer();
		else if (nSBCode == TB_ENDTRACK)
		{
			QWORD pos = BASS_ChannelSeconds2Bytes(stream, slider.GetPos()/100.0f);		
			BASS_ChannelSetPosition(stream, pos, BASS_POS_BYTE);
			setTimer();

		}
		updateTime();
	}
	CResizableDialog::OnHScroll(nSBCode, nPos, pScrollBar);
}

void CPreviewDlg::OnBnClickedRepeat()
{
	if (!stream)
		return;

	if (repeatCtrl.GetCheck() == BST_CHECKED)
		BASS_ChannelFlags(stream, BASS_SAMPLE_LOOP, -1);
	else
		BASS_ChannelFlags(stream, 0, -1);
}
#pragma endregion

#pragma region ContextMenu handlers
void CPreviewDlg::OnBackgroundBlack()
{
	background = BLACK;
	imageCtrl->setBackground(background);
}

void CPreviewDlg::OnBackgroundCheck()
{
	background = CHECK;
	imageCtrl->setBackground(background);
}

void CPreviewDlg::OnBackgroundGrey()
{
	background = GREY;
	imageCtrl->setBackground(background);
}

void CPreviewDlg::OnBackgroundWhite()
{
	background = WHITE;
	imageCtrl->setBackground(background);
}

void CPreviewDlg::OnBackgroundNone()
{
	background = NONE;
	imageCtrl->setBackground(background);
}

void CPreviewDlg::OnImageExportImage()
{
	CString s;
	s.Format(L"%d_%s", fileNumber, typeToString(file.type));

	ExportImage(GetSafeHwnd(), bitmap, s);
}

void CPreviewDlg::OnHexExportRawData()
{
	CString s;
	s.Format(L"%d_%s", fileNumber, typeToString(file.type));

	ExportRaw(GetSafeHwnd(), data, file.uncompressedSize, s, file);
}

void CPreviewDlg::OnSoundExportSound()
{
	CString s;
	s.Format(L"%d_%s", fileNumber, typeToString(file.type));

	ExportRaw(GetSafeHwnd(), data, file.uncompressedSize, s, file);
}

void CPreviewDlg::OnColumnsShowAddress()
{
	fieldAddress = !fieldAddress;
	m_hex_ctrl->SetStyles(m_hex_ctrl->GetStyles() ^ PPDUMP_FIELD_ADDRESS);
}

void CPreviewDlg::OnColumnsShowHex()
{
	fieldHex = !fieldHex;
	m_hex_ctrl->SetStyles(m_hex_ctrl->GetStyles() ^ PPDUMP_FIELD_HEX);
}

void CPreviewDlg::OnColumnsShowDec()
{
	fieldDec = !fieldDec;
	m_hex_ctrl->SetStyles(m_hex_ctrl->GetStyles() ^ PPDUMP_FIELD_DEC);
}

void CPreviewDlg::OnColumnsShowBinary()
{
	fieldBin = !fieldBin;
	m_hex_ctrl->SetStyles(m_hex_ctrl->GetStyles() ^ PPDUMP_FIELD_BIN);
}

void CPreviewDlg::OnColumnsShowOctal()
{
	fieldOct = !fieldOct;
	m_hex_ctrl->SetStyles(m_hex_ctrl->GetStyles() ^ PPDUMP_FIELD_OCT);
}

void CPreviewDlg::OnColumnsShowAscii()
{
	fieldAscii = !fieldAscii;
	m_hex_ctrl->SetStyles(m_hex_ctrl->GetStyles() ^ PPDUMP_FIELD_ASCII);
}

void CPreviewDlg::OnColumnsShowheader()
{
	showHeader = !showHeader;
	m_hex_ctrl->SetStyles(m_hex_ctrl->GetStyles() ^ PPDUMP_NAMED_FIELDS);
}

void CPreviewDlg::OnHeaderShowAddress()
{
	headerAddress = !headerAddress;
	m_hex_ctrl->SetStyles(m_hex_ctrl->GetStyles() ^ PPDUMP_BAR_ADDRESS);
}

void CPreviewDlg::OnHeaderShowHex()
{
	headerHex = !headerHex;
	m_hex_ctrl->SetStyles(m_hex_ctrl->GetStyles() ^ PPDUMP_BAR_HEX);
}

void CPreviewDlg::OnHeaderShowDecimal()
{
	headerDec = !headerDec;
	m_hex_ctrl->SetStyles(m_hex_ctrl->GetStyles() ^ PPDUMP_BAR_DEC);
}

void CPreviewDlg::OnHeaderShowBinary()
{
	headerBin = !headerBin;
	m_hex_ctrl->SetStyles(m_hex_ctrl->GetStyles() ^ PPDUMP_BAR_BIN);
}

void CPreviewDlg::OnHeaderShowOctal()
{
	headerOct = !headerOct;
	m_hex_ctrl->SetStyles(m_hex_ctrl->GetStyles() ^ PPDUMP_BAR_OCT);
}

void CPreviewDlg::OnHeaderShowAscii()
{
	headerAscii = !headerAscii;
	m_hex_ctrl->SetStyles(m_hex_ctrl->GetStyles() ^ PPDUMP_BAR_ASCII);
}

#pragma endregion
