                              //SeparatingLine.h//                                
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

#pragma once
#include ".\videobox.h"

class CVideoWindow;

class CSeparatingLine : public CWnd
{
public:
	CSeparatingLine();
	virtual ~CSeparatingLine();

	int m_w;
	int m_old_w;
	int m_h;
	int m_old_h;

	int m_sw;
	int m_sh;

	int m_min;
	int m_max;
	int	m_offset;

	int	m_orientation; // 0 horizontal, 1 vertical
	
	double	m_pos;
	double	m_pos_min;
	double	m_pos_max;

	CVideoWindow*	m_pParentWnd;
	CRgn*	m_pRgn;

	bool	m_bDown;

public:
	void DoCreate(CVideoWindow* pParentWnd, int w, int h, int sw, int sh, int minpos, int maxpos, int offset, int orientation, UINT nID);
	void CreateNewRgn();
	void MoveSL();
	void Update();
	double CalculateCurPos();
	int GetCurPos();

protected:
	DECLARE_MESSAGE_MAP()

public:
	afx_msg void OnPaint();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
};