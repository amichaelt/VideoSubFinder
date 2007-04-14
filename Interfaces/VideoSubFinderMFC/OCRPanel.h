                              //OCRPanel.h//                                
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
#include "vector"
#include <fstream>

using namespace std;

extern bool g_use_FRD_images;

extern int g_IsCreateClearedTextImages;
extern int g_RunCreateClearedTextImages;
extern bool g_ValidateAndCompareTXTImages;
extern bool g_DontDeleteUnrecognizedImages1;
extern bool g_DontDeleteUnrecognizedImages2;

class CMainFrame;
class CSSOWnd;

class COCRPanel : public CWnd
{
public:
	COCRPanel();
	virtual ~COCRPanel();

	CFont    m_BTNFont;
	CFont    m_LBLFont;

	CStatic m_lblMSD;
	CEdit	m_MSD;
	CButton m_TEST;
	CButton m_CCTI;
	CButton m_CES;
	CButton m_CSCTI;
	CButton m_CSTXT;
	
	CBrush   m_OCRBrush;
	CBrush   m_CL1Brush;

	CSSOWnd		*m_pParent;

	CMainFrame	*m_pMainFrm;

	DWORD		m_dwSearchThreadID; 
	HANDLE		m_hSearchThread;

	void Init(CSSOWnd* pParent);

	void CreateSubFromTXTResults();

protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg void OnBnClickedCreateEmptySub();
	afx_msg void OnBnClickedCreateSubFromClearedTXTImages();
	afx_msg void OnBnClickedCreateSubFromTXTResults();
	afx_msg void OnBnClickedCreateClearedTextImages();
	afx_msg void OnBnClickedTest();
};

DWORD WINAPI ThreadCreateClearedTextImages(PVOID pParam);
