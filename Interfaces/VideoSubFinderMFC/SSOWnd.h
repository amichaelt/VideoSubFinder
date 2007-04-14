                              //SSOWnd.h//                                
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
#include "SearchPanel.h"
#include "SettingsPanel.h"
#include "OCRPanel.h"
#include "MainFrm.h"
#include <afxcmn.h>

class CMainFrame;

class CSSOWnd : public CMDIChildWnd
{
public:
	CSSOWnd();           // protected constructor used by dynamic creation
	virtual ~CSSOWnd();

	CFont			m_LBLFont;
	CTabCtrl		m_Tab;
	CSearchPanel	m_SHPanel;
	CSettingsPanel	m_SSPanel;
	COCRPanel		m_OCRPanel;

	bool			m_WasInited;

	CMainFrame		*m_pParent;

	void Init(CMainFrame* pParent);
	void ResizeControls();

protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnTcnSelchangeTab(NMHDR *pNMHDR, LRESULT *pResult);
};


