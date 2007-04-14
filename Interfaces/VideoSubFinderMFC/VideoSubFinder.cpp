                              //VideoSubFinder.cpp//                                
//////////////////////////////////////////////////////////////////////////////////
//							  Version 1.76              						//
//																				//
// Author:  Simeon Kosnitsky													//
//          skosnits@gmail.com													//
//																				//
// License:																		//
//     This software is released into the public domain.  You are free to use	//
//     it in any way you like, except that you may not sell this source code.	//
//																				//
//     This software is provided "as is" with no expressed or implied warranty.	//
//     I accept no liability for any damage or loss of business that this		//
//     software may cause.														//
//																				//
//////////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "VideoSubFinder.h"

/////////////////////////////////////////////////////////////////////////////

BEGIN_MESSAGE_MAP(CVideoSubFinderApp, CWinApp)
	ON_COMMAND(ID_APP_ABOUT, OnAppAbout)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////

CVideoSubFinderApp::CVideoSubFinderApp()
{
}

/////////////////////////////////////////////////////////////////////////////

CVideoSubFinderApp NEAR theApp;

/////////////////////////////////////////////////////////////////////////////

BOOL CVideoSubFinderApp::InitInstance()
{
	CMainFrame* pMainFrame = new CMainFrame;
	if (!pMainFrame->LoadFrame(IDR_MAINFRAME)) return FALSE;
	pMainFrame->ShowWindow(m_nCmdShow);
	pMainFrame->UpdateWindow();
	m_pMainWnd = pMainFrame;

	pMainFrame->Init();

	return true;
}

/////////////////////////////////////////////////////////////////////////////

void CVideoSubFinderApp::OnAppAbout()
{
	CDialog(IDD_ABOUTBOX).DoModal();
}