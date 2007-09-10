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
#include "TextBox.h"
#include "DataGrid.h"
#include <wx/panel.h>
#include <wx/bmpbuttn.h>

class CMainFrame;
class CSSOWnd;

class CSettingsPanel : public wxPanel
{
public:
	CSettingsPanel(CSSOWnd* pParent);
	~CSettingsPanel();

	wxFont    m_BTNFont;
	wxFont    m_LBLFont;

	//CStatic m_lblPOS;
	//CStatic m_lblMDT;
	//CStatic m_lblHVT;
	//CStatic m_lblLNH;
	//CStatic m_lblPLF;
	//CStatic m_lblBTD;
	//CStatic m_lblTCO;
	//CEdit m_MDT;
	//CEdit m_HVT;
	//CEdit m_LNH;
	//CEdit m_BTD;
	//CEdit m_TCO;
	//CStatic m_lblPPCE;
	//CStatic m_lblMPN;
	//CStatic m_lblMPD;
	//CStatic m_lblPCF;
	//CStatic m_lblSW;
	//CStatic m_lblMSC;
	//CStatic m_lblSCD;
	//CStatic m_lblSMCD;
	//CEdit m_MPN;
	//CEdit m_MPD;
	//CEdit m_SW;
	//CEdit m_MSC;
	//CEdit m_SCD;
	//CEdit m_SMCD;
	//CStatic m_lblPFFS;
	//CStatic m_lblSFL;
	//CStatic m_lblSSE;
	//CStatic m_lblPFCS;
	//CStatic m_lblVEPLE;
	//CStatic m_lblPFVS;
	//CStatic m_lblTP;
	//CStatic m_lblMTL;
	//CEdit m_SFL;
	//CEdit m_SSE;
	//CEdit m_VEPLE;
	//CEdit m_TP;
	//CEdit m_MTL;

	CDataGrid	*m_pOI;

	wxButton	*m_pTest;
	wxStaticBox *m_pGB1;
	wxStaticBox *m_pGB2;
	wxStaticBox	*m_pGB3;
	wxPanel		*m_pP2;

	//CObjectInspector m_OI;
	//CObjectInspector m_OIM;

	wxBitmapButton *m_pLeft;
	wxBitmapButton *m_pRight;
	CTextBox	   *m_plblIF;

	int		m_cn;
	int		m_n;

	int		m_w;
	int		m_h;

	wxColour   m_CLSP;
	wxColour   m_CL1;
	wxColour   m_CL2;
	wxColour   m_CL3;

	//HCURSOR  m_hCursor;

	CSSOWnd		*m_pParent;

	CMainFrame	*m_pMF;

	void Init();

public:
	void OnBnClickedTest(wxCommandEvent& event);
	void OnBnClickedLeft(wxCommandEvent& event);
	void OnBnClickedRight(wxCommandEvent& event);
	//afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);
	//afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	//afx_msg void OnPaint();

private:
   DECLARE_EVENT_TABLE()
};


