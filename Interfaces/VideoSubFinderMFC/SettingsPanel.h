                              //SettingsPanel.h//                                
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
#include "SSOWnd.h"
#include <afxext.h>
#include "ObjectInspector.h"

class CMainFrame;
class CSSOWnd;

class CSettingsPanel : public CWnd
{
public:
	CSettingsPanel();
	virtual ~CSettingsPanel();

	CFont    m_BTNFont;
	CFont    m_LBLFont;

	CStatic m_lblPOS;
	CStatic m_lblMDT;
	CStatic m_lblHVT;
	CStatic m_lblLNH;
	CStatic m_lblPLF;
	CStatic m_lblBTD;
	CStatic m_lblTCO;
	CEdit m_MDT;
	CEdit m_HVT;
	CEdit m_LNH;
	CEdit m_BTD;
	CEdit m_TCO;
	CStatic m_lblPPCE;
	CStatic m_lblMPN;
	CStatic m_lblMPD;
	CStatic m_lblPCF;
	CStatic m_lblSW;
	CStatic m_lblMSC;
	CStatic m_lblSCD;
	CStatic m_lblSMCD;
	CEdit m_MPN;
	CEdit m_MPD;
	CEdit m_SW;
	CEdit m_MSC;
	CEdit m_SCD;
	CEdit m_SMCD;
	CStatic m_lblPFFS;
	CStatic m_lblSFL;
	CStatic m_lblSSE;
	CStatic m_lblPFCS;
	CStatic m_lblVEPLE;
	CStatic m_lblPFVS;
	CStatic m_lblTP;
	CStatic m_lblMTL;
	CEdit m_SFL;
	CEdit m_SSE;
	CEdit m_VEPLE;
	CEdit m_TP;
	CEdit m_MTL;
	CButton m_Test;
	CButton m_GB1;
	CButton m_GB2;
	CButton m_GB3;
	CObjectInspector m_OI;
	CObjectInspector m_OIM;

	CBitmapButton m_Left;
	CBitmapButton m_Right;
	CStatic	m_lblIF;

	int		m_cn;
	int		m_n;

	int		m_w;
	int		m_h;

	CBrush   m_SPBrush;
	CBrush   m_CL1Brush;
	CBrush   m_CL2Brush;
	CBrush   m_CL3Brush;

	HCURSOR  m_hCursor;

	CSSOWnd		*m_pParent;

	CMainFrame	*m_pMainFrm;

	void Init(CSSOWnd* pParent);

protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg void OnBnClickedTest();
	afx_msg void OnBnClickedLeft();
	afx_msg void OnBnClickedRight();
	afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);

public:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnPaint();
};


