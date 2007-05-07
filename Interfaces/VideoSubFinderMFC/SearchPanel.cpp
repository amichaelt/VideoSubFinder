                              //SearchPanel.cpp//                                
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
#include "myResource.h"
#include "VideoSubFinder.h"
#include ".\searchpanel.h"

int IsSearching = 0;
int IsClose = 0;

CSearchPanel::CSearchPanel()
{
}

CSearchPanel::~CSearchPanel()
{
}

BEGIN_MESSAGE_MAP(CSearchPanel, CWnd)
	ON_WM_CTLCOLOR()
	ON_BN_CLICKED(ID_BTN_RUN, OnBnClickedRun)
	ON_BN_CLICKED(ID_BTN_CLEAR, OnBnClickedClear)
END_MESSAGE_MAP()

void CSearchPanel::Init(CSSOWnd* pParent)
{
	CString strSPClass;

	m_pParent = pParent;
	m_pMainFrm = pParent->m_pParent;

	m_SPBrush.CreateSolidBrush(RGB(125,125,125));

	strSPClass = AfxRegisterWndClass(CS_VREDRAW | CS_HREDRAW,
									LoadCursor(NULL, IDC_ARROW),
									m_SPBrush,
									NULL);

	Create( strSPClass, 
			"", 
			WS_CHILD | WS_VISIBLE | WS_BORDER,
			CRect(10,10,400,300),
			pParent,
			ID_PNL_SEARCH );

	m_CL1Brush.CreateSolidBrush(RGB(255, 215, 0));
	m_CL2Brush.CreateSolidBrush(RGB(127, 255, 0));

	m_LBLFont.CreateFont(
		14,                        // nHeight
		0,                         // nWidth
		0,                         // nEscapement
		0,                         // nOrientation
		FW_NORMAL,                 // nWeight
		FALSE,                     // bItalic
		FALSE,                     // bUnderline
		0,                         // cStrikeOut
		ANSI_CHARSET,              // nCharSet
		OUT_DEFAULT_PRECIS,        // nOutPrecision
		CLIP_DEFAULT_PRECIS,       // nClipPrecision
		DEFAULT_QUALITY,           // nQuality
		DEFAULT_PITCH | FF_SWISS,  // nPitchAndFamily
		"Microsoft Sans Serif");   // lpszFacename

	m_BTNFont.CreateFont(
		22,                        // nHeight
		0,                         // nWidth
		0,                         // nEscapement
		0,                         // nOrientation
		FW_NORMAL,                 // nWeight
		FALSE,                     // bItalic
		FALSE,                     // bUnderline
		0,                         // cStrikeOut
		ANSI_CHARSET,              // nCharSet
		OUT_DEFAULT_PRECIS,        // nOutPrecision
		CLIP_DEFAULT_PRECIS,       // nClipPrecision
		DEFAULT_QUALITY,           // nQuality
		DEFAULT_PITCH | FF_SWISS,  // nPitchAndFamily
		"Arial Black");			   // lpszFacename

	CRect rcP1, rcClP1, rcBT1, rcBTA1, rcBT2, rcBTA2, rcClear, rcRun;
	int dw, dh;

	this->GetClientRect(rcClP1);
	this->GetWindowRect(rcP1);
	dw = rcP1.Width() - rcClP1.Width();
	dh = rcP1.Height() - rcClP1.Height();
	
	rcBT1.left = 20;
	rcBT1.right = rcBT1.left + 80;
	rcBT1.top = 20;
	rcBT1.bottom = rcBT1.top + 16;
	
	rcBTA1.left = rcBT1.right+2;
	rcBTA1.right = rcBTA1.left + 260;
	rcBTA1.top = rcBT1.top;
	rcBTA1.bottom = rcBT1.bottom;

	rcBT2.left = rcBT1.left;
	rcBT2.right = rcBT1.right;
	rcBT2.top = rcBT1.bottom + 6;
	rcBT2.bottom = rcBT2.top + rcBT1.Height();
	
	rcBTA2.left = rcBTA1.left;
	rcBTA2.right = rcBTA1.right;
	rcBTA2.top = rcBT2.top;
	rcBTA2.bottom = rcBTA2.top + rcBTA1.Height();
	
	rcClear.left = rcBT2.left + 8;
	rcClear.right = rcClear.left + 150;
	rcClear.top = rcBT2.bottom + 10;
	rcClear.bottom = rcClear.top + 30;

	rcRun.right = rcBTA2.right - 8;
	rcRun.left = rcRun.right - 150;
	rcRun.top = rcBT2.bottom + 10;
	rcRun.bottom = rcRun.top + 30;
	
	rcP1.left = 10;
	rcP1.right = rcP1.left + rcBTA1.right + rcBT1.left + dw;
	rcP1.top = 10;
	rcP1.bottom = rcP1.top + rcRun.bottom + 20 + dh;

	this->MoveWindow(&rcP1);

	m_lblBT1.Create(" Begin Time:", WS_CHILD | WS_VISIBLE | SS_LEFT | SS_CENTERIMAGE | WS_BORDER,
					rcBT1, this, ID_LBL_BT1);

	m_lblBT2.Create(" End Time:", WS_CHILD | WS_VISIBLE | SS_LEFT | SS_CENTERIMAGE | WS_BORDER,
					rcBT2, this, ID_LBL_BT2);

	m_lblBTA1.Create("", WS_CHILD | WS_VISIBLE | SS_LEFT | SS_CENTERIMAGE | WS_BORDER,
					rcBTA1, this, ID_LBL_BTA1);

	m_lblBTA2.Create("", WS_CHILD | WS_VISIBLE | SS_LEFT | SS_CENTERIMAGE | WS_BORDER,
					rcBTA2, this, ID_LBL_BTA2);

	m_Clear.Create("Clear Folders", WS_CHILD | WS_VISIBLE, rcClear, this, ID_BTN_CLEAR);
	m_Run.Create("Run Search", WS_CHILD | WS_VISIBLE, rcRun, this, ID_BTN_RUN);

	m_lblBT1.SetFont(&m_LBLFont);
	m_lblBT2.SetFont(&m_LBLFont);
	m_lblBTA1.SetFont(&m_LBLFont);
	m_lblBTA2.SetFont(&m_LBLFont);

	m_Clear.SetFont(&m_BTNFont);
	m_Run.SetFont(&m_BTNFont);
}

HBRUSH CSearchPanel::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	HBRUSH hbr = CWnd::OnCtlColor(pDC, pWnd, nCtlColor);

	switch (pWnd->GetDlgCtrlID())
	{
		case ID_LBL_BT1:
			pDC->SetBkMode(TRANSPARENT);
			hbr = m_CL2Brush;
			break;

		case ID_LBL_BTA1:
			pDC->SetBkMode(TRANSPARENT);
			hbr = m_CL2Brush;
			break;

		case ID_LBL_BT2:
			pDC->SetBkMode(TRANSPARENT);
			hbr = m_CL1Brush;
			break;

		case ID_LBL_BTA2:
			pDC->SetBkMode(TRANSPARENT);
			hbr = m_CL1Brush;
			break;
	}

	return hbr;
}

void CSearchPanel::OnBnClickedRun()
{
	if (m_pMainFrm->m_VIsOpen)
	{
		m_pMainFrm->OnStop();

		m_pMainFrm->m_VIsOpen = false;

		if (m_pMainFrm->m_VTimerFuncID == 1) 
		{
			m_pMainFrm->KillTimer(m_pMainFrm->m_VTimerFuncID);
			m_pMainFrm->m_VTimerFuncID = 0;
		}

		m_pMainFrm->m_VTimerFuncID = 1;
		m_pMainFrm->m_ct = -1;
		m_pMainFrm->SetTimer(m_pMainFrm->m_VTimerFuncID, 1000, (TIMERPROC)VTimerFunc);

		m_Run.SetWindowText("Stop Search");

		m_pMainFrm->m_pVideoBox->m_VBar.SetButtonStyle(0, TBBS_CHECKBOX);
		m_pMainFrm->m_pVideoBox->m_VBar.SetButtonStyle(1, TBBS_CHECKBOX);
		m_pMainFrm->m_pVideoBox->m_VBar.SetButtonStyle(2, TBBS_CHECKBOX);

		m_pMainFrm->m_pPanel->m_SSPanel.EnableWindow(0);
		m_pMainFrm->m_pPanel->m_OCRPanel.EnableWindow(0);
		m_pMainFrm->m_pImageBox->ClearScreen();


		InitIPData((int)m_pMainFrm->m_pVideo->m_Width, (int)m_pMainFrm->m_pVideo->m_Height, 1);

		m_hSearchThread = CreateThread(NULL, 0, ThreadSearchSubtitles, (PVOID)m_pMainFrm, 0, &m_dwSearchThreadID);
		SetThreadPriority(m_hSearchThread, THREAD_PRIORITY_BELOW_NORMAL);
	}
	else
	{
		if (g_RunSubSearch == 1) 
		{
			g_RunSubSearch = 0;
		}
	}
}

void CSearchPanel::OnBnClickedClear()
{
	m_pMainFrm->ClearDir("RGBImages");
	m_pMainFrm->ClearDir("FRDImages");
	m_pMainFrm->ClearDir("TSTImages");
	m_pMainFrm->ClearDir("TXTImages");
	m_pMainFrm->ClearDir("TestImages");
	m_pMainFrm->ClearDir("TXTResults");	
}

DWORD WINAPI ThreadSearchSubtitles(PVOID pParam)
{	
	IsSearching = 1;

	CMainFrame* pMF = (CMainFrame*)pParam;

	if (g_fast_search == true)
	{
        if ( pMF->m_pVideo->SetNullRender() )
        {
	        SetVideoWindowSettins(pMF->m_pVideo, 
                                  pMF->m_pVideoBox->m_VBox.m_VSL1.m_pos, 
                                  pMF->m_pVideoBox->m_VBox.m_VSL2.m_pos, 
                                  pMF->m_pVideoBox->m_VBox.m_HSL1.m_pos, 
                                  pMF->m_pVideoBox->m_VBox.m_HSL2.m_pos);

            pMF->m_BegTime = FastSearchSubtitles( pMF->m_pVideo, pMF->m_BegTime, pMF->m_EndTime );
        }
	}
	else
	{
        if ( pMF->m_pVideo->SetNullRender() )
        {
	        SetVideoWindowSettins(pMF->m_pVideo, 
                                  pMF->m_pVideoBox->m_VBox.m_VSL1.m_pos, 
                                  pMF->m_pVideoBox->m_VBox.m_VSL2.m_pos, 
                                  pMF->m_pVideoBox->m_VBox.m_HSL1.m_pos, 
                                  pMF->m_pVideoBox->m_VBox.m_HSL2.m_pos);

		    pMF->m_BegTime = SearchSubtitles( pMF->m_pVideo, pMF->m_BegTime, pMF->m_EndTime );
        }
	}

	if (IsClose == 1) 
	{
		IsSearching = 0;
		SendMessage(pMF->m_hWnd, WM_CLOSE, 0, 0);
		return 0;
	}

	pMF->m_pPanel->m_SHPanel.m_Run.SetWindowText("Run Search");

	HBITMAP	hbm = pMF->m_pVideoBox->m_hbm;
	HDC		hdc = pMF->m_pVideoBox->m_hdc;

	pMF->m_pVideoBox->m_hbm = NULL;
	pMF->m_pVideoBox->m_hdc = NULL;

	if (hbm != NULL)
	{
		DeleteObject(hbm);
		DeleteObject(hdc);
		hbm = NULL;
	}

	pMF->m_pPanel->m_SSPanel.EnableWindow(1);
	pMF->m_pPanel->m_OCRPanel.EnableWindow(1);

	SendMessage(pMF->m_hWnd, WM_COMMAND, ID_FILE_REOPENVIDEO, 0);
	
	IsSearching = 0;

	if ( (g_RunSubSearch == 1) && (g_CLEAN_RGB_IMAGES == true) )
	{
		pMF->m_pPanel->m_OCRPanel.OnBnClickedCreateClearedTextImages();
	}

	return 0;
}