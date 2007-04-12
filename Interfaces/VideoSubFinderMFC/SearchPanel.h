
//////////////////////////////////////////////////////////////////////////////////
//							SearchPanel.h  Version 1.75							//
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
#include "SSOWnd.h"

class CMainFrame;
class CSSOWnd;

extern int IsSearching;
extern int IsClose;

class CSearchPanel : public CWnd
{
public:
	CSearchPanel();
	virtual ~CSearchPanel();

	CFont    m_BTNFont;
	CFont    m_LBLFont;

	CButton  m_Clear;
	CButton  m_Run;
	
	CStatic  m_lblBT1;
	CStatic  m_lblBTA1;
	CStatic  m_lblBT2;
	CStatic  m_lblBTA2;
	
	CBrush   m_SPBrush;
	CBrush   m_CL1Brush;
	CBrush   m_CL2Brush;

	CSSOWnd		*m_pParent;

	CMainFrame	*m_pMainFrm;

	DWORD  dwSearchThreadID; 
	HANDLE hSearchThread;

	void Init(CSSOWnd* pParent);

protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg void OnBnClickedRun();
	afx_msg void OnBnClickedClear();
};

DWORD WINAPI ThreadSearchSubtitles(PVOID pParam);