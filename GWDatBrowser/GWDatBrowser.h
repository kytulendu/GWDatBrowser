// GWDatBrowser.h : main header file for the PROJECT_NAME application
//

#pragma once

#ifndef __AFXWIN_H__
	#error "include 'stdafx.h' before including this file for PCH"
#endif

#include "resource.h"		// main symbols


// CGWDatBrowserApp:
// See GWDatBrowser.cpp for the implementation of this class
//

class CGWDatBrowserApp : public CWinApp
{
public:
	CGWDatBrowserApp();

// Overrides
	public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();
protected:
	unsigned long m_gdiplusToken;

// Implementation

	DECLARE_MESSAGE_MAP()
};

extern CGWDatBrowserApp theApp;