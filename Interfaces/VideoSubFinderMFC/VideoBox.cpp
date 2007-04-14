                              //VideoBox.cpp//                                
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
#include "videobox.h"

CVideoWnd::CVideoWnd()
{
	m_pVB = NULL;
}

CVideoWnd::~CVideoWnd()
{
}

BEGIN_MESSAGE_MAP(CVideoWnd, CWnd)
	ON_WM_PAINT()
	ON_WM_MOUSEMOVE()
	ON_WM_SETFOCUS()
END_MESSAGE_MAP()

void CVideoWnd::OnPaint()
{
	CPaintDC dc(this);

	if ( (m_pVB==NULL) || (m_pVB->m_hbm==NULL) )	return;

	int w, h;
	CRect rect;
	BOOL bln;

	this->GetClientRect(rect);

	w = rect.Width();
	h = rect.Height();

	int res = SetStretchBltMode(dc.m_hDC, HALFTONE);
	if (res == 0) SetStretchBltMode(dc.m_hDC, COLORONCOLOR);

	bln = StretchBlt( dc.m_hDC,
			0,0, 
			w, h,
			m_pVB->m_hdc, 
			0,0, 
			m_pVB->m_w, m_pVB->m_h,
			SRCCOPY);
}

CVideoWindow::CVideoWindow()
{
	m_WasInited = false;
	m_pVB = NULL;
}

CVideoWindow::~CVideoWindow()
{
}

BEGIN_MESSAGE_MAP(CVideoWindow, CWnd)
	ON_WM_SIZE()
END_MESSAGE_MAP()

void CVideoWindow::Init(CVideoBox* pParent)
{
	CString strVWClass;
	CString strVWNDClass;

	m_pVB = pParent;

	m_CL1Brush.CreateSolidBrush(RGB(125, 125, 125));

	strVWClass = AfxRegisterWndClass(CS_VREDRAW | CS_HREDRAW,
									LoadCursor(NULL, IDC_ARROW),
									(HBRUSH) (COLOR_WINDOW),
									NULL);

	strVWNDClass = AfxRegisterWndClass(CS_VREDRAW | CS_HREDRAW,
									LoadCursor(NULL, IDC_ARROW),
									m_CL1Brush,
									NULL);

	Create( strVWClass, 
			"", 
			WS_CLIPSIBLINGS | WS_VISIBLE,
			CRect(50,50,250,250),
			pParent,
			ID_VBOX );
	
	m_VideoWnd.Create(strVWNDClass, 
			"", 
			WS_CLIPSIBLINGS | WS_VISIBLE | WS_BORDER,
			CRect(0,0,200,10),
			this,
			57809 );

	m_VideoWnd.m_pVB = pParent;

	m_HSL1.DoCreate(this, 200, 3, 7, 3, 100, 110, 50, 0, 57810);
	m_HSL1.m_pos = 0;
	m_HSL2.DoCreate(this, 200, 3, 7, 3, 140, 150, 50, 0, 57811);
	m_HSL2.m_pos = 1;
	m_VSL1.DoCreate(this, 3, 100, 3, 7, 100, 110, 50, 1, 57812);
	m_VSL1.m_pos = 0;
	m_VSL2.DoCreate(this, 3, 100, 3, 7, 140, 150, 50, 1, 57813);
	m_VSL2.m_pos = 1;

	ResizeControls();

	Update();

	m_WasInited = true;
}

void CVideoWindow::ResizeControls()
{
	CRect rcCL, rcVWND, rcCLVWND;

	this->GetClientRect(rcCL);
	
	rcVWND.left = rcCL.left+9;
	rcVWND.right = rcCL.right-9;
	rcVWND.top = rcCL.top+9;
	rcVWND.bottom = rcCL.bottom-9;

	m_VideoWnd.MoveWindow(&rcVWND);

	if (m_pVB->m_pParent->m_VIsOpen) 
	{
		m_VideoWnd.GetClientRect(rcCLVWND);
		m_pVB->m_pParent->m_pVideo->SetVideoWindowPosition(0, 0, rcCLVWND.right, rcCLVWND.bottom);
	}

	m_HSL1.m_offset = rcVWND.left-2;
	m_HSL1.m_w = rcVWND.Width()+4;
	m_HSL1.m_min = rcVWND.top;
	m_HSL1.m_max = rcVWND.bottom;
	//m_HSL1.m_pos = 0;

	m_HSL2.m_offset = m_HSL1.m_offset;
	m_HSL2.m_w = m_HSL1.m_w;
	m_HSL2.m_min = rcVWND.top;
	m_HSL2.m_max = rcVWND.bottom;
	//m_HSL2.m_pos = 1;

	m_VSL1.m_offset = rcVWND.top-2;
	m_VSL1.m_h = rcVWND.Height()+4;
	m_VSL1.m_min = rcVWND.left;
	m_VSL1.m_max = rcVWND.right;
	//m_VSL1.m_pos = 0;

	m_VSL2.m_offset = m_VSL1.m_offset;
	m_VSL2.m_h = m_VSL1.m_h;
	m_VSL2.m_min = rcVWND.left;
	m_VSL2.m_max = rcVWND.right;
	//m_VSL2.m_pos = 1;

	m_HSL1.Update();
	m_HSL2.Update();
	m_VSL1.Update();
	m_VSL2.Update();
}

void CVideoWindow::Update()
{
	/*CRect rcHSL1, rcHSL2, rcVSL1, rcVSL2;

	m_HSL1.GetWindowRect(rcHSL1);
	this->ScreenToClient(rcHSL1);

	m_HSL2.GetWindowRect(rcHSL2);
	this->ScreenToClient(rcHSL2);

	m_VSL1.GetWindowRect(rcVSL1);
	this->ScreenToClient(rcVSL1);

	m_VSL2.GetWindowRect(rcVSL2);
	this->ScreenToClient(rcVSL2);

	m_HSL1.m_max = rcHSL2.top-10;
	m_HSL1.m_pos = m_HSL1.CalculateCurPos();
	m_HSL1.Update();

	m_HSL2.m_min = rcHSL1.bottom+10;
	m_HSL2.m_pos = m_HSL2.CalculateCurPos();
	m_HSL2.Update();

	m_VSL1.m_max = rcVSL2.left-10;
	m_VSL1.m_pos = m_VSL1.CalculateCurPos();
	m_VSL1.Update();

	m_VSL2.m_min = rcVSL1.right+10;
	m_VSL2.m_pos = m_VSL2.CalculateCurPos();
	m_VSL2.Update();*/

	m_HSL1.m_pos_max = m_HSL2.m_pos-0.05;
	m_HSL2.m_pos_min = m_HSL1.m_pos+0.05;
	m_VSL1.m_pos_max = m_VSL2.m_pos-0.05;
	m_VSL2.m_pos_min = m_VSL1.m_pos+0.05;

	if (m_pVB->m_pParent->m_VIsOpen) 
	{
		if (m_pVB->m_pParent->m_vs != CMainFrame::Play)
		{
			s64 Cur;
			Cur = m_pVB->m_pParent->m_pVideo->GetPos();
			m_pVB->m_pParent->m_pVideo->SetPos(Cur);
		}
	}
}

void CVideoWindow::OnSize(UINT nType, int cx, int cy)
{
	CWnd::OnSize(nType, cx, cy);

	if (m_WasInited) ResizeControls();
}

CVideoBox::CVideoBox()
{
	m_w = 0;
	m_h = 0;
	m_hbm = NULL;

	m_WasInited = false;
}

CVideoBox::~CVideoBox()
{
	if (m_hbm != NULL)
	{
		DeleteObject(m_hbm);
		DeleteObject(m_hdc);
	}
}

BEGIN_MESSAGE_MAP(CVideoBox, CMDIChildWnd)
	ON_BN_CLICKED(ID_TB_RUN, OnBnClickedRun)
	ON_BN_CLICKED(ID_TB_PAUSE, OnBnClickedPause)
	ON_BN_CLICKED(ID_TB_STOP, OnBnClickedStop)
	ON_WM_CTLCOLOR()
	ON_WM_SIZE()
	ON_WM_MOUSEWHEEL()
	ON_WM_PAINT()
	ON_WM_HSCROLL()
END_MESSAGE_MAP()

void CVideoBox::DoDataExchange(CDataExchange* pDX)
{
	CMDIChildWnd::DoDataExchange(pDX);	
}

void CVideoBox::Init(CMDIFrameWnd* pParent)
{
	CString strVBClass;
	CString strVBXClass;

	m_pParent = (CMainFrame*)pParent; 

	m_VBXBrush.CreateSolidBrush(RGB(125, 125, 125));
	
	strVBClass = AfxRegisterWndClass(CS_VREDRAW | CS_HREDRAW,
									LoadCursor(NULL, IDC_ARROW),
									(HBRUSH) (COLOR_WINDOW),
									NULL);

	strVBXClass = AfxRegisterWndClass(CS_VREDRAW | CS_HREDRAW,
									LoadCursor(NULL, IDC_ARROW),
									m_VBXBrush,
									NULL);

	Create( strVBClass, 
			"", 
			WS_CHILD | WS_VISIBLE | WS_THICKFRAME | WS_CAPTION,
			CRect(0,0,400,300),
			pParent,
			0 );

	m_CL1Brush.CreateSolidBrush(RGB(255, 255, 225));
	m_CL2Brush.CreateSolidBrush(RGB(0, 0, 0));

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

	m_VBar.CreateEx(this, TBSTYLE_FLAT, WS_CHILD | WS_VISIBLE | CBRS_BOTTOM | CBRS_BORDER_3D );
	m_VBar.LoadToolBar(IDR_VIDEOBAR);
	m_VBar.SetHeight(30);

	m_lblVB.Create("Video Box", WS_CHILD | WS_VISIBLE | SS_CENTER | SS_CENTERIMAGE | WS_BORDER,
					CRect(0,0,390,30), this, ID_LBL_VB);
	m_lblVB.SetFont(&m_LBLFont);
	
	m_lblTIME.Create("00:00:00,000/00:00:00,000", WS_CHILD | WS_VISIBLE | SS_CENTER | SS_CENTERIMAGE | WS_BORDER,
					CRect(200,242,390,268), this, ID_LBL_TIME);
	m_lblTIME.SetFont(&m_LBLFont);

	m_VBox.Init(this);

	this->MoveWindow(0,0,402,300);

	m_VBar.SetButtonStyle(0, TBBS_CHECKBOX );
	m_VBar.SetButtonStyle(1, TBBS_CHECKBOX );
	m_VBar.SetButtonStyle(2, TBBS_CHECKBOX );

	m_TBr.Create("", WS_CHILD | WS_VISIBLE | WS_BORDER,
					CRect(0,0,200,13), this, ID_TRACKBAR);

	m_TB.CreateFromStatic(SBS_HORZ | WS_CHILD | SS_LEFT | SS_NOTIFY | WS_VISIBLE,
						   this, ID_TRACKBAR, IDC_SCROLL1);
	m_TB.SetScrollRange(0, 255);
    m_TB.SetThumbColor(RGB(255,0,0));

//	m_TBr.EnableThumbGripper(m_bThumbGripper);

	ResizeControls();	

	m_WasInited = true;
}

void CVideoBox::ResizeControls()
{
	CRect rcClVB, rlVB, rlTIME, rcTB, rcVBOX;

	this->GetClientRect(rcClVB);
	
	rlVB.left = 0;
	rlVB.right = rcClVB.right;
	rlVB.top = 0;
	rlVB.bottom = 28;

	rlTIME.right = rcClVB.right - 2;
	rlTIME.left = rlTIME.right - 150;
	rlTIME.top = rcClVB.bottom - 30 + 4;
	rlTIME.bottom = rlTIME.top + 23;

	rcTB.left = 2;
	rcTB.right = rcClVB.right - 2;
	rcTB.top = rcClVB.bottom - 48;
	rcTB.bottom = rcTB.top + 13;

	rcVBOX.left = 0;
	rcVBOX.right = rcClVB.right;
	rcVBOX.top = rlVB.bottom;
	rcVBOX.bottom = rcTB.top;

	m_lblVB.MoveWindow(&rlVB);
	m_VBox.MoveWindow(&rcVBOX);
	m_TB.MoveWindow(&rcTB);
	m_TB.SetScrollPos(m_TB.GetScrollPos());
	m_lblTIME.MoveWindow(&rlTIME);
	m_lblTIME.BringWindowToTop();
}

void CVideoBox::OnBnClickedRun()
{
	if (m_pParent->m_VIsOpen)
	{
		m_VBar.SetButtonStyle(0, TBBS_CHECKBOX | TBBS_CHECKED);
		m_VBar.SetButtonStyle(1, TBBS_CHECKBOX);
		m_VBar.SetButtonStyle(2, TBBS_CHECKBOX);

		m_pParent->m_pVideo->Run();
		m_pParent->m_vs = m_pParent->Play;
	}
	else
	{
		m_VBar.SetButtonStyle(0, TBBS_CHECKBOX);
		m_VBar.SetButtonStyle(1, TBBS_CHECKBOX);
		m_VBar.SetButtonStyle(2, TBBS_CHECKBOX);
	}
}

void CVideoBox::OnBnClickedPause()
{
	if (m_pParent->m_VIsOpen)
	{
		m_VBar.SetButtonStyle(0, TBBS_CHECKBOX);
		m_VBar.SetButtonStyle(1, TBBS_CHECKBOX | TBBS_CHECKED);
		m_VBar.SetButtonStyle(2, TBBS_CHECKBOX);
		
		m_pParent->m_pVideo->Pause();		
		m_pParent->m_vs = m_pParent->Pause;
	}
	else
	{
		m_VBar.SetButtonStyle(0, TBBS_CHECKBOX);
		m_VBar.SetButtonStyle(1, TBBS_CHECKBOX);
		m_VBar.SetButtonStyle(2, TBBS_CHECKBOX);
	}
}

void CVideoBox::OnBnClickedStop()
{
	if (m_pParent->m_VIsOpen)
	{
		m_pParent->OnStop();
	}
	else
	{
		m_VBar.SetButtonStyle(0, TBBS_CHECKBOX);
		m_VBar.SetButtonStyle(1, TBBS_CHECKBOX);
		m_VBar.SetButtonStyle(2, TBBS_CHECKBOX);
	}
}

HBRUSH CVideoBox::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	HBRUSH hbr = CMDIChildWnd::OnCtlColor(pDC, pWnd, nCtlColor);

	switch (pWnd->GetDlgCtrlID())
	{
		case ID_LBL_VB:
			pDC->SetBkMode(TRANSPARENT);
			hbr = m_CL1Brush;
			break;
		
		case ID_LBL_TIME:
			pDC->SetBkMode(TRANSPARENT);
			pDC->SetTextColor(RGB(255, 255, 255));
			hbr = m_CL2Brush;
			break;
	}

	return hbr;
}

void CVideoBox::OnSize(UINT nType, int cx, int cy)
{
	CMDIChildWnd::OnSize(nType, cx, cy);

	if (m_WasInited) ResizeControls();
}

BOOL CVideoBox::PreTranslateMessage(MSG* pMsg)
{
	if (pMsg->message == WM_KEYDOWN)
	{
		s64 Cur;

		if (!m_pParent->m_VIsOpen) return CMDIChildWnd::PreTranslateMessage(pMsg);

		switch (pMsg->wParam)
		{
			case VK_RIGHT:
				m_pParent->PauseVideo();
				m_pParent->m_pVideo->OneStep();
				return true;

			case VK_UP:
				m_pParent->PauseVideo();
				m_pParent->m_pVideo->OneStep();
				return true;

			case VK_LEFT:
				m_pParent->PauseVideo();
				Cur = m_pParent->m_pVideo->GetPos();
				Cur -= m_pParent->m_dt;
				if (Cur < 0) Cur = 0;
				m_pParent->m_pVideo->SetPosFast(Cur);
				return true;
			
			case VK_DOWN:
				m_pParent->PauseVideo();
				Cur = m_pParent->m_pVideo->GetPos();
				Cur -= m_pParent->m_dt;
				if (Cur < 0) Cur = 0;
				m_pParent->m_pVideo->SetPosFast(Cur);
				return true;

			default:
				return CMDIChildWnd::PreTranslateMessage(pMsg);
		}
	}

	return CMDIChildWnd::PreTranslateMessage(pMsg);
}

BOOL CVideoBox::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt)
{
	if (m_pParent->m_VIsOpen)
	{
		s64 Cur;

		if (zDelta>0)
		{
			m_pParent->PauseVideo();
			m_pParent->m_pVideo->OneStep();
		}
		else
		{
			m_pParent->PauseVideo();
			Cur = m_pParent->m_pVideo->GetPos();
			Cur -= m_pParent->m_dt;
			if (Cur < 0) Cur = 0;
			m_pParent->m_pVideo->SetPosFast(Cur);
		}
	}

	return CMDIChildWnd::OnMouseWheel(nFlags, zDelta, pt);
}

void CVideoBox::ViewImage(int *Im, int w, int h)
{
	int i, x, y;
	HDC hdc;
	u8 *color;

	hdc = ::GetDC(m_VBox.m_hWnd);

	if (m_hbm == NULL) 
	{
		m_hbm = CreateCompatibleBitmap(hdc, w, h); 
		m_hdc = CreateCompatibleDC(hdc);
		SelectObject(m_hdc, m_hbm);
		m_w = w;
		m_h = h;
	}
	else
	{
		if ((m_w!=w) || (m_h!=h))
		{
			DeleteObject(m_hbm);
			DeleteObject(m_hdc);
			
			m_hbm = CreateCompatibleBitmap(hdc, w, h);
			m_hdc = CreateCompatibleDC(hdc);
			SelectObject(m_hdc, m_hbm);
			m_w = w;
			m_h = h;
		}
	}

	for(y=0, i=0; y<h; y++)
	for(x=0; x<w; x++, i++)
	{
		color = (u8*)(&Im[i]);
		SetPixel(m_hdc, x, y, RGB(color[2], color[1], color[0]));
	}

	::ReleaseDC(m_VBox.m_hWnd, hdc);

	m_VBox.RedrawWindow();
}

void CVideoBox::OnPaint()
{	
	CWnd::OnPaint();
}

void CVideoBox::Update()
{	
	//m_VBox.RedrawWindow(0,0,RDW_INVALIDATE | RDW_UPDATENOW);
	/*m_HSL1.RedrawWindow();
	m_HSL2.RedrawWindow();
	m_VSL1.RedrawWindow();
	m_VSL2.RedrawWindow();*/
	/*m_HSL1.RedrawWindow(0,0,RDW_INVALIDATE | RDW_UPDATENOW);
	m_HSL2.RedrawWindow(0,0,RDW_INVALIDATE | RDW_UPDATENOW);
	m_VSL1.RedrawWindow(0,0,RDW_INVALIDATE | RDW_UPDATENOW);
	m_VSL2.RedrawWindow(0,0,RDW_INVALIDATE | RDW_UPDATENOW);*/
}
void CVideoBox::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	if (m_pParent->m_VIsOpen) 
	{
		s64 Cur, Pos, endPos;
		s64 SP;

		if (nPos != 0)
		{
			SP = (s64)m_TB.GetScrollPos();

			m_pParent->PauseVideo();
			
			Pos = SP*(s64)10000;

			endPos = m_pParent->m_pVideo->m_Duration;
			Cur = m_pParent->m_pVideo->GetPos();

			if (Pos != Cur)
			{
				m_pParent->m_pVideo->SetPosFast(Pos);
			}
		}
		else
		{
			if (nSBCode == 1)
			{
				m_pParent->PauseVideo();
				m_pParent->m_pVideo->OneStep();
			}
			else
			{
				m_pParent->PauseVideo();
				Cur = m_pParent->m_pVideo->GetPos();
				Cur -= m_pParent->m_dt;
				if (Cur < 0) Cur = 0;
				m_pParent->m_pVideo->SetPosFast(Cur);				
			}
		}
	}

	CMDIChildWnd::OnHScroll(nSBCode, nPos, pScrollBar);
}

void CVideoWnd::OnMouseMove(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default

	CWnd::OnMouseMove(nFlags, point);
}

void CVideoWnd::OnSetFocus(CWnd* pOldWnd)
{
	CWnd::OnSetFocus(pOldWnd);

	m_pVB->SetFocus();
}
