// GWDatBrowserDlg.cpp : implementation file
//

#include "stdafx.h"
#include "GWDatBrowser.h"
#include "GWDatBrowserDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

bool exitThread = false;

// CAboutDlg dialog used for App About
class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// Dialog Data
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

// Implementation
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
END_MESSAGE_MAP()


// CGWDatBrowserDlg dialog
CGWDatBrowserDlg::CGWDatBrowserDlg(CWnd* pParent /*=NULL*/)
	: CResizableDialog(CGWDatBrowserDlg::IDD, pParent), workerThread(NULL)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CGWDatBrowserDlg::DoDataExchange(CDataExchange* pDX)
{
	CResizableDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST1, fileListCtrl);
	DDX_Control(pDX, IDC_PROGRESS1, progressBarCtrl);
	DDX_Control(pDX, IDC_FILESREAD, filesReadCtrl);
}

BEGIN_MESSAGE_MAP(CGWDatBrowserDlg, CResizableDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_MESSAGE(WM_FILEREAD, &CGWDatBrowserDlg::onFileRead)
	ON_NOTIFY(LVN_GETDISPINFO, IDC_LIST1, &CGWDatBrowserDlg::OnLvnGetdispinfoList1)
	ON_NOTIFY(LVN_COLUMNCLICK, IDC_LIST1, &CGWDatBrowserDlg::OnLvnColumnclickList1)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_LIST1, &CGWDatBrowserDlg::OnLvnItemchangedList1)
	ON_WM_NCDESTROY()
	ON_WM_CONTEXTMENU()
	ON_COMMAND(ID_FILELIST_EXPORTRAWDATA, &CGWDatBrowserDlg::OnFilelistExportrawdata)
	ON_COMMAND(ID_FILELIST_EXPORTIMAGE, &CGWDatBrowserDlg::OnFilelistExportImage)
	ON_WM_CLOSE()
END_MESSAGE_MAP()


UINT MyThreadProc( LPVOID pParam )
{
	GWDat* dat = ((ThreadData*)pParam)->dat;
	CGWDatBrowserDlg* dlg = ((ThreadData*)pParam)->dlg;

	delete (ThreadData*)pParam;

	unsigned char* data;
	for (unsigned int i = 0; i < dat->getNumFiles(); ++i)
	{
		data = dat->readFile(i, false);
		delete[] data;
		if(exitThread)
			return 0;

		dlg->SendMessage(WM_FILEREAD, dlg->indexToSortedIndex[i], i);
	}

	return 0;   // thread completed successfully
}

// CGWDatBrowserDlg message handlers
BOOL CGWDatBrowserDlg::OnInitDialog()
{
	CResizableDialog::OnInitDialog();
	// Add "About..." menu item to system menu.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		CString strAboutMenu;
		strAboutMenu.LoadString(IDS_ABOUTBOX);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	AddAnchor(fileListCtrl, TOP_LEFT, BOTTOM_RIGHT);
	AddAnchor(filesReadCtrl, BOTTOM_LEFT, BOTTOM_RIGHT);
	AddAnchor(progressBarCtrl, BOTTOM_LEFT, BOTTOM_RIGHT);

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	fileListCtrl.InsertColumn(0, L"#", 0, 60, 0);
	fileListCtrl.InsertColumn(1, L"ID", 0, 60, 1);
	fileListCtrl.InsertColumn(2, L"Position", 0, 80, 2);
	fileListCtrl.InsertColumn(3, L"Sector", 0, 80, 3);
	fileListCtrl.InsertColumn(4, L"Size", 0, 60, 4);
	fileListCtrl.InsertColumn(5, L"Uncomp. Size", 0, 80, 5);
	fileListCtrl.InsertColumn(6, L"Flags", 0, 60, 6);
	fileListCtrl.InsertColumn(7, L"Type", 0, 80, 7);
	fileListCtrl.InsertColumn(8, L"Hash", 0, 80, 7);
	
	fileListCtrl.SetExtendedStyle(fileListCtrl.GetExtendedStyle() | LVS_EX_FULLROWSELECT | LVS_EX_DOUBLEBUFFER);

	previewDlg = new CPreviewDlg(this);
	previewDlg->Create(CPreviewDlg::IDD);

	// Find the path to the GW.dat in the registry
	CRegKey key;
	if (ERROR_SUCCESS == key.Open( HKEY_CURRENT_USER, L"SOFTWARE\\ArenaNet\\Guild Wars", KEY_READ))
	{
		TCHAR buffer[1024];
		DWORD dwCount = sizeof(buffer);

		if (ERROR_SUCCESS == key.QueryStringValue(L"Path", buffer, &dwCount))
		{
			gwpath = buffer;
			gwpath = gwpath.Left(gwpath.ReverseFind(L'\\'));
			gwpath += L"\\GW.dat";
		}
		key.Close();
	}
	else
	{
		MessageBox(L"Couldn't open \"HKEY_CURRENT_USER\\SOFTWARE\\ArenaNet\\Guild Wars\".");
		//TODO: Display a file Open dialog here 
		EndDialog(0);
	}

	CString title = L"GWDatbrowser (" + gwpath + L")";
	SetWindowText(title);
	
	unsigned int files = dat.readDat(gwpath.GetString());

	if (files == 0)
		EndDialog(0);

	sortedIndexToIndex = new unsigned int[files];
	indexToSortedIndex = new unsigned int[files];

	for (unsigned int i = 0; i < files; ++i)
		sortedIndexToIndex[i] = indexToSortedIndex[i] = i;

	fileListCtrl.SetItemCount(files);
	progressBarCtrl.SetRange32(0, files);

	ThreadData* t = new ThreadData(&dat, this);
	workerThread = AfxBeginThread(MyThreadProc, t);

	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CGWDatBrowserDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CResizableDialog::OnSysCommand(nID, lParam);
	}
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.
void CGWDatBrowserDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting
		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CResizableDialog::OnPaint();
	}
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CGWDatBrowserDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CGWDatBrowserDlg::OnLvnGetdispinfoList1(NMHDR *pNMHDR, LRESULT *pResult)
{
	NMLVDISPINFO *pDispInfo = reinterpret_cast<NMLVDISPINFO*>(pNMHDR);
	LV_ITEM* pItem= &(pDispInfo)->item;

	int iItemIndx= sortedIndexToIndex[pItem->iItem];
	
	if (pItem->mask & LVIF_TEXT) //valid text buffer?
	{
		CString text;
		switch(pItem->iSubItem)
		{
		case 0: //fill in main text
			text.Format(L"%d", iItemIndx + 1);
			break;
		case 1:
			text.Format(L"%d", dat[iItemIndx].ID);
			break;
		case 2:
			text.Format(L"%I64d ", dat[iItemIndx].Offset);
			break;
		case 3:
			text.Format(L"%d", dat[iItemIndx].Offset/dat.getSectorSize());
			break;
		case 4:
			text.Format(L"%d", dat[iItemIndx].Size);
			break;
		case 5:
			if (dat[iItemIndx].uncompressedSize != -1)
				text.Format(L"%d", dat[iItemIndx].uncompressedSize);
			break;
		case 6:
			text.Format(L"%02d %02d %02d", dat[iItemIndx].a, dat[iItemIndx].b, dat[iItemIndx].c);
			break;
		case 7:
			text = typeToString(dat[iItemIndx].type);
			break;
		case 8:
			text.Format(L"%x", dat[iItemIndx].Hash);
			break;
		}

		lstrcpy(pItem->pszText, text.GetString());
	}
	*pResult = 0;
}

void CGWDatBrowserDlg::OnLvnColumnclickList1(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
	
	static int lastColumn = 0;
	static bool ascending = false;

	if (lastColumn == pNMLV->iSubItem)
		ascending = !ascending;
	else
		ascending = false;
	lastColumn = pNMLV->iSubItem;

	//Get the selected item
	POSITION pos = fileListCtrl.GetFirstSelectedItemPosition();
	int selectedItem = -1;
	if (pos != NULL)
	{
		selectedItem = fileListCtrl.GetNextSelectedItem(pos);
		fileListCtrl.SetItemState(selectedItem, 0, LVIS_SELECTED);
	}

	if (selectedItem != -1)
		selectedItem = sortedIndexToIndex[selectedItem];

	dat.sort(sortedIndexToIndex, lastColumn, ascending);

	for (unsigned int i = 0; i < dat.getNumFiles(); ++i)
		indexToSortedIndex[sortedIndexToIndex[i]] = i;

	//Select the item that was selected before sorting
	if (selectedItem != -1)
	{
		selectedItem = indexToSortedIndex[selectedItem];
		fileListCtrl.EnsureVisible(selectedItem, FALSE);
		fileListCtrl.SetItemState(selectedItem, LVIS_SELECTED, LVIS_SELECTED);
	}
	
	fileListCtrl.RedrawItems(0, fileListCtrl.GetItemCount());

	*pResult = 0;
}

void CGWDatBrowserDlg::OnLvnItemchangedList1(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
	// TODO: Add your control notification handler code here
	*pResult = 0;

	if ((pNMLV->uChanged & LVIF_STATE) && (pNMLV->uNewState & LVIS_SELECTED)) 
	{
		//Selection changed
		unsigned int index = sortedIndexToIndex[pNMLV->iItem]; 
		unsigned char* data = dat.readFile(index);
		
		if (data)
		{
			int type = dat[index].type;
			if (type >= ATEXDXT1 && type <= ATTXDXTL  && type != ATEXDXTA && type != ATTXDXTA)
			{
				Picture p = ProcessImageFile(data, dat[index].uncompressedSize);
				delete[] data;
				if (p.bitmap)
				{
					previewDlg->setImage(p, dat[index], index + 1);
				}
			}
			else if(type == SOUND || type == AMP)
			{
				previewDlg->setSoundData(data, dat[index], index + 1);
			}
			else
			{
				previewDlg->setHexData(data, dat[index], index + 1);
			}
			
		}
		onFileRead(pNMLV->iItem, 0);
	}	
}

void CGWDatBrowserDlg::OnNcDestroy()
{
	CResizableDialog::OnNcDestroy();
	delete previewDlg;
	delete[] indexToSortedIndex;
	delete[] sortedIndexToIndex;
}

LRESULT CGWDatBrowserDlg::onFileRead( WPARAM wParam, LPARAM lParam )
{
	progressBarCtrl.SetPos(dat.getFilesRead());

	if (dat.getFilesRead() == dat.getNumFiles())
		progressBarCtrl.SendMessage(PBM_SETSTATE, PBST_PAUSED, 0);

	CString s;
	s.Format(L"Files read: %d (%.2f%%) | Textures: %d | Text: %d | Sounds: %d | FFNA: %d | AMAT: %d | MFTBase: %d | Unknown: %d", dat.getFilesRead(), (float)dat.getFilesRead()/dat.getNumFiles()*100.0f, dat.getTextureFiles(), dat.getTextFiles(), dat.getSoundFiles(), dat.getFfnaFiles(), dat.getAmatFiles(), dat.getMftBaseFiles(), dat.getUnknownFiles());
	filesReadCtrl.SetWindowText(s);

	fileListCtrl.RedrawItems((int)wParam, (int)wParam);

	return 0;
}
void CGWDatBrowserDlg::OnContextMenu(CWnd* /*pWnd*/, CPoint point)
{
	POSITION pos = fileListCtrl.GetFirstSelectedItemPosition();
	if (pos != NULL)
	{
		while (pos)
		{
			int nItem = fileListCtrl.GetNextSelectedItem(pos);
			
			CMenu contextMenu; 
			CMenu* tracker = NULL; 

			//at first we'll load our menu
			contextMenu.LoadMenu(IDR_CONTEXT); 

			MENUITEMINFO m;
			m.cbSize = sizeof(m);
			m.fMask = MIIM_STATE;
			m.fState = 0;

			tracker = contextMenu.GetSubMenu(2); 

			if (dat[nItem].type >= ATEXDXT1 && dat[nItem].type <= ATTXDXTL && dat[nItem].type != ATEXDXTA && dat[nItem].type != ATTXDXTA )
				tracker->SetMenuItemInfo(ID_FILELIST_EXPORTIMAGE,&m);

			if (dat[nItem].type != MFTBASE)
				tracker->SetMenuItemInfo(ID_FILELIST_EXPORTRAWDATA,&m);
			
			//show the context menu
			tracker->TrackPopupMenu(TPM_LEFTALIGN | TPM_LEFTBUTTON | TPM_RIGHTBUTTON, point.x , point.y , this); 
		}
	}
}

void CGWDatBrowserDlg::OnFilelistExportrawdata()
{
	previewDlg->OnHexExportRawData();
}

void CGWDatBrowserDlg::OnFilelistExportImage()
{
	previewDlg->OnImageExportImage();
}

void CGWDatBrowserDlg::OnClose()
{
	//Signal the worker thread to exit
	exitThread = true;

	CResizableDialog::OnClose();
}
