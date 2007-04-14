                              //VideoBox.h//                                
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
#include "TrackBar.h"
#include "MainFrm.h"
#include <afxext.h>
#include <afxcmn.h>
#include ".\separatingline.h"
#include ".\XScrollBar.h"

class CVideoBox;

class CVideoWnd : public CWnd
{
public:
	CVideoWnd();
	virtual ~CVideoWnd();

	CVideoBox *m_pVB;

protected:
	DECLARE_MESSAGE_MAP()

public:
	afx_msg void OnPaint();
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnSetFocus(CWnd* pOldWnd);
};

class CVideoWindow: public CWnd
{
public:
	CVideoWindow();
	virtual ~CVideoWindow();

	CVideoWnd m_VideoWnd;
	CSeparatingLine m_HSL1;
	CSeparatingLine m_HSL2;
	CSeparatingLine m_VSL1;
	CSeparatingLine m_VSL2;

	CBrush   m_CL1Brush;

	bool     m_WasInited;

	CVideoBox *m_pVB;

public:
	void Init(CVideoBox* pParent);
	void ResizeControls();
	void Update();

	protected:
	DECLARE_MESSAGE_MAP()

public:
	afx_msg void OnSize(UINT nType, int cx, int cy);
};

class CVideoBox : public CMDIChildWnd
{
public:
	CVideoBox();           // protected constructor used by dynamic creation
	virtual ~CVideoBox();

	CFont    m_LBLFont;

	CToolBar     m_VBar;
	CStatic      m_lblVB;
	CStatic      m_lblTIME;
	CVideoWindow m_VBox;
	CStatic	     m_TBr;
	CXScrollBar  m_TB;

	CBrush   m_VBXBrush;
	CBrush   m_CL1Brush;
	CBrush   m_CL2Brush;

	HDC			m_hdc;
	HBITMAP		m_hbm;
	int			m_w;
	int			m_h;

	CMainFrame* m_pParent;

	bool     m_WasInited;

public:
	void Init(CMDIFrameWnd* pParent);
	void ResizeControls();
	void ViewImage(int *Im, int w, int h);
	void Update();
	
protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support

protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
	afx_msg void OnPaint();
	afx_msg void OnBnClickedRun();
	afx_msg void OnBnClickedPause();
	afx_msg void OnBnClickedStop();
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
};


