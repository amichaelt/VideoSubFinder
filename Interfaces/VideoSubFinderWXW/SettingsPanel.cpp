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

#include "MyResource.h"
#include "SettingsPanel.h"

string	StrFN[6];

BEGIN_EVENT_TABLE(CSettingsPanel, wxPanel)
	EVT_BUTTON(ID_TEST, CSettingsPanel::OnBnClickedTest)
	EVT_BUTTON(ID_SP_LEFT, CSettingsPanel::OnBnClickedLeft)
	EVT_BUTTON(ID_SP_RIGHT, CSettingsPanel::OnBnClickedRight)
	//ON_WM_SETCURSOR()
	//ON_WM_CREATE()
	//ON_WM_PAINT()
END_EVENT_TABLE()

CSettingsPanel::CSettingsPanel(CSSOWnd* pParent)
		:wxPanel( pParent, wxID_ANY )
{
	m_pParent = pParent;
	m_pMF = pParent->m_pMF;

	StrFN[0] = string("VEdges Points Image");
	StrFN[1] = string("NEdges Points Image");
	StrFN[2] = string("HEdges Points Image");
	StrFN[3] = string("After First Filtration");
	StrFN[4] = string("After Second Filtration");
	StrFN[5] = string("After Third Filtration");
	m_cn = 5;
	m_n = 6; 
	m_w = 10;
	m_h = 10;
}

CSettingsPanel::~CSettingsPanel()
{
}

void CSettingsPanel::Init()
{
	int bbw, bbh, w, h, dw, dh;
	wxRect rcP2, rcClP2, rcGB1, rcGB2, rcGB3; 
	wxRect rcTEST, rcLeft, rcRight, rlIF;
	wxRect rcOI, rcOIM;
	wxBitmap bmp_na, bmp_od;
	//CObjectInspector::CProperty *pProp;

	m_CLSP = wxColour(170,170,170);
	m_CL1 = wxColour(255, 215, 0);
	m_CL2 = wxColour(127, 255, 0);
	m_CL3 = wxColour(127, 255, 212);

	//"Arial Black"
	m_BTNFont = wxFont(12, wxFONTFAMILY_SWISS, wxFONTSTYLE_NORMAL,
                    wxFONTWEIGHT_BOLD, false /* !underlined */,
                    wxEmptyString /* facename */, wxFONTENCODING_DEFAULT);

	//"Microsoft Sans Serif"
	m_LBLFont = wxFont(8, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL,
                    wxFONTWEIGHT_NORMAL, false /* !underlined */,
                    wxEmptyString /* facename */, wxFONTENCODING_DEFAULT);
	
	
	rcP2 = this->GetRect();

	this->GetClientSize(&w, &h);
	rcClP2.x = rcClP2.y = 0; 
	rcClP2.width = w;
	rcClP2.height = h;

	dw = rcP2.width - rcClP2.width;
	dh = rcP2.height - rcClP2.height;

	rcGB1.x = 10;
	rcGB1.y = 2;
	rcGB1.width = 370;
	rcGB1.height = 200;

	rcOI.x = rcGB1.x + 3;
	rcOI.y = rcGB1.y + 15;
	rcOI.width = rcGB1.width - 3*2;
	rcOI.height = rcGB1.height - 15 - 3;

	rcGB2.x = rcGB1.GetRight() + 5;
	rcGB2.y = rcGB1.y;
	rcGB2.width = rcGB1.width;
	rcGB2.height = rcGB1.height;

	rcOIM.x = rcGB2.x + 3;
	rcOIM.y = rcGB2.y + 15;
	rcOIM.width = rcGB2.width - 3*2;
	rcOIM.height = rcGB2.height - 15 - 3;

	rcTEST.x = rcGB2.GetRight() + 70;
	rcTEST.y = 70;
	rcTEST.width = 100;
	rcTEST.height = 30;

	bbw = 23;
	bbh = 22;

	rcLeft.x = rcTEST.x - 50;
	rcLeft.y = rcTEST.y - 30;
	rcLeft.width = bbw;
	rcLeft.height = bbh;

	rcRight.x = rcTEST.GetRight() + (rcTEST.x - rcLeft.GetRight());
	rcRight.y = rcLeft.y;
	rcRight.width = bbw;
	rcRight.height = bbh;

	rlIF.x = rcLeft.GetRight() + 2;
	rlIF.y = rcLeft.y;
	rlIF.width = rcRight.x - 2 - rlIF.x;
	rlIF.height = bbh;

	rcGB3.x = rcGB2.GetRight() + 5;
	rcGB3.y = rcGB2.y;
	rcGB3.width = rcGB2.GetRight() + 2*(rcTEST.x+rcTEST.width/2-rcGB2.GetRight())-5-rcGB3.x;
	rcGB3.height = rcGB2.height;

	rcP2.x = 10;
	rcP2.y = 10;
	rcP2.width = rcGB3.GetRight() + 10 + dw;
	rcP2.height = (rcGB2.GetBottom() + 5) + dh;

	this->SetSize(rcP2);

	m_pP2 = new wxPanel( this, wxID_ANY, rcP2.GetPosition(), rcP2.GetSize() );
	m_pP2->SetMinSize(rcP2.GetSize());
	m_pP2->SetBackgroundColour( m_CLSP );

	wxBoxSizer *top_sizer = new wxBoxSizer( wxVERTICAL );

	wxBoxSizer *button_sizer = new wxBoxSizer( wxHORIZONTAL );

	button_sizer->Add(m_pP2, 1, wxALIGN_CENTER, 0 );

	top_sizer->Add(button_sizer, 1, wxALIGN_CENTER );

	this->SetSizer(top_sizer);

	m_pGB1 = new wxStaticBox( m_pP2, wxID_ANY,
		wxT("Параметры Влияющие На Обработку Изображения"), rcGB1.GetPosition(), rcGB1.GetSize() );
	m_pGB1->SetFont(m_LBLFont);

	m_pGB2 = new wxStaticBox( m_pP2, wxID_ANY,
		wxT("OCR и Мультифреймовая Обработка Потока Изображений"), rcGB2.GetPosition(), rcGB2.GetSize() );
	m_pGB2->SetFont(m_LBLFont);

	m_pGB3 = new wxStaticBox( m_pP2, wxID_ANY,
		wxT(""), rcGB3.GetPosition(), rcGB3.GetSize() );
	m_pGB3->SetFont(m_LBLFont);
	
	
	m_pTest = new wxButton( m_pP2, ID_TEST,
		wxT("Test"), rcTEST.GetPosition(), rcTEST.GetSize() );
	m_pTest->SetFont(m_BTNFont);
	
	bmp_na = wxBitmap(wxImage("bitmaps/left_na.bmp"));
	bmp_od = wxBitmap(wxImage("bitmaps/left_od.bmp"));

	m_pLeft = new wxBitmapButton( m_pP2, ID_SP_LEFT,
		bmp_na, rcLeft.GetPosition(), rcLeft.GetSize() );
	m_pLeft->SetBitmapSelected(bmp_od);

	bmp_na = wxBitmap(wxImage("bitmaps/right_na.bmp"));
	bmp_od = wxBitmap(wxImage("bitmaps/right_od.bmp"));

	m_pRight = new wxBitmapButton( m_pP2, ID_SP_RIGHT,
		bmp_na, rcRight.GetPosition(), rcRight.GetSize() );
	m_pRight->SetBitmapSelected(bmp_od);

	m_plblIF = new CTextBox( m_pP2, wxID_ANY, wxT(StrFN[m_cn]));
	m_plblIF->SetFont(m_LBLFont);
	m_plblIF->SetBackgroundColour( m_CL3 );
	m_plblIF->SetSize(rlIF);

	m_pOI = new CDataGrid( m_pP2, ID_OI,
                           rcOI.GetPosition(), rcOI.GetSize() );

    m_pOI->AppendRows(20);

	/*int ir = grid->GetNumberRows();
    m_pOI->DeleteRows(0, ir);
    m_pOI->AppendRows(ir);*/

	////////////////////////////////////////////////////////////////////////

	//m_OI.Create(rcOI, this, ID_OI, WS_CHILD | WS_VISIBLE | BS_OWNERDRAW/*WS_BORDER*/);

	//m_OI.AddProperty (pProp = new CObjectInspector::CProperty("Глобальные Настройки Обработки Изображений", RGB(0, 255, 255)));
	//pProp->Expand ();
	//pProp->AddProperty (new CObjectInspector::CProperty("Using fast version (partially reduced) : ", &g_fast_search));
	//pProp->AddProperty (new CObjectInspector::CProperty("Using MMX and SSE optimization : ", &g_MMX_SSE));
	//m_OI.AddProperty (pProp = new CObjectInspector::CProperty("Первичная Обработка Изображения", RGB(0, 255, 255)));
	//m_OI.AddProperty (pProp = new CObjectInspector::CProperty("Настройки Для Операторов Собеля"));
	//pProp->Expand ();
	//pProp->AddProperty (new CObjectInspector::CProperty("Moderate Threshold : ", &g_mthr, 0.0, 1.0));
	//pProp->AddProperty (new CObjectInspector::CProperty("Moderate VEdges Threshold : ", &g_mvthr, 0.0, 1.0));
	//pProp->AddProperty (new CObjectInspector::CProperty("Moderate NEdges Threshold : ", &g_mnthr, 0.0, 1.0));
	//pProp->AddProperty (new CObjectInspector::CProperty("Moderate HEdges Threshold : ", &g_mhthr, 0.0, 1.0));
	//m_OI.AddProperty (pProp = new CObjectInspector::CProperty("Настройки Для Цветовой Фильтрации"));
	//pProp->Expand ();
	//pProp->AddProperty (new CObjectInspector::CProperty("Segment Width : ", &g_segw, 4, 50));
	//pProp->AddProperty (new CObjectInspector::CProperty("Min Segments Count : ", &g_msegc, 1, 10));
	//pProp->AddProperty (new CObjectInspector::CProperty("Min Sum Color Difference : ", &g_scd, 0, 10000));
	//m_OI.AddProperty (pProp = new CObjectInspector::CProperty("Вторичная Обработка Изображения", RGB(0, 255, 255)));	
	//m_OI.AddProperty (pProp = new CObjectInspector::CProperty("Настройки Для Линейной Фильтрации"));
	//pProp->Expand ();
	//pProp->AddProperty (new CObjectInspector::CProperty("Line Height : ", &g_segh, 1, 50));
	//pProp->AddProperty (new CObjectInspector::CProperty("Max Between Text Distance : ", &g_btd, 0.0, 1.0));
	//pProp->AddProperty (new CObjectInspector::CProperty("Max Text Centre Offset : ", &g_tco, 0.0, 1.0));
	//pProp->AddProperty (new CObjectInspector::CProperty("Max Text Centre Percent Offset : ", &g_tcpo, 0.0, 1.0));
	//m_OI.AddProperty (pProp = new CObjectInspector::CProperty("Настройки Для Точек Цветовых Границ"));
	//pProp->Expand ();
	//pProp->AddProperty (new CObjectInspector::CProperty("Min Points Number : ", &g_mpn, 0, 10000));
	//pProp->AddProperty (new CObjectInspector::CProperty("Min Points Density : ", &g_mpd, 0.0, 1.0));
	//pProp->AddProperty (new CObjectInspector::CProperty("Min VEdges points density : ", &g_mpved, 0.0, 1.0));
	//pProp->AddProperty (new CObjectInspector::CProperty("Min NEdges points density : ", &g_mpned, 0.0, 1.0));
	//m_OI.AddProperty (pProp = new CObjectInspector::CProperty("Настройки Для Цветовой Фильтрации"));
	//pProp->Expand ();
	//pProp->AddProperty (new CObjectInspector::CProperty("Min Sum Multiple Color Difference : ", &g_smcd, 0, 10000));
	//m_OI.AddProperty (pProp = new CObjectInspector::CProperty("Третичная Обработка Изображения", RGB(0, 255, 255)));	
	//m_OI.AddProperty (pProp = new CObjectInspector::CProperty("Настройки Для Линейной Фильтрации"));
	//pProp->Expand ();
	//pProp->AddProperty (new CObjectInspector::CProperty("Min VEdges points density (per half line) : ", &g_mpvd, 0.0, 1.0));
	//pProp->AddProperty (new CObjectInspector::CProperty("Min HEdges points density (per half line) : ", &g_mphd, 0.0, 1.0));
	//pProp->AddProperty (new CObjectInspector::CProperty("Min NEdges points density (per half line) : ", &g_mpnd, 0.0, 1.0));	

	//m_OI.SetBorderStyle (CObjectInspector::bsLowered);
	//m_OI.SetHotTrack (true);
	//m_OI.Set3dFocus (true);
	//m_OI.SetStyle(true);
	//m_OI.SetMinPropertyColWidth (50);
	//m_OI.SetMinValueColWidth (50);
	//m_OI.SetHeaderTitles ("Property", "Value");
	//m_OI.SetBackColor(::GetSysColor(COLOR_BTNFACE));
	//m_OI.SetPropertyHPTColor(RGB(0, 0, 0));
	//m_OI.SetPropertyHPColor(RGB(255, 215, 0));
	//m_OI.SetPropertyCPColor(RGB(127, 255, 0));
	//m_OI.SetPropertyCVPColor(RGB(255, 255, 0));
	//m_OI.SetPropertyColTextColor (RGB(0,0,0));
	//m_OI.SetValueColTextColor (RGB(0,0,0));
	//m_OI.SetGridLineColor (RGB(80,80,80));
	//m_OI.SetFocusLineColor (RGB(100,100,255));

	////////////////////////////////////////////////////////////////////////

	//m_OIM.Create(rcOIM, this, ID_OIM, WS_CHILD | WS_VISIBLE | BS_OWNERDRAW /*WS_BORDER*/);

	//m_OIM.AddProperty (pProp = new CObjectInspector::CProperty("OCR настройки", RGB(0, 255, 255)));	
	//pProp->Expand ();
	//pProp->AddProperty (new CObjectInspector::CProperty("Clear RGBImages after search subtitles : ", &g_CLEAN_RGB_IMAGES));
	//pProp->AddProperty (new CObjectInspector::CProperty("Using hard algorithm for text mining from background : ", &g_hard_sub_mining));
	//pProp->AddProperty (new CObjectInspector::CProperty("Using FRDImages for getting TXT areas : ", &g_use_FRD_images));
	//pProp->AddProperty (new CObjectInspector::CProperty("Validate And Compare Cleared TXT Images : ", &g_ValidateAndCompareTXTImages));
	//pProp->AddProperty (new CObjectInspector::CProperty("Dont Delete Unrecognized Images (First) : ", &g_DontDeleteUnrecognizedImages1));
	//pProp->AddProperty (new CObjectInspector::CProperty("Dont Delete Unrecognized Images (Second) : ", &g_DontDeleteUnrecognizedImages2));
	//m_OIM.AddProperty (pProp = new CObjectInspector::CProperty("Настройки Мультифреймовой Обработки Изображений", RGB(0, 255, 255)));
	//m_OIM.AddProperty (pProp = new CObjectInspector::CProperty("Настройки Для Обнаружения Саба"));
	//pProp->Expand ();
	//pProp->AddProperty (new CObjectInspector::CProperty("Sub Frames Length : ", &g_DL, 1, 100));
	//pProp->AddProperty (new CObjectInspector::CProperty("Sub Square Error : ", &g_sse, 0.0, 1.0));
	//m_OIM.AddProperty (pProp = new CObjectInspector::CProperty("Настройки Для Сравнения Сабов"));
	//pProp->Expand ();
	//pProp->AddProperty (new CObjectInspector::CProperty("VEdges Points line error : ", &g_veple, 0.0, 1.0));
	//m_OIM.AddProperty (pProp = new CObjectInspector::CProperty("Настройки Для Проверки Саба"));
	//pProp->Expand ();
	//pProp->AddProperty (new CObjectInspector::CProperty("Text Procent : ", &g_tp, 0, 1.0));
	//pProp->AddProperty (new CObjectInspector::CProperty("Min Text Length : ", &g_mtpl, 0.0, 1.0));

	//m_OIM.SetBorderStyle (CObjectInspector::bsLowered);
	//m_OIM.SetHotTrack (true);
	//m_OIM.Set3dFocus (true);
	//m_OIM.SetStyle(true);
	//m_OIM.SetMinPropertyColWidth (50);
	//m_OIM.SetMinValueColWidth (50);
	//m_OIM.SetHeaderTitles ("Property", "Value");
	//m_OIM.SetBackColor(::GetSysColor(COLOR_BTNFACE));
	//m_OIM.SetPropertyHPTColor(RGB(0, 0, 0));
	//m_OIM.SetPropertyHPColor(RGB(255, 215, 0));
	//m_OIM.SetPropertyCPColor(RGB(127, 255, 0));
	//m_OIM.SetPropertyCVPColor(RGB(255, 255, 0));
	//m_OIM.SetPropertyColTextColor (RGB(0,0,0));
	//m_OIM.SetValueColTextColor (RGB(0,0,0));
	//m_OIM.SetGridLineColor (RGB(80,80,80));
	//m_OIM.SetFocusLineColor (RGB(100,100,255));
}


void CSettingsPanel::OnBnClickedTest(wxCommandEvent& event)
{
	CVideo *pVideo;
	int i, k, w, h, S=0;
	char str[30];
	clock_t t;
	

	if (m_pMF->m_VIsOpen == false) return;

	//m_pMF->m_Video.SetPos(GetVideoTime(4, 03, 519));

	pVideo = m_pMF->m_pVideo;
	
	SetVideoWindowSettins(m_pMF->m_pVideo, 
                                  m_pMF->m_pVideoBox->m_pVBox->m_pVSL1->m_pos, 
                                  m_pMF->m_pVideoBox->m_pVBox->m_pVSL2->m_pos, 
                                  m_pMF->m_pVideoBox->m_pVBox->m_pHSL1->m_pos, 
                                  m_pMF->m_pVideoBox->m_pVBox->m_pHSL2->m_pos);

	m_w = w = g_w;
	m_h = h = g_h;

	InitIPData((int)m_pMF->m_pVideo->m_Width, (int)m_pMF->m_pVideo->m_Height, 1);

	if (g_fast_search == true)
	{
		memset(g_ImF[0], 0, (g_W*g_H)*sizeof(int));
		memset(g_ImF[1], 0, (g_W*g_H)*sizeof(int));
		memset(g_ImF[2], 0, (g_W*g_H)*sizeof(int));
		memset(g_ImF[3], 0, (g_W*g_H)*sizeof(int));
		memset(g_ImF[4], 0, (g_W*g_H)*sizeof(int));
		memset(g_ImF[5], 0, (g_W*g_H)*sizeof(int));

		t = clock();
		m_pMF->m_pVideo->GetRGBImage(g_ImRGB, g_xmin, g_xmax, g_ymin, g_ymax);
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

	m_pMF->m_pImageBox->ViewImage(g_ImF[m_cn], g_W, g_H);
	
	SaveRGBImage(g_ImRGB, "/TSTImages/RGBImage.jpeg", g_W, g_H);
	
	for (i=0; i<m_n; i++) 
	{		
		itoa(i, str, 10);
		SaveImage(g_ImF[i], string("/TSTImages/") + string(str) + string("TSTImage _ ") + string(StrFN[i]) + string(".jpeg"), g_W, g_H);
	}
}

void CSettingsPanel::OnBnClickedLeft(wxCommandEvent& event)
{
	m_cn--;
	if (m_cn < 0) m_cn = m_n-1;
	
	m_plblIF->SetLabel(StrFN[m_cn]);

	if (m_pMF->m_VIsOpen == true)
	{
		m_pMF->m_pImageBox->ViewImage(g_ImF[m_cn], g_W, g_H);
	}
}

void CSettingsPanel::OnBnClickedRight(wxCommandEvent& event)
{
	m_cn++;
	if (m_cn > m_n-1) m_cn = 0;

	m_plblIF->SetLabel(StrFN[m_cn]);

	if (m_pMF->m_VIsOpen == true)
	{
		m_pMF->m_pImageBox->ViewImage(g_ImF[m_cn], g_W, g_H);
	}
}

//BOOL CSettingsPanel::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message)
//{
//	if (pWnd == &m_Left)
//	{
//		::SetCursor(m_hCursor);	
//		return TRUE;
//	}
//	else if(pWnd == &m_Right)
//	{
//		::SetCursor(m_hCursor);	
//		return TRUE;
//	}
//	else
//	{
//		return CWnd::OnSetCursor(pWnd, nHitTest, message);
//	}
//}
//
//int CSettingsPanel::OnCreate(LPCREATESTRUCT lpCreateStruct)
//{
//	if (CWnd::OnCreate(lpCreateStruct) == -1)
//		return -1;
//
//	return 0;
//}
//
//void CSettingsPanel::OnPaint()
//{
//	CPaintDC dc(this); // device context for painting
//	// TODO: Add your message handler code here
//	// Do not call CWnd::OnPaint() for painting messages
//
//	m_OI.Invalidate(0);
//	m_OIM.Invalidate(0);
//}
