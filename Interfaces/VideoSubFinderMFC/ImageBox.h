                              //ImageBox.h//                                
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
#include "myresource.h"
#include "MainFrm.h"

class CImageBox;

class CImageWnd : public CWnd
{
public:
	CImageWnd();
	virtual ~CImageWnd();
	
	CImageBox *m_pParent;

	HDC hdcCompatible; 


protected:
	DECLARE_MESSAGE_MAP()

public:
	afx_msg void OnPaint();
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
};

class CImageBox : public CMDIChildWnd
{
public:
	CImageBox();           // protected constructor used by dynamic creation
	virtual ~CImageBox();

	CFont		m_LBLFont;

	CStatic		m_lblIB;
	CImageWnd	m_IBox;

	CBrush		m_IBXBrush;
	CBrush		m_CL1Brush;

	bool		m_WasInited;

	HDC			m_hdc;
	HBITMAP		m_hbm;
	int			m_w;
	int			m_h;

	CMainFrame*	m_pParent;

public:
	void Init(CMDIFrameWnd* pParent);
	void ResizeControls();

	void ViewRGBImage(int *Im, int w, int h);
	void ViewGrayscaleImage(int *Im, int w, int h);
	void ViewImage(int *Im, int w, int h);
	void ClearScreen();

protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg void OnSize(UINT nType, int cx, int cy);
};


