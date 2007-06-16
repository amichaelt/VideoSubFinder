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
#include "DataTypes.h"
#include <windows.h>
#include <vector>
#include <fstream>

using namespace std;

extern bool g_use_FRD_images;

extern int g_IsCreateClearedTextImages;
extern int g_RunCreateClearedTextImages;
extern bool g_ValidateAndCompareTXTImages;
extern bool g_DontDeleteUnrecognizedImages1;
extern bool g_DontDeleteUnrecognizedImages2;

extern bool g_CLEAN_RGB_IMAGES;

class CMainFrame;
class CSSOWnd;
class AssTXTStyle;

class AssTXTLine
{
public:
	AssTXTLine();

	AssTXTLine& operator=(const AssTXTLine& other);

	string	m_TXTStr;
	int		m_LH;
	int		m_LY;
	int		m_LXB;
	int		m_LXE;
	int		m_LYB;
	int		m_LYE;

	int		m_mY; // main color in YIQ color system
	int		m_mI;
	int		m_mQ;

	s64		m_BT;
	s64		m_ET;

	int		m_dX; //смещение pos(m_dX, m_dY)
	int		m_dY;

	int		m_Alignment;

	int			 m_AssStyleIndex;
	AssTXTStyle *m_pAssStyle;
};

struct YIQ_LH_Struct
{
	int		m_mY;
	int		m_mI;
	int		m_mQ;

	int		m_LH;
};

class AssTXTStyle
{
public:
	AssTXTStyle();

	vector<YIQ_LH_Struct> m_data;
	
	void Compute();

	int		m_minY;
	int		m_minI;
	int		m_minQ;
	
	int		m_maxY;
	int		m_maxI;
	int		m_maxQ;

	int		m_mY;
	int		m_mI;
	int		m_mQ;

	int		m_minLH;
	int		m_maxLH;

	int		m_LH;

	int		m_Alignment;
	int		m_MarginL;
	int		m_MarginR;
	int		m_MarginV;
	string  m_Name;
};

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
