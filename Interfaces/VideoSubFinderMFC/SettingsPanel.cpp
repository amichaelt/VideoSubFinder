                              //SettingsPanel.cpp//                                
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

#include "stdafx.h"
#include "myresource.h"
#include "VideoSubFinder.h"
#include "SettingsPanel.h"

CString	StrFN[6];

CSettingsPanel::CSettingsPanel()
{
	StrFN[0] = "VEdges Points Image";
	StrFN[1] = "NEdges Points Image";
	StrFN[2] = "HEdges Points Image";
	StrFN[3] = "After First Filtration";
	StrFN[4] = "After Second Filtration";
	StrFN[5] = "After Third Filtration";
	m_cn = 5;
	m_n = 6; 
	m_w = 10;
	m_h = 10;
}

CSettingsPanel::~CSettingsPanel()
{
}

BEGIN_MESSAGE_MAP(CSettingsPanel, CWnd)
	ON_WM_CTLCOLOR()
	ON_BN_CLICKED(ID_TEST, OnBnClickedTest)
	ON_BN_CLICKED(ID_SP_LEFT, OnBnClickedLeft)
	ON_BN_CLICKED(ID_SP_RIGHT, OnBnClickedRight)
	ON_WM_SETCURSOR()
	ON_WM_CREATE()
	ON_WM_PAINT()
END_MESSAGE_MAP()

void CSettingsPanel::Init(CSSOWnd* pParent)
{
	CString strSPClass;

	m_pParent = pParent;
	m_pMainFrm = pParent->m_pParent;

	m_SPBrush.CreateSolidBrush(RGB(170,170,170));

	strSPClass = AfxRegisterWndClass(CS_VREDRAW | CS_HREDRAW,
									LoadCursor(NULL, IDC_ARROW),
									m_SPBrush,
									NULL);

	Create( strSPClass, 
			"", 
			WS_CHILD | WS_VISIBLE | WS_BORDER,
			CRect(10,10,400,300),
			pParent,
			ID_PNL_SETTINGS );

	m_BTNFont.CreateFont(
		22,                        // nHeight
		0,                         // nWidth
		0,                         // nEscapement
		0,                         // nOrientation
		FW_NORMAL,                 // nWeight
		FALSE,                     // bItalic
		FALSE,                     // bUnderline
		0,                         // cStrikeOut
		ANSI_CHARSET,              // nCharSet
		OUT_DEFAULT_PRECIS,        // nOutPrecision
		CLIP_DEFAULT_PRECIS,       // nClipPrecision
		DEFAULT_QUALITY,           // nQuality
		DEFAULT_PITCH | FF_SWISS,  // nPitchAndFamily
		"Arial Black");			   // lpszFacename

	m_LBLFont.CreateFont(
		14,                        // nHeight
		0,                         // nWidth
		0,                         // nEscapement
		0,                         // nOrientation
		FW_NORMAL,                 // nWeight
		FALSE,                     // bItalic
		FALSE,                     // bUnderline
		0,                         // cStrikeOut
		ANSI_CHARSET,              // nCharSet
		OUT_DEFAULT_PRECIS,        // nOutPrecision
		CLIP_DEFAULT_PRECIS,       // nClipPrecision
		DEFAULT_QUALITY,           // nQuality
		DEFAULT_PITCH | FF_SWISS,  // nPitchAndFamily
		"Microsoft Sans Serif");   // lpszFacename

	CObjectInspector::CProperty *pProp;
	CRect rcP2, rcClP2, rcGB1, rcGB2, rcGB3; 
	CRect rcTEST, rcLeft, rcRight, rlIF;
	RECT rcOI, rcOIM;

	int bbw, bbh;
	int dw, dh;

	this->GetClientRect(rcClP2);
	this->GetWindowRect(rcP2);
	
	dw = rcP2.Width() - rcClP2.Width();
	dh = rcP2.Height() - rcClP2.Height();

	m_CL1Brush.CreateSolidBrush(RGB(255, 215, 0));
	m_CL2Brush.CreateSolidBrush(RGB(127, 255, 0));
	m_CL3Brush.CreateSolidBrush(RGB(127, 255, 212));

	rcGB1.left = 10;
	rcGB1.right = rcGB1.left + 370;
	rcGB1.top = 2;
	rcGB1.bottom = rcGB1.top + 200;

	rcOI.left = rcGB1.left + 3;
	rcOI.right = rcGB1.right -3;
	rcOI.top = rcGB1.top + 15;
	rcOI.bottom = rcGB1.bottom -3;

	rcGB2.left = rcGB1.right+5;
	rcGB2.right = rcGB2.left + rcGB1.Width();
	rcGB2.top = rcGB1.top;
	rcGB2.bottom = rcGB1.bottom;

	rcOIM.left = rcGB2.left + 3;
	rcOIM.right = rcGB2.right -3;
	rcOIM.top = rcGB2.top + 15;
	rcOIM.bottom = rcGB2.bottom -3;
	
	rcTEST.left = rcGB2.right + 70;
	rcTEST.right = rcTEST.left + 100;
	rcTEST.top = 70;
	rcTEST.bottom = rcTEST.top + 30;

	bbw = 23;
	bbh = 22;

	rcLeft.left = rcTEST.left - 50;
	rcLeft.right = rcLeft.left + bbw;
	rcLeft.top = rcTEST.top - 30;
	rcLeft.bottom = rcLeft.top  + bbh;

	rcRight.left = rcTEST.right + (rcTEST.left - rcLeft.right);
	rcRight.right = rcRight.left + bbw;
	rcRight.top = rcLeft.top;
	rcRight.bottom = rcLeft.bottom;

	rlIF.left = rcLeft.right + 2;
	rlIF.right = rcRight.left - 2;
	rlIF.top = rcLeft.top;
	rlIF.bottom = rcLeft.bottom;

	rcGB3.left = rcGB2.right+5;
	rcGB3.right = rcGB2.right + 2*(rcTEST.left+rcTEST.Width()/2-rcGB2.right)-5;
	rcGB3.top = rcGB2.top;
	rcGB3.bottom = rcGB2.bottom;

	rcP2.left = 10;
	rcP2.right = rcP2.left + rcGB3.right + 10 + dw;
	rcP2.top = 10;
	rcP2.bottom = rcP2.top + (rcGB2.bottom + 2) + dh;

	this->MoveWindow(&rcP2);
	
	m_GB1.Create("Параметры Влияющие На Обработку Изображения", BS_GROUPBOX | WS_CHILD | WS_VISIBLE, 
					rcGB1, this, ID_GB1);
	m_GB1.SetFont(&m_LBLFont);
	
	m_GB2.Create("OCR и Мультифреймовая Обработка Потока Изображений", BS_GROUPBOX | WS_CHILD | WS_VISIBLE, 
					rcGB2, this, ID_GB2);
	m_GB2.SetFont(&m_LBLFont);

	m_GB3.Create("", BS_GROUPBOX | WS_CHILD | WS_VISIBLE, 
					rcGB3, this, ID_GB3);
	m_GB3.SetFont(&m_LBLFont);

	m_Test.Create("Test", WS_CHILD | WS_VISIBLE, rcTEST, this, ID_TEST);
	m_Test.SetFont(&m_BTNFont);	

	m_Left.Create(NULL, WS_CHILD | WS_VISIBLE | BS_OWNERDRAW, rcLeft, this, ID_SP_LEFT);
	m_Left.LoadBitmaps(IDB_LEFT_NA, IDB_LEFT_OD, IDB_LEFT_NA);
	m_Left.SizeToContent();

	m_Right.Create(NULL, WS_CHILD | WS_VISIBLE | BS_OWNERDRAW, rcRight, this, ID_SP_RIGHT);
	m_Right.LoadBitmaps(IDB_RIGHT_NA, IDB_RIGHT_OD, IDB_RIGHT_NA);
	m_Right.SizeToContent();

	m_lblIF.Create(StrFN[m_cn], WS_CHILD | WS_VISIBLE | SS_CENTER | SS_CENTERIMAGE | WS_BORDER,
					rlIF, this, ID_LBL_IF);
	m_lblIF.SetFont(&m_LBLFont);

	m_hCursor = AfxGetApp()->LoadStandardCursor(MAKEINTRESOURCE(32649));

	//////////////////////////////////////////////////////////////////////

	m_OI.Create(rcOI, this, ID_OI, WS_CHILD | WS_VISIBLE | BS_OWNERDRAW/*WS_BORDER*/);

	m_OI.AddProperty (pProp = new CObjectInspector::CProperty("Глобальные Настройки Обработки Изображений", RGB(0, 255, 255)));
	pProp->Expand ();
	pProp->AddProperty (new CObjectInspector::CProperty("Using fast version (partially reduced) : ", &g_fast_search));
	pProp->AddProperty (new CObjectInspector::CProperty("Using MMX and SSE optimization : ", &g_MMX_SSE));	
	m_OI.AddProperty (pProp = new CObjectInspector::CProperty("Первичная Обработка Изображения", RGB(0, 255, 255)));
	m_OI.AddProperty (pProp = new CObjectInspector::CProperty("Настройки Для Операторов Собеля"));
	pProp->Expand ();
	pProp->AddProperty (new CObjectInspector::CProperty("Moderate Threshold : ", &g_mthr, 0.0, 1.0));
	pProp->AddProperty (new CObjectInspector::CProperty("Moderate VEdges Threshold : ", &g_mvthr, 0.0, 1.0));
	pProp->AddProperty (new CObjectInspector::CProperty("Moderate NEdges Threshold : ", &g_mnthr, 0.0, 1.0));
	pProp->AddProperty (new CObjectInspector::CProperty("Moderate HEdges Threshold : ", &g_mhthr, 0.0, 1.0));
	m_OI.AddProperty (pProp = new CObjectInspector::CProperty("Настройки Для Цветовой Фильтрации"));
	pProp->Expand ();
	pProp->AddProperty (new CObjectInspector::CProperty("Segment Width : ", &g_segw, 4, 50));
	pProp->AddProperty (new CObjectInspector::CProperty("Min Segments Count : ", &g_msegc, 1, 10));
	pProp->AddProperty (new CObjectInspector::CProperty("Min Sum Color Difference : ", &g_scd, 0, 10000));
	m_OI.AddProperty (pProp = new CObjectInspector::CProperty("Вторичная Обработка Изображения", RGB(0, 255, 255)));	
	m_OI.AddProperty (pProp = new CObjectInspector::CProperty("Настройки Для Линейной Фильтрации"));
	pProp->Expand ();
	pProp->AddProperty (new CObjectInspector::CProperty("Line Height : ", &g_segh, 1, 50));
	pProp->AddProperty (new CObjectInspector::CProperty("Max Between Text Distance : ", &g_btd, 0.0, 1.0));
	pProp->AddProperty (new CObjectInspector::CProperty("Max Text Centre Offset : ", &g_tco, 0.0, 1.0));
	pProp->AddProperty (new CObjectInspector::CProperty("Max Text Centre Percent Offset : ", &g_tcpo, 0.0, 1.0));
	m_OI.AddProperty (pProp = new CObjectInspector::CProperty("Настройки Для Точек Цветовых Границ"));
	pProp->Expand ();
	pProp->AddProperty (new CObjectInspector::CProperty("Min Points Number : ", &g_mpn, 0, 10000));
	pProp->AddProperty (new CObjectInspector::CProperty("Min Points Density : ", &g_mpd, 0.0, 1.0));
	pProp->AddProperty (new CObjectInspector::CProperty("Min VEdges points density : ", &g_mpved, 0.0, 1.0));
	pProp->AddProperty (new CObjectInspector::CProperty("Min NEdges points density : ", &g_mpned, 0.0, 1.0));
	m_OI.AddProperty (pProp = new CObjectInspector::CProperty("Настройки Для Цветовой Фильтрации"));
	pProp->Expand ();
	pProp->AddProperty (new CObjectInspector::CProperty("Min Sum Multiple Color Difference : ", &g_smcd, 0, 10000));
	m_OI.AddProperty (pProp = new CObjectInspector::CProperty("Третичная Обработка Изображения", RGB(0, 255, 255)));	
	m_OI.AddProperty (pProp = new CObjectInspector::CProperty("Настройки Для Линейной Фильтрации"));
	pProp->Expand ();
	pProp->AddProperty (new CObjectInspector::CProperty("Min VEdges points density (per half line) : ", &g_mpvd, 0.0, 1.0));
	pProp->AddProperty (new CObjectInspector::CProperty("Min HEdges points density (per half line) : ", &g_mphd, 0.0, 1.0));
	pProp->AddProperty (new CObjectInspector::CProperty("Min NEdges points density (per half line) : ", &g_mpnd, 0.0, 1.0));	

	m_OI.SetBorderStyle (CObjectInspector::bsLowered);
	m_OI.SetHotTrack (true);
	m_OI.Set3dFocus (true);
	m_OI.SetStyle(true);
	m_OI.SetMinPropertyColWidth (50);
	m_OI.SetMinValueColWidth (50);
	m_OI.SetHeaderTitles ("Property", "Value");
	m_OI.SetBackColor(::GetSysColor(COLOR_BTNFACE));
	m_OI.SetPropertyHPTColor(RGB(0, 0, 0));
	m_OI.SetPropertyHPColor(RGB(255, 215, 0));
	m_OI.SetPropertyCPColor(RGB(127, 255, 0));
	m_OI.SetPropertyCVPColor(RGB(255, 255, 0));
	m_OI.SetPropertyColTextColor (RGB(0,0,0));
	m_OI.SetValueColTextColor (RGB(0,0,0));
	m_OI.SetGridLineColor (RGB(80,80,80));
	m_OI.SetFocusLineColor (RGB(100,100,255));

	//////////////////////////////////////////////////////////////////////

	m_OIM.Create(rcOIM, this, ID_OIM, WS_CHILD | WS_VISIBLE | BS_OWNERDRAW /*WS_BORDER*/);

	m_OIM.AddProperty (pProp = new CObjectInspector::CProperty("OCR настройки", RGB(0, 255, 255)));	
	pProp->Expand ();
	pProp->AddProperty (new CObjectInspector::CProperty("Clear RGBImages after search subtitles : ", &g_CLEAN_RGB_IMAGES));
	pProp->AddProperty (new CObjectInspector::CProperty("Using hard algorithm for text mining from background : ", &g_hard_sub_mining));
	pProp->AddProperty (new CObjectInspector::CProperty("Using FRDImages for getting TXT areas : ", &g_use_FRD_images));
	pProp->AddProperty (new CObjectInspector::CProperty("Validate And Compare Cleared TXT Images : ", &g_ValidateAndCompareTXTImages));
	pProp->AddProperty (new CObjectInspector::CProperty("Dont Delete Unrecognized Images (First) : ", &g_DontDeleteUnrecognizedImages1));
	pProp->AddProperty (new CObjectInspector::CProperty("Dont Delete Unrecognized Images (Second) : ", &g_DontDeleteUnrecognizedImages2));
	m_OIM.AddProperty (pProp = new CObjectInspector::CProperty("Настройки Мультифреймовой Обработки Изображений", RGB(0, 255, 255)));
	m_OIM.AddProperty (pProp = new CObjectInspector::CProperty("Настройки Для Обнаружения Саба"));
	pProp->Expand ();
	pProp->AddProperty (new CObjectInspector::CProperty("Sub Frames Length : ", &g_DL, 1, 100));
	pProp->AddProperty (new CObjectInspector::CProperty("Sub Square Error : ", &g_sse, 0.0, 1.0));
	m_OIM.AddProperty (pProp = new CObjectInspector::CProperty("Настройки Для Сравнения Сабов"));
	pProp->Expand ();
	pProp->AddProperty (new CObjectInspector::CProperty("VEdges Points line error : ", &g_veple, 0.0, 1.0));
	m_OIM.AddProperty (pProp = new CObjectInspector::CProperty("Настройки Для Проверки Саба"));
	pProp->Expand ();
	pProp->AddProperty (new CObjectInspector::CProperty("Text Procent : ", &g_tp, 0, 1.0));
	pProp->AddProperty (new CObjectInspector::CProperty("Min Text Length : ", &g_mtpl, 0.0, 1.0));

	m_OIM.SetBorderStyle (CObjectInspector::bsLowered);
	m_OIM.SetHotTrack (true);
	m_OIM.Set3dFocus (true);
	m_OIM.SetStyle(true);
	m_OIM.SetMinPropertyColWidth (50);
	m_OIM.SetMinValueColWidth (50);
	m_OIM.SetHeaderTitles ("Property", "Value");
	m_OIM.SetBackColor(::GetSysColor(COLOR_BTNFACE));
	m_OIM.SetPropertyHPTColor(RGB(0, 0, 0));
	m_OIM.SetPropertyHPColor(RGB(255, 215, 0));
	m_OIM.SetPropertyCPColor(RGB(127, 255, 0));
	m_OIM.SetPropertyCVPColor(RGB(255, 255, 0));
	m_OIM.SetPropertyColTextColor (RGB(0,0,0));
	m_OIM.SetValueColTextColor (RGB(0,0,0));
	m_OIM.SetGridLineColor (RGB(80,80,80));
	m_OIM.SetFocusLineColor (RGB(100,100,255));
}

HBRUSH CSettingsPanel::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	HBRUSH hbr = CWnd::OnCtlColor(pDC, pWnd, nCtlColor);

	switch (pWnd->GetDlgCtrlID())
	{
		case ID_LBL_POS:
			pDC->SetBkMode(TRANSPARENT);
			hbr = m_CL1Brush;
			break;
		
		case ID_LBL_MDT:
			pDC->SetBkMode(TRANSPARENT);
			hbr = m_CL2Brush;
			break;

		case ID_LBL_HVT:
			pDC->SetBkMode(TRANSPARENT);
			hbr = m_CL2Brush;
			break;

		case ID_LBL_LNH:
			pDC->SetBkMode(TRANSPARENT);
			hbr = m_CL3Brush;
			break;

		case ID_LBL_PLF:
			pDC->SetBkMode(TRANSPARENT);
			hbr = m_CL1Brush;
			break;
		
		case ID_LBL_BTD:
			pDC->SetBkMode(TRANSPARENT);
			hbr = m_CL2Brush;
			break;

		case ID_LBL_TCO:
			pDC->SetBkMode(TRANSPARENT);
			hbr = m_CL2Brush;
			break;

		case ID_LBL_PPCE:
			pDC->SetBkMode(TRANSPARENT);
			hbr = m_CL1Brush;
			break;
		
		case ID_LBL_MPN:
			pDC->SetBkMode(TRANSPARENT);
			hbr = m_CL2Brush;
			break;

		case ID_LBL_MPD:
			pDC->SetBkMode(TRANSPARENT);
			hbr = m_CL2Brush;
			break;

		case ID_LBL_PCF:
			pDC->SetBkMode(TRANSPARENT);
			hbr = m_CL1Brush;
			break;
		
		case ID_LBL_SW:
			pDC->SetBkMode(TRANSPARENT);
			hbr = m_CL2Brush;
			break;

		case ID_LBL_MSC:
			pDC->SetBkMode(TRANSPARENT);
			hbr = m_CL2Brush;
			break;

		case ID_LBL_SCD:
			pDC->SetBkMode(TRANSPARENT);
			hbr = m_CL2Brush;
			break;

		case ID_LBL_SMCD:
			pDC->SetBkMode(TRANSPARENT);
			hbr = m_CL2Brush;
			break;

		case ID_LBL_PFFS:
			pDC->SetBkMode(TRANSPARENT);
			hbr = m_CL1Brush;
			break;
		
		case ID_LBL_SFL:
			pDC->SetBkMode(TRANSPARENT);
			hbr = m_CL2Brush;
			break;

		case ID_LBL_SSE:
			pDC->SetBkMode(TRANSPARENT);
			hbr = m_CL2Brush;
			break;

		case ID_LBL_PFCS:
			pDC->SetBkMode(TRANSPARENT);
			hbr = m_CL1Brush;
			break;
		
		case ID_LBL_VEPLE:
			pDC->SetBkMode(TRANSPARENT);
			hbr = m_CL2Brush;
			break;

		case ID_LBL_PFVS:
			pDC->SetBkMode(TRANSPARENT);
			hbr = m_CL1Brush;
			break;
		
		case ID_LBL_TP:
			pDC->SetBkMode(TRANSPARENT);
			hbr = m_CL2Brush;
			break;

		case ID_LBL_MTL:
			pDC->SetBkMode(TRANSPARENT);
			hbr = m_CL2Brush;
			break;

		case ID_LBL_PFFTS:
			pDC->SetBkMode(TRANSPARENT);
			hbr = m_CL1Brush;
			break;
		
		case ID_LBL_DE:
			pDC->SetBkMode(TRANSPARENT);
			hbr = m_CL2Brush;
			break;

		case ID_LBL_LLE:
			pDC->SetBkMode(TRANSPARENT);
			hbr = m_CL2Brush;
			break;

		case ID_LBL_IF:
			pDC->SetBkMode(TRANSPARENT);
			hbr = m_CL3Brush;
			break;

		case ID_GB1:
			pDC->SetBkMode(TRANSPARENT);
			hbr = m_SPBrush;
			break;

		case ID_GB2:
			pDC->SetBkMode(TRANSPARENT);
			hbr = m_SPBrush;
			break;
	}

	return hbr;
}

void CSettingsPanel::OnBnClickedTest()
{
	CVideo *pVideo;
	int i, k, w, h, S;
	char str[30];
	clock_t t;
	

	if (m_pMainFrm->m_VIsOpen == false) return;

	//m_pMainFrm->m_Video.SetPos(GetVideoTime(4, 03, 519));

	pVideo = m_pMainFrm->m_pVideo;
	
	SetVideoWindowSettins(m_pMainFrm->m_pVideo, 
                          m_pMainFrm->m_pVideoBox->m_VBox.m_VSL1.m_pos, 
                          m_pMainFrm->m_pVideoBox->m_VBox.m_VSL2.m_pos, 
                          m_pMainFrm->m_pVideoBox->m_VBox.m_HSL1.m_pos, 
                          m_pMainFrm->m_pVideoBox->m_VBox.m_HSL2.m_pos);

	m_w = w = g_w;
	m_h = h = g_h;

	InitIPData((int)m_pMainFrm->m_pVideo->m_Width, (int)m_pMainFrm->m_pVideo->m_Height, 1);

	if (g_fast_search == true)
	{
		memset(g_ImF[0], 0, (g_W*g_H)*sizeof(int));
		memset(g_ImF[1], 0, (g_W*g_H)*sizeof(int));
		memset(g_ImF[2], 0, (g_W*g_H)*sizeof(int));
		memset(g_ImF[3], 0, (g_W*g_H)*sizeof(int));
		memset(g_ImF[4], 0, (g_W*g_H)*sizeof(int));
		memset(g_ImF[5], 0, (g_W*g_H)*sizeof(int));

		t = clock();
		m_pMainFrm->m_pVideo->GetRGBImage(g_ImRGB, g_xmin, g_xmax, g_ymin, g_ymax);
		S = ConvertImage(g_ImRGB, g_ImF[5], g_ImF[0], w, h);
		t = clock()-t;
		
		if (S > 0)
		{
			memcpy(g_ImF[3], g_ImF[5], (w*h)*sizeof(int));
			memcpy(g_ImF[4], g_ImF[5], (w*h)*sizeof(int));
			UnpackImage(g_ImRES2, g_ImF[1], g_pLB, g_pLE, g_LN, w, h);
			UnpackImage(g_ImRES3, g_ImF[2], g_pLB, g_pLE, g_LN, w, h);			
		}
		else
		{
			if (g_blnVNE == 1) 
			{
				UnpackImage(g_ImRES1, g_ImF[0], g_pLB, g_pLE, g_LN, w, h);
				UnpackImage(g_ImRES2, g_ImF[1], g_pLB, g_pLE, g_LN, w, h);
			}
			if (g_blnVNE == 1) 
			{
				UnpackImage(g_ImRES3, g_ImF[2], g_pLB, g_pLE, g_LN, w, h);			
			}
		}
	}
	else
	{
		t = clock();
		S = GetAndConvertImage(g_ImRGB, g_ImF[3], g_ImF[4], g_ImF[5], g_ImF[0], g_ImF[1], g_ImF[2], pVideo, w, h);
		t = clock()-t;

		if (S == 0)
		{
			for(i=0; i<m_n; i++) 
			{
				if (g_ImF[i][0] == -1)
				{
					memset(g_ImF[i], 0, g_W*g_H*sizeof(int));
				}
			}
		}
	}
	
	itoa((int)t, str, 10);
	//this->MessageBox(str);
	
	if (S > 0)
	{
		if ((w != g_W) || (h != g_H))
		{
			ImToNativeSize(g_ImRGB, w, h);
			
			for(k=0; k<m_n; k++)
			{
				ImToNativeSize(g_ImF[k], w, h);
			}
		}
	}	

	//GetFastTransformedImage(Im, g_ImF[5], g_ImF[0], w, h);
	//GetVeryFastTransformedImage(Im, g_ImF[5], g_ImF[0], w, h);

	m_pMainFrm->m_pImageBox->ViewImage(g_ImF[m_cn], g_W, g_H);
	
	SaveRGBImage(g_ImRGB, "\\TSTImages\\RGBImage.jpeg", g_W, g_H);
	
	for (i=0; i<m_n; i++) 
	{		
		itoa(i, str, 10);
		SaveImage(g_ImF[i], string("\\TSTImages\\") + string(str) + string("TSTImage _ ") + string(StrFN[i]) + string(".jpeg"), g_W, g_H);
	}

	return;

	/*//m_pMainFrm->m_Video.SetPos(GetVideoTime(4, 03, 519));

	//m_pMainFrm->GetImage(Im, Res);

	int *Temp, *ImY, *ImU, *ImV, *ImMOE, *ImHOE, *ImVOE, *ImNOE, *ImSOE;
	__int64 *ImALL, *ImYIQ;
	int N;
	int vthr, nthr, mthr;
	clock_t t1, t2;

	Temp = new int[w*h];
	ImY = new int[w*h];
	ImU = new int[w*h];
	ImV = new int[w*h];
	ImMOE = new int[w*h];
	ImHOE = new int[w*h];
	ImVOE = new int[w*h];
	ImNOE = new int[w*h];
	ImSOE = new int[w*h];
	ImALL = new __int64[w*h];
	ImYIQ = new __int64[w*h];
	
	RGB_to_YIQ(Im, ImY, ImU, ImV, w, h);
	memset(ImMOE, 129, w*h*sizeof(int));
	memset(Temp, 0, w*h*sizeof(int));
	ImprovedSobelMEdge(ImY, ImMOE, w, h);
	//AplyESS(ImMOE, Temp, w, h);
	//AplyECP(ImMOE, Temp, m_w, m_h);

	t1 = clock();
	for(i=0; i<1000; i++)
	{
		//GetAndConvertImage(Im, g_ImF[3], g_ImF[4], g_ImF[5], g_ImF[0], g_ImF[1], g_ImF[2], pVideo, w, h);
		//GetTransformedImage(Im, g_ImF[3], g_ImF[4], g_ImF[5], g_ImF[0], g_ImF[1], g_ImF[2], w, h);
		//BorderClear(Im, 2, w, h);
		//EasyBorderClear(Im, w, h);
		//memset(Im, 0, w*h*sizeof(int));
		//ColorFiltration(Im, LB, LE, N, w, h);
		//m_pMainFrm->GetImage(Im, Temp);
		//RGB_to_YIQ(Im, ImY, ImU, ImV, w, h);
		//ApplyModerateThreshold(ImMOE, 0.5, w, h);
		//GetFullTransformedImage(Im, Res, w, h);
		//S = GetAndConvertImage(Im, g_ImF[3], g_ImF[4], g_ImF[5], g_ImF[0], g_ImF[1], g_ImF[2], pVideo, w, h);
		//GetTransformedImage(Im, Res, w, h);
		//AplyESS(ImMOE, Temp, w, h);
		//AplyECP(ImMOE, Temp, w, h);
		//RGB_to_YUV(Im, ImY, ImU, ImU, m_w, m_h);
		//SobelMEdge(ImY, ImMOE, m_w, m_h);	
		//FastImprovedSobelHEdgeOld(ImY, ImHOE, m_w, m_h);
		//ImprovedSobelMEdge(ImY, ImMOE, w, h);
		//FastImprovedSobelVEdgeOld(ImY, ImVOE, w, h);
		//FastImprovedSobelNEdgeOld(ImY, ImNOE, m_w, m_h);
		//SobelSEdge(ImY, ImSOE, m_w, m_h);
	}
	t1 = clock()-t1;

	t2 = clock();
	for(i=0; i<1000; i++)
	{
		//GetAndConvertImage(Im, g_ImF[5], g_ImF[0], pVideo, w, h);
		//GetFastTransformedImage(Im, g_ImF[5], g_ImF[0], w, h);
		GetVeryFastTransformedImage(Im, g_ImF[5], g_ImF[0], w, h);
		//RGB_to_YIQ(Im, ImYIQ, w, h);
		//BorderClear(Im, 2, w, h);
		//EasyBorderClear(Im, w, h);
		//memset(Im, 0, w*h*sizeof(int));
		//ColorFiltration(Im, LB, LE, N, w, h);
		//m_pMainFrm->GetImage(Im, Temp);
		//ApplyModerateThreshold_MMX_SSE(ImMOE, 0.5, w, h);
		//GetFullTransformedImage(Im, Res, w, h);
		//S = GetAndConvertImage(Im, g_ImF[3], g_ImF[4], g_ImF[5], g_ImF[0], g_ImF[1], g_ImF[2], pVideo, w, h);
		//GetTransformedImage(Im, Res, w, h);
		//AplyESS(ImMOE, Temp, w, h);
		//AplyECP(ImMOE, Temp, w, h);
		//RGB_to_YUV(Im, ImY, ImU, ImU, m_w, m_h);
		//SobelMEdge(ImY, ImMOE, m_w, m_h);	
		//FastImprovedSobelHEdge(ImY, ImHOE, m_w, m_h);
		//ImprovedSobelAllEdge(ImY, ImALL, vthr, nthr, hthr, mthr, w, h);
		//ImprovedSobelMEdgeMMX(ImY, ImMOE, w, h);
		//FastImprovedSobelVEdge(ImY, ImVOE, w, h);
		//FastImprovedSobelNEdge(ImY, ImNOE, m_w, m_h);
		//SobelSEdge(ImY, ImSOE, m_w, m_h);
	}
	t2 = clock()-t2;
	
	delete[] Temp;
	delete[] ImY;
	delete[] ImU;
	delete[] ImV;
	delete[] ImMOE;
	delete[] ImHOE;
	delete[] ImVOE;
	delete[] ImNOE;
	delete[] ImSOE;
	delete[] ImALL;
	delete[] ImYIQ;

	itoa((int)t1, str, 10);

	this->MessageBox(str);

	itoa((int)t2, str, 10);

	this->MessageBox(str);*/
}

void CSettingsPanel::OnBnClickedLeft()
{
	m_cn--;
	if (m_cn < 0) m_cn = m_n-1;
	
	m_lblIF.SetWindowText(StrFN[m_cn]);

	if (m_pMainFrm->m_VIsOpen == true)
	{
		m_pMainFrm->m_pImageBox->ViewImage(g_ImF[m_cn], g_W, g_H);
	}
}

void CSettingsPanel::OnBnClickedRight()
{
	m_cn++;
	if (m_cn > m_n-1) m_cn = 0;

	m_lblIF.SetWindowText(StrFN[m_cn]);

	if (m_pMainFrm->m_VIsOpen == true)
	{
		m_pMainFrm->m_pImageBox->ViewImage(g_ImF[m_cn], g_W, g_H);
	}
}
BOOL CSettingsPanel::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message)
{
	if (pWnd == &m_Left)
	{
		::SetCursor(m_hCursor);	
		return TRUE;
	}
	else if(pWnd == &m_Right)
	{
		::SetCursor(m_hCursor);	
		return TRUE;
	}
	else
	{
		return CWnd::OnSetCursor(pWnd, nHitTest, message);
	}
}

int CSettingsPanel::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CWnd::OnCreate(lpCreateStruct) == -1)
		return -1;

	return 0;
}

void CSettingsPanel::OnPaint()
{
	CPaintDC dc(this); // device context for painting
	// TODO: Add your message handler code here
	// Do not call CWnd::OnPaint() for painting messages

	m_OI.Invalidate(0);
	m_OIM.Invalidate(0);
}
