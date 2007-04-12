
//////////////////////////////////////////////////////////////////////////////////
//							SeparatingLine.cpp  Version 1.75					//
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
#include ".\separatingline.h"

CSeparatingLine::CSeparatingLine()
{
	m_bDown = false;
	m_pRgn = NULL;
}

CSeparatingLine::~CSeparatingLine()
{
	if (m_pRgn != NULL) delete m_pRgn;
}

BEGIN_MESSAGE_MAP(CSeparatingLine, CWnd)
	ON_WM_PAINT()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_MOUSEMOVE()
END_MESSAGE_MAP()

void CSeparatingLine::CreateNewRgn()
{
	if (m_pRgn != NULL) delete m_pRgn;
	m_pRgn = new CRgn;

	CPoint ps[20];
	int i = 0;

	if (m_orientation == 0)
	{
		ps[i].x = 0;
		ps[i].y = 0;
		i++;

		ps[i].x = m_sw;
		ps[i].y = m_sh;
		i++;

		ps[i].x = m_w+m_sw;
		ps[i].y = m_sh;
		i++;

		ps[i].x = m_w+2*m_sw;
		ps[i].y = 0;
		i++;

		ps[i].x = m_w+m_sw+1;
		ps[i].y = m_sh+(m_h+1)/2+1;
		i++;

		ps[i].x = m_w+m_sw+1;
		ps[i].y = m_sh+(m_h+1)/2-1;
		i++;

		ps[i].x = m_w+2*m_sw;
		ps[i].y = m_sh+m_h+m_sh;
		i++;

		ps[i].x = m_w+m_sw;
		ps[i].y = m_sh+m_h;
		i++;

		ps[i].x = m_sw;
		ps[i].y = m_sh+m_h;
		i++;

		ps[i].x = 0;
		ps[i].y = m_sh+m_h+m_sh;
		i++;

		ps[i].x = m_sw-1;
		ps[i].y = m_sh+(m_h+1)/2-1;
		i++;

		ps[i].x = m_sw-1;
		ps[i].y = m_sh+(m_h+1)/2+1;
		i++;
	}
	else
	{
		ps[i].x = 0;
		ps[i].y = 0;
		i++;

		ps[i].x = m_sw+1+(m_w+1)/2;
		ps[i].y = m_sh-1;
		i++;

		ps[i].x = m_sw-1+(m_w+1)/2;
		ps[i].y = m_sh-1;
		i++;

		ps[i].x = m_w+2*m_sw;
		ps[i].y = 0;
		i++;

		ps[i].x = m_w+m_sw;
		ps[i].y = m_sh;
		i++;

		ps[i].x = m_w+m_sw;
		ps[i].y = m_sh+m_h;
		i++;

		ps[i].x = m_w+2*m_sw;
		ps[i].y = m_h+2*m_sh;
		i++;

		ps[i].x = m_sw-1+(m_w+1)/2;
		ps[i].y = m_h+m_sh+1;
		i++;

		ps[i].x = m_sw+1+(m_w+1)/2;
		ps[i].y = m_h+m_sh+1;
		i++;

		ps[i].x = 0;
		ps[i].y = m_h+2*m_sh;
		i++;

		ps[i].x = m_sw;
		ps[i].y = m_h+m_sh;
		i++;

		ps[i].x = m_sw;
		ps[i].y = m_sh;
		i++;
	}

	BOOL res;

	res = m_pRgn->CreatePolygonRgn(ps, i, WINDING);
	res = SetWindowRgn( *m_pRgn, TRUE );

	m_old_w = m_w;
	m_old_h = m_w;
}

void CSeparatingLine::DoCreate(CWnd* pParentWnd, int w, int h, int sw, int sh, int minpos, int maxpos, int offset, int orientation, UINT nID)
{
	m_pParentWnd = pParentWnd;
	
	m_w = w;
	m_h = h;

	m_sw = sw;
	m_sh = sh;

	m_min = minpos;
	m_max = maxpos;

	m_offset = offset;

	m_orientation = orientation;

	m_pos = 0;
	m_pos_min = 0;
	m_pos_max = 1;

	CString strSLClass;

	CRect rc;

	if (m_orientation == 0)
	{
		strSLClass = AfxRegisterWndClass(CS_VREDRAW | CS_HREDRAW,
									LoadCursor(NULL, IDC_SIZENS),
									(HBRUSH) (COLOR_WINDOW),
									NULL);

		rc.left = m_offset-m_sw;
		rc.right = rc.left+m_w+2*m_sw;
		rc.top = m_min-m_h/2-m_sh;
		rc.bottom = rc.top+m_h+2*m_sh;
	}	
	else
	{
		strSLClass = AfxRegisterWndClass(CS_VREDRAW | CS_HREDRAW,
									LoadCursor(NULL, IDC_SIZEWE),
									(HBRUSH) (COLOR_WINDOW),
									NULL);

		rc.left = m_min-m_w/2-m_sw;
		rc.right = rc.left+m_w+2*m_sw;
		rc.top = m_offset-m_sh;
		rc.bottom = rc.top+m_h+2*m_sh;
	}

	Create(strSLClass, 
			"", 
			WS_CLIPSIBLINGS | WS_BORDER,
			rc,
			pParentWnd,
			nID );

	CreateNewRgn();
	
	this->ShowWindow(1);
	this->BringWindowToTop();

	Update();
}
	
void CSeparatingLine::OnPaint()
{
	CPaintDC dc(this);
	
	CRect rect;
	GetClientRect( rect );
	CBrush blackBrush( RGB(0,0,0) );
	CBrush whiteBrush( RGB(255, 255, 255) );

	CRgn rgn;
	BOOL res;
	if (m_orientation == 0)
		rgn.CreateRectRgn(m_sw, m_sh, m_sw+m_w-2, m_sh+m_h-2);
	else
		rgn.CreateRectRgn(m_sw, m_sh, m_sw+m_w-1, m_sh+m_h-1);

	dc.FillRect( rect, &blackBrush );
	res = dc.FillRgn( &rgn, &whiteBrush );
}

void CSeparatingLine::OnLButtonDown(UINT nFlags, CPoint point)
{
	m_bDown = true;

	SetCapture();

	CWnd::OnLButtonDown(nFlags, point);
}

void CSeparatingLine::OnLButtonUp(UINT nFlags, CPoint point)
{
	if (m_bDown == true) 
	{
		m_bDown = false;
		ReleaseCapture();
		((CVideoWindow*)m_pParentWnd)->Update();
	}

	CWnd::OnLButtonUp(nFlags, point);
}

void CSeparatingLine::OnMouseMove(UINT nFlags, CPoint point)
{
	if (m_bDown == true) 
	{
		RECT rc;

		this->GetWindowRect(&rc);
		m_pParentWnd->ScreenToClient(&rc);

		m_pParentWnd->RedrawWindow(&rc, 0, 256);
		MoveSL();
	}

	CWnd::OnMouseMove(nFlags, point);
}

void CSeparatingLine::MoveSL()
{
	POINT pt;
	int val;
	
	GetCursorPos(&pt); 
	m_pParentWnd->ScreenToClient(&pt);

	if (m_orientation == 0)
		val = pt.y;
	else 
		val = pt.x;

	if (val > m_max)
	{
		m_pos = 1;
	}
	else
	{
		if (val < m_min)
		{
			m_pos = 0;
		}
		else
		{
			m_pos = (double)(val-m_min)/(double)(m_max-m_min);
		}
	}

	if (m_pos < m_pos_min) m_pos = m_pos_min;
	if (m_pos > m_pos_max) m_pos = m_pos_max;

	Update();
}

double CSeparatingLine::CalculateCurPos()
{
	RECT rc;
	int val;
	double res;

	this->GetWindowRect(&rc);
	
	m_pParentWnd->ScreenToClient(&rc);

	if (m_orientation == 0)
	{
		val = rc.top+m_h/2+m_sh;
	}
	else
	{
		val = rc.left+m_w/2+m_sw;
	}

	res = (double)(val-m_min)/(double)(m_max-m_min);

	return res;
}

int CSeparatingLine::GetCurPos()
{
	RECT rc;
	int res;

	this->GetWindowRect(&rc);
	
	m_pParentWnd->ScreenToClient(&rc);

	if (m_orientation == 0)
	{
		res = rc.top+m_h/2+m_sh;
	}
	else
	{
		res = rc.left+m_w/2+m_sw;
	}

	return res;
}

void CSeparatingLine::Update()
{
	CRect rc;
	int pos;

	pos = m_min+(int)(m_pos*(double)(m_max-m_min));

	if ( (m_w != m_old_w) || (m_h != m_old_h) )
	{
		CreateNewRgn();
	}

	if (m_orientation == 0)
	{
		rc.left = m_offset-m_sw;
		rc.right = rc.left+m_w+2*m_sw;
		rc.top = pos-m_h/2-m_sh;
		rc.bottom = rc.top+m_h+2*m_sh;
	}	
	else
	{
		rc.left = pos-m_w/2-m_sw;
		rc.right = rc.left+m_w+2*m_sw;
		rc.top = m_offset-m_sh;
		rc.bottom = rc.top+m_h+2*m_sh;
	}

	this->MoveWindow(&rc); 
}
