                              //SettingsPanel.cpp//                                
//////////////////////////////////////////////////////////////////////////////////
//                              Version 1.76                                      //
//                                                                                //
// Author:  Simeon Kosnitsky                                                    //
//          skosnits@gmail.com                                                    //
//                                                                                //
// License:                                                                        //
//     This software is released into the public domain.  You are free to use    //
//     it in any way you like, except that you may not sell this source code.    //
//                                                                                //
//     This software is provided "as is" with no expressed or implied warranty.    //
//     I accept no liability for any damage or loss of business that this        //
//     software may cause.                                                        //
//                                                                                //
//////////////////////////////////////////////////////////////////////////////////

#include "MyResource.h"
#include "SettingsPanel.h"

std::string    StrFN[6];

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

    StrFN[0] = std::string("VEdges Points Image");
    StrFN[1] = std::string("NEdges Points Image");
    StrFN[2] = std::string("HEdges Points Image");
    StrFN[3] = std::string("After First Filtration");
    StrFN[4] = std::string("After Second Filtration");
    StrFN[5] = std::string("After Third Filtration");
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
    m_CL4 = wxColour(255, 255, 0);
    m_CLGG = wxColour(0, 255, 255);

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
        wxT("��������� �������� �� ��������� �����������"), rcGB1.GetPosition(), rcGB1.GetSize() );
    m_pGB1->SetFont(m_LBLFont);

    m_pGB2 = new wxStaticBox( m_pP2, wxID_ANY,
        wxT("OCR � ��������������� ��������� ������ �����������"), rcGB2.GetPosition(), rcGB2.GetSize() );
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

    m_pOI->AddGroup("���������� ��������� ��������� �����������", m_CLGG, m_LBLFont);
    m_pOI->AddProperty("Using fast version (partially reduced) : ", m_CL2, m_CL4, m_LBLFont, &g_fast_search);
    m_pOI->AddProperty("Using MMX and SSE optimization : ", m_CL2, m_CL4, m_LBLFont, &g_MMX_SSE);
    
    m_pOI->AddGroup("��������� ��������� �����������", m_CLGG, m_LBLFont);
    m_pOI->AddSubGroup("��������� ��� ���������� ������", m_CL1, m_LBLFont);
    m_pOI->AddProperty("Moderate Threshold : ", m_CL2, m_CL4, m_LBLFont, m_LBLFont, &g_ModerateThreshold, 0.0, 1.0);
    m_pOI->AddProperty("Moderate VEdges Threshold : ", m_CL2, m_CL4, m_LBLFont, m_LBLFont, &g_ModerateVerticalEdgesThreshold, 0.0, 1.0);
    m_pOI->AddProperty("Moderate NEdges Threshold : ", m_CL2, m_CL4, m_LBLFont, m_LBLFont, &g_ModerateNEdgesThreshold, 0.0, 1.0);
    m_pOI->AddProperty("Moderate HEdges Threshold : ", m_CL2, m_CL4, m_LBLFont, m_LBLFont, &g_ModerateHorizontalEdgesThreshold, 0.0, 1.0);
    m_pOI->AddSubGroup("��������� ��� �������� ����������", m_CL1, m_LBLFont);    
    m_pOI->AddProperty("Segment Width : ", m_CL2, m_CL4, m_LBLFont, m_LBLFont, &g_SegmentWidth, 4, 50);
    m_pOI->AddProperty("Min Segments Count : ", m_CL2, m_CL4, m_LBLFont, m_LBLFont, &g_MinSegmentsCount, 1, 10);
    m_pOI->AddProperty("Min Sum Color Difference : ", m_CL2, m_CL4, m_LBLFont, m_LBLFont, &g_MinSumColorDifference, 0, 10000);

    m_pOI->AddGroup("��������� ��������� �����������", m_CLGG, m_LBLFont);
    m_pOI->AddSubGroup("��������� ��� �������� ����������", m_CL1, m_LBLFont);
    m_pOI->AddProperty("Line Height : ", m_CL2, m_CL4, m_LBLFont, m_LBLFont, &g_SegmentHeight, 1, 50);
    m_pOI->AddProperty("Max Between Text Distance : ", m_CL2, m_CL4, m_LBLFont, m_LBLFont, &g_DistanceBetweenText, 0.0, 1.0);
    m_pOI->AddProperty("Max Text Centre Offset : ", m_CL2, m_CL4, m_LBLFont, m_LBLFont, &g_CenterTextOffset, 0.0, 1.0);
    m_pOI->AddProperty("Max Text Centre Percent Offset : ", m_CL2, m_CL4, m_LBLFont, m_LBLFont, &g_CenterTextOffsetPercent, 0.0, 1.0);
    m_pOI->AddSubGroup("��������� ��� ����� �������� ������", m_CL1, m_LBLFont);
    m_pOI->AddProperty("Min Points Number : ", m_CL2, m_CL4, m_LBLFont, m_LBLFont, &g_MinPointsNumber, 0, 10000);
    m_pOI->AddProperty("Min Points Density : ", m_CL2, m_CL4, m_LBLFont, m_LBLFont, &g_MinPointsDensity, 0.0, 1.0);
    m_pOI->AddProperty("Min VEdges points density : ", m_CL2, m_CL4, m_LBLFont, m_LBLFont, &g_MinVerticalEdgesPointsDensity, 0.0, 1.0);
    m_pOI->AddProperty("Min NEdges points density : ", m_CL2, m_CL4, m_LBLFont, m_LBLFont, &g_MinNEdgesPointsDensity, 0.0, 1.0);
    m_pOI->AddSubGroup("��������� ��� �������� ����������", m_CL1, m_LBLFont);
    m_pOI->AddProperty("Min Sum Multiple Color Difference : ", m_CL2, m_CL4, m_LBLFont, m_LBLFont, &g_MinSumMultipleColorDifference, 0, 10000);

    m_pOI->AddGroup("��������� ��������� �����������", m_CLGG, m_LBLFont);
    m_pOI->AddSubGroup("��������� ��� �������� ����������", m_CL1, m_LBLFont);
    m_pOI->AddProperty("Min VEdges points density (per half line) : ", m_CL2, m_CL4, m_LBLFont, m_LBLFont, &g_MinVerticalEdgesPointsDensityPerLine, 0.0, 1.0);
    m_pOI->AddProperty("Min HEdges points density (per half line) : ", m_CL2, m_CL4, m_LBLFont, m_LBLFont, &g_MinHorizontalEdgesPointsDensityPerLine, 0.0, 1.0);
    m_pOI->AddProperty("Min NEdges points density (per half line) : ", m_CL2, m_CL4, m_LBLFont, m_LBLFont, &g_MinNEdgesPointsDensityPerLine, 0.0, 1.0);

    ////////////////////////////////////////////////////////////////////////

    m_pOIM = new CDataGrid( m_pP2, ID_OIM,
                           rcOIM.GetPosition(), rcOIM.GetSize() );

    m_pOIM->AddGroup("OCR ���������", m_CLGG, m_LBLFont);
    m_pOIM->AddProperty("Clear RGBImages after search subtitles : ", m_CL2, m_CL4, m_LBLFont, &g_CLEAN_RGB_IMAGES);
    m_pOIM->AddProperty("Using hard algorithm for text mining : ", m_CL2, m_CL4, m_LBLFont, &g_hard_sub_mining);
    m_pOIM->AddProperty("Using FRDImages for getting TXT areas : ", m_CL2, m_CL4, m_LBLFont, &g_use_FRD_images);
    m_pOIM->AddProperty("Validate And Compare Cleared TXT Images : ", m_CL2, m_CL4, m_LBLFont, &g_ValidateAndCompareTXTImages);
    m_pOIM->AddProperty("Dont Delete Unrecognized Images (First) : ", m_CL2, m_CL4, m_LBLFont, &g_DontDeleteUnrecognizedImages1);
    m_pOIM->AddProperty("Dont Delete Unrecognized Images (Second) : ", m_CL2, m_CL4, m_LBLFont, &g_DontDeleteUnrecognizedImages2);
    m_pOIM->AddProperty("Default std::string for empty sub : ", m_CL2, m_CL4, m_LBLFont, m_LBLFont, &g_DefStringForEmptySub);
    
    m_pOIM->AddGroup("��������� ��������������� ��������� �����������", m_CLGG, m_LBLFont);
    m_pOIM->AddSubGroup("��������� ��� ����������� ����", m_CL1, m_LBLFont);
    m_pOIM->AddProperty("Sub Frames Length : ", m_CL2, m_CL4, m_LBLFont, m_LBLFont, &g_SubFrameLength, 1, 100);
    m_pOIM->AddProperty("Sub Square Error : ", m_CL2, m_CL4, m_LBLFont, m_LBLFont, &g_SubSquareError, 0.0, 1.0);
    m_pOIM->AddSubGroup("��������� ��� ��������� �����", m_CL1, m_LBLFont);
    m_pOIM->AddProperty("VEdges Points line error : ", m_CL2, m_CL4, m_LBLFont, m_LBLFont, &g_VerticalEdgesPointsLineError, 0.0, 1.0);
    m_pOIM->AddSubGroup("��������� ��� �������� ����", m_CL1, m_LBLFont);
    m_pOIM->AddProperty("Text Procent : ", m_CL2, m_CL4, m_LBLFont, m_LBLFont, &g_TextPercent, 0.0, 1.0);
    m_pOIM->AddProperty("Min Text Length : ", m_CL2, m_CL4, m_LBLFont, m_LBLFont, &g_MinTextLengthPercent, 0.0, 1.0);
}


void CSettingsPanel::OnBnClickedTest(wxCommandEvent& event)
{
    CVideo *pVideo;
    int i, k, w, h, S=0;
    char str[30];
    clock_t t;
    

    if (!m_pMF->m_VIsOpen) return;

    pVideo = m_pMF->m_pVideo;
    
    SetVideoWindowSettins(m_pMF->m_pVideo, 
                                  m_pMF->m_pVideoBox->m_pVBox->m_pVSL1->m_pos, 
                                  m_pMF->m_pVideoBox->m_pVBox->m_pVSL2->m_pos, 
                                  m_pMF->m_pVideoBox->m_pVBox->m_pHSL1->m_pos, 
                                  m_pMF->m_pVideoBox->m_pVBox->m_pHSL2->m_pos);

    m_w = w = g_width;
    m_h = h = g_height;

    InitIPData((int)m_pMF->m_pVideo->m_Width, (int)m_pMF->m_pVideo->m_Height, 1);

    if (g_fast_search)
    {
        memset(g_ImF[0], 0, (g_Width*g_Height)*sizeof(int));
        memset(g_ImF[1], 0, (g_Width*g_Height)*sizeof(int));
        memset(g_ImF[2], 0, (g_Width*g_Height)*sizeof(int));
        memset(g_ImF[3], 0, (g_Width*g_Height)*sizeof(int));
        memset(g_ImF[4], 0, (g_Width*g_Height)*sizeof(int));
        memset(g_ImF[5], 0, (g_Width*g_Height)*sizeof(int));

        t = clock();
        m_pMF->m_pVideo->GetRGBImage(g_RGBImage, g_xmin, g_xmax, g_ymin, g_ymax);
        S = ConvertImage(g_RGBImage, g_ImF[5], g_ImF[0], w, h);
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
            if (g_blnVNE) 
            {
                UnpackImage(g_ImRES1, g_ImF[0], g_pLB, g_pLE, g_LN, w, h);
                UnpackImage(g_ImRES2, g_ImF[1], g_pLB, g_pLE, g_LN, w, h);
            }
            if (g_blnVNE) 
            {
                UnpackImage(g_ImRES3, g_ImF[2], g_pLB, g_pLE, g_LN, w, h);            
            }
        }
    }
    else
    {
        t = clock();
        S = GetAndConvertImage(g_RGBImage, g_ImF[3], g_ImF[4], g_ImF[5], g_ImF[0], g_ImF[1], g_ImF[2], pVideo, w, h);
        t = clock()-t;

        if (S == 0)
        {
            for(i=0; i<m_n; i++) 
            {
                if (g_ImF[i][0] == -1)
                {
                    memset(g_ImF[i], 0, g_Width*g_Height*sizeof(int));
                }
            }
        }
    }
    
    itoa((int)t, str, 10);
    
    if (S > 0)
    {
        if ((w != g_Width) || (h != g_Height))
        {
            ImageToNativeSize(g_RGBImage, w, h);
            
            for(k=0; k<m_n; k++)
            {
                ImageToNativeSize(g_ImF[k], w, h);
            }
        }
    }

    m_pMF->m_pImageBox->ViewImage(g_ImF[m_cn], g_Width, g_Height);
    
    SaveRGBImage(g_RGBImage, "/TSTImages/RGBImage.jpeg", g_Width, g_Height);
    
    for (i=0; i<m_n; i++) 
    {        
        itoa(i, str, 10);
        SaveImage(g_ImF[i], std::string("/TSTImages/") + std::string(str) + std::string("TSTImage _ ") + std::string(StrFN[i]) + std::string(".jpeg"), g_Width, g_Height);
    }
}

void CSettingsPanel::OnBnClickedLeft(wxCommandEvent& event)
{
    m_cn--;
    if (m_cn < 0) m_cn = m_n-1;
    
    m_plblIF->SetLabel(StrFN[m_cn]);

    if (m_pMF->m_VIsOpen == true)
    {
        m_pMF->m_pImageBox->ViewImage(g_ImF[m_cn], g_Width, g_Height);
    }
}

void CSettingsPanel::OnBnClickedRight(wxCommandEvent& event)
{
    m_cn++;
    if (m_cn > m_n-1) m_cn = 0;

    m_plblIF->SetLabel(StrFN[m_cn]);

    if (m_pMF->m_VIsOpen == true)
    {
        m_pMF->m_pImageBox->ViewImage(g_ImF[m_cn], g_Width, g_Height);
    }
}

//BOOL CSettingsPanel::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message)
//{
//    if (pWnd == &m_Left)
//    {
//        ::SetCursor(m_hCursor);    
//        return TRUE;
//    }
//    else if(pWnd == &m_Right)
//    {
//        ::SetCursor(m_hCursor);    
//        return TRUE;
//    }
//    else
//    {
//        return CWnd::OnSetCursor(pWnd, nHitTest, message);
//    }
//}
//
//int CSettingsPanel::OnCreate(LPCREATESTRUCT lpCreateStruct)
//{
//    if (CWnd::OnCreate(lpCreateStruct) == -1)
//        return -1;
//
//    return 0;
//}
//
//void CSettingsPanel::OnPaint()
//{
//    CPaintDC dc(this); // device context for painting
//    // TODO: Add your message handler code here
//    // Do not call CWnd::OnPaint() for painting messages
//
//    m_OI.Invalidate(0);
//    m_OIM.Invalidate(0);
//}
