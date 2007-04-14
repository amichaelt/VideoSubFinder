                              //ImageBox.cpp//                                
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
#include ".\imagebox.h"

CImageWnd::CImageWnd()
{
	m_pParent = NULL;
}

CImageWnd::~CImageWnd()
{
}

BEGIN_MESSAGE_MAP(CImageWnd, CWnd)
	ON_WM_PAINT()
	ON_WM_MOUSEMOVE()
END_MESSAGE_MAP()

void CImageWnd::OnPaint()
{
	CPaintDC dc(this);
	
	if (m_pParent==NULL) return;
	
	if (m_pParent->m_hbm==NULL) return;

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
			m_pParent->m_hdc, 
			0,0, 
			m_pParent->m_w, m_pParent->m_h,
			SRCCOPY);
}

CImageBox::CImageBox()
{
	m_w = 0;
	m_h = 0;
	m_hbm = NULL;

	m_WasInited = false;
}

CImageBox::~CImageBox()
{
	if (m_hbm != NULL)
	{
		DeleteObject(m_hbm);
		DeleteObject(m_hdc);
	}
}

BEGIN_MESSAGE_MAP(CImageBox, CMDIChildWnd)
	ON_WM_CTLCOLOR()
	ON_WM_SIZE()
END_MESSAGE_MAP()

void CImageBox::Init(CMDIFrameWnd* pParent)
{
	CString strIBClass;
	CString strIBXClass;

	m_pParent = (CMainFrame*)pParent;

	m_IBXBrush.CreateSolidBrush(RGB(125, 125, 125));
	
	strIBClass = AfxRegisterWndClass(CS_VREDRAW | CS_HREDRAW,
									LoadCursor(NULL, IDC_ARROW),
									(HBRUSH) (COLOR_WINDOW),
									NULL);

	strIBXClass = AfxRegisterWndClass(CS_VREDRAW | CS_HREDRAW,
									LoadCursor(NULL, IDC_ARROW),
									m_IBXBrush,
									NULL);

	Create( strIBClass, 
			"", 
			WS_CHILD | WS_VISIBLE | WS_THICKFRAME | WS_CAPTION,
			CRect(0,0,400,300),
			pParent,
			0 );

	m_CL1Brush.CreateSolidBrush(RGB(255, 255, 225));

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

	m_lblIB.Create("Image Box", WS_CHILD | WS_VISIBLE | SS_CENTER | SS_CENTERIMAGE | WS_BORDER,
					CRect(0,0,390,30), this, ID_LBL_IB);

	m_lblIB.SetFont(&m_LBLFont);

	m_IBox.Create( strIBXClass, 
			"", 
			WS_CHILD | WS_VISIBLE | WS_BORDER,
			CRect(0,0,200,200),
			this,
			ID_IBOX );

	m_IBox.m_pParent = this;

	ResizeControls();	

	m_WasInited = true;
}

void CImageBox::ResizeControls()
{
	CRect rcClIB, rlIB, rcIBOX;

	this->GetClientRect(rcClIB);
	
	rlIB.left = 0;
	rlIB.right = rcClIB.right;
	rlIB.top = 0;
	rlIB.bottom = 28;

	rcIBOX.left = 9;
	rcIBOX.right = rcClIB.right - 9;
	rcIBOX.top = rlIB.bottom + 9;
	rcIBOX.bottom = rcClIB.bottom - 9;

	m_lblIB.MoveWindow(&rlIB);
	m_IBox.MoveWindow(&rcIBOX);
}

void CImageBox::ViewRGBImage(int *Im, int w, int h)
{
	int i, x, y;
	HDC hdc;
	u8 *color;

	hdc = ::GetDC(m_IBox.m_hWnd);

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

	::ReleaseDC(m_IBox.m_hWnd, hdc);

	m_IBox.RedrawWindow();
}

void CImageBox::ViewGrayscaleImage(int *Im, int w, int h)
{
	int i, x, y;
	HDC hdc;
	int color;

	hdc = ::GetDC(m_IBox.m_hWnd);

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
		color = Im[i];
		SetPixel(m_hdc, x, y, RGB(color, color, color));
	}

	::ReleaseDC(m_IBox.m_hWnd, hdc);

	m_IBox.RedrawWindow();
}

void CImageBox::ViewImage(int *Im, int w, int h)
{
	int i, x, y;
	HDC hdc;

	hdc = ::GetDC(m_IBox.m_hWnd);

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

	COLORREF white, black;

	white = RGB(255, 255, 255);
	black = RGB(0, 0, 0);

	for(y=0, i=0; y<h; y++)
	for(x=0; x<w; x++, i++)
	{
		if (Im[i] == 0)	SetPixel(m_hdc, x, y, black);
		else SetPixel(m_hdc, x, y, white);
	}

	::ReleaseDC(m_IBox.m_hWnd, hdc);

	m_IBox.RedrawWindow();
}

void CImageBox::ClearScreen()
{
	if (m_hbm != NULL) 
	{
		int *pixels, size;

		size = m_w*m_h;
		pixels = new int[size];
		memset(pixels, 0, size*sizeof(int));
		SetBitmapBits(m_hbm, size*sizeof(int), pixels);
		delete[] pixels;

		m_IBox.RedrawWindow();
	}
}

HBRUSH CImageBox::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	HBRUSH hbr = CMDIChildWnd::OnCtlColor(pDC, pWnd, nCtlColor);

	switch (pWnd->GetDlgCtrlID())
	{
		case ID_LBL_IB:
			pDC->SetBkMode(TRANSPARENT);
			hbr = m_CL1Brush;
			break;
	}

	return hbr;
}

void CImageBox::OnSize(UINT nType, int cx, int cy)
{
	CMDIChildWnd::OnSize(nType, cx, cy);

	if (m_WasInited) ResizeControls();
}

void CImageWnd::OnMouseMove(UINT nFlags, CPoint point)
{
	CWnd::OnMouseMove(nFlags, point);
}
