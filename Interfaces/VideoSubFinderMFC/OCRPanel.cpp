                              //OCRPanel.cpp//                                
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
#include "myResource.h"
#include "VideoSubFinder.h"
#include "OCRPanel.h"

bool g_use_FRD_images = false;

int g_IsCreateClearedTextImages = 0;
int g_RunCreateClearedTextImages = 0;
bool g_ValidateAndCompareTXTImages = false;
bool g_DontDeleteUnrecognizedImages1 = false;
bool g_DontDeleteUnrecognizedImages2 = true;

COCRPanel::COCRPanel()
{
}

COCRPanel::~COCRPanel()
{
}

BEGIN_MESSAGE_MAP(COCRPanel, CWnd)
	ON_WM_CTLCOLOR()
	ON_BN_CLICKED(ID_BTN_CES, OnBnClickedCreateEmptySub)
	ON_BN_CLICKED(ID_BTN_CSCTI, OnBnClickedCreateSubFromClearedTXTImages)
	ON_BN_CLICKED(ID_BTN_CSTXT, OnBnClickedCreateSubFromTXTResults)
	ON_BN_CLICKED(ID_BTN_CCTI, OnBnClickedCreateClearedTextImages)
	ON_BN_CLICKED(ID_BTN_TEST, OnBnClickedTest)
END_MESSAGE_MAP()

void COCRPanel::Init(CSSOWnd* pParent)
{
	CString strOCRClass;

	m_pParent = pParent;
	m_pMainFrm = pParent->m_pParent;

	m_CL1Brush.CreateSolidBrush(RGB(255, 215, 0));
	m_OCRBrush.CreateSolidBrush(RGB(170, 170, 170));

	strOCRClass = AfxRegisterWndClass(CS_VREDRAW | CS_HREDRAW,
									LoadCursor(NULL, IDC_ARROW),
									m_OCRBrush,//(HBRUSH) (COLOR_WINDOW),
									NULL);

	Create( strOCRClass, 
			"", 
			WS_CHILD | WS_VISIBLE | WS_BORDER,
			CRect(10,10,400,300),
			pParent,
			ID_PNL_SEARCH );

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
		"Times New Roman");			   // lpszFacename

	CRect rcCCTI, rcCES, rcP3, rcClP3, rlMSD, reMSD, rcTEST, rcCSCTI, rcCSTXT;
	int w, w2, h, dw, dh;

	w2 = 700;
	w = 400;
	h = 25;

	rcCCTI.left = w2/2 - w/2;
	rcCCTI.right = rcCCTI.left + w;
	rcCCTI.top = 20;
	rcCCTI.bottom = rcCCTI.top + h;

	rcCSTXT.left = rcCCTI.left;
	rcCSTXT.right = rcCCTI.right;
	rcCSTXT.top = rcCCTI.bottom + 10;
	rcCSTXT.bottom = rcCSTXT.top + h;

	rcCSCTI.left = rcCCTI.left;
	rcCSCTI.right = rcCCTI.right;
	rcCSCTI.top = rcCSTXT.bottom + 10;
	rcCSCTI.bottom = rcCSCTI.top + h;

	rcCES.left = rcCCTI.left;
	rcCES.right = rcCCTI.right;
	rcCES.top = rcCSCTI.bottom + 10;
	rcCES.bottom = rcCES.top + h;

	rcTEST.left = rcCCTI.right + 30;
	rcTEST.right = rcTEST.left + 100;
	rcTEST.top = rcCCTI.bottom + 5 - h/2;
	rcTEST.bottom = rcTEST.top + h;

	rlMSD.left = 20; 
	rlMSD.right = rcCCTI.left - 20;
	rlMSD.top = 20;
	rlMSD.bottom = rlMSD.top + 18;

	reMSD.left = rlMSD.left; 
	reMSD.right = rlMSD.right;
	reMSD.top = rlMSD.bottom + 10;
	reMSD.bottom = reMSD.top + 18;

	this->GetWindowRect(&rcP3);
	this->GetClientRect(&rcClP3);
	dw = rcP3.Width() - rcClP3.Width();
	dh = rcP3.Height() - rcClP3.Height();
	
	rcP3.left = 10;
	rcP3.right = rcP3.left + w2 + dw;
	rcP3.top = 10;
	rcP3.bottom = rcP3.top + rcCES.bottom + 20 + dh;

	this->MoveWindow(&rcP3);

	m_lblMSD.Create("Min Sub Duration:", WS_CHILD | WS_VISIBLE | SS_CENTER | SS_CENTERIMAGE | WS_BORDER,
					rlMSD, this, ID_LBL_MSD);
	m_lblMSD.SetFont(&m_LBLFont);

	m_MSD.CreateEx(WS_EX_CLIENTEDGE, _T("EDIT"), "", WS_CHILD | WS_VISIBLE | ES_AUTOHSCROLL, reMSD, this, ID_MSD);
	m_MSD.SetFont(&m_LBLFont);
	m_MSD.SetWindowText("0.000");

	m_CES.Create("Create Empty Sub", WS_CHILD | WS_VISIBLE, rcCES, this, ID_BTN_CES);
	m_CES.SetFont(&m_BTNFont);

	m_CCTI.Create("Create Cleared TXT Images", WS_CHILD | WS_VISIBLE, rcCCTI, this, ID_BTN_CCTI);
	m_CCTI.SetFont(&m_BTNFont);

	m_CSTXT.Create("Create Sub From TXT Results", WS_CHILD | WS_VISIBLE, rcCSTXT, this, ID_BTN_CSTXT);
	m_CSTXT.SetFont(&m_BTNFont);

	m_CSCTI.Create("Create Empty Sub From Cleared TXT Images", WS_CHILD | WS_VISIBLE, rcCSCTI, this, ID_BTN_CSCTI);
	m_CSCTI.SetFont(&m_BTNFont);

	m_TEST.Create("Test", WS_CHILD | WS_VISIBLE, rcTEST, this, ID_BTN_TEST);
	m_TEST.SetFont(&m_BTNFont);
}

HBRUSH COCRPanel::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	HBRUSH hbr = CWnd::OnCtlColor(pDC, pWnd, nCtlColor);

	switch (pWnd->GetDlgCtrlID())
	{
		case ID_LBL_MSD:
			pDC->SetBkMode(TRANSPARENT);
			hbr = m_CL1Brush;
			break;
	}

	return hbr;
}

void COCRPanel::OnBnClickedCreateEmptySub()
{
	CString Str, hour1, hour2, min1, min2, sec1, sec2, msec1, msec2;
	int i, j, k, sec, msec;
	fstream fout;
	u64 bt, et, dt, mdt;
	char str[30];

	WIN32_FIND_DATA FindFileData;
	HANDLE hFind;
	BOOL FileFinded;
	vector<CString> FileNamesVector;
	vector<u64> BT, ET;

	hFind = FindFirstFile(m_pMainFrm->m_Dir+"\\RGBImages\\*.jpeg", &FindFileData);
	if (hFind != INVALID_HANDLE_VALUE) FileFinded = true;
	else return;
    
	do
	{
		FileNamesVector.push_back(FindFileData.cFileName);
		FileFinded = FindNextFile(hFind,&FindFileData);
	} 
	while (FileFinded);

	for (i=0; i<(int)FileNamesVector.size()-1; i++)
	for (j=i+1; j<(int)FileNamesVector.size(); j++)
	{
		if (FileNamesVector[i] > FileNamesVector[j])
		{
			Str = FileNamesVector[i];
			FileNamesVector[i] = FileNamesVector[j];
			FileNamesVector[j] = Str;
		}
	}

	fout.open(m_pMainFrm->m_Dir+"\\sub.srt", ios::out);

	m_MSD.GetWindowText(Str);
	mdt = (s64)atof(Str)*1000;

	for(k=0; k<(int)FileNamesVector.size(); k++)
	{
		Str = FileNamesVector[k];

		hour1 = Str.Mid(0,1);
		min1 = Str.Mid(2,2);
		sec1 = Str.Mid(5,2);
		msec1 = Str.Mid(8,3);

		hour2 = Str.Mid(13,1);
		min2 = Str.Mid(15,2);
		sec2 = Str.Mid(18,2);
		msec2 = Str.Mid(21,3);

		bt = (atoi(hour1)*3600 + atoi(min1)*60 + atoi(sec1))*1000 + atoi(msec1);
		et = (atoi(hour2)*3600 + atoi(min2)*60 + atoi(sec2))*1000 + atoi(msec2);

		BT.push_back(bt);
		ET.push_back(et);
	}

	for(k=0; k<(int)FileNamesVector.size()-1; k++)
	{
		if (ET[k]-BT[k] < mdt)
		{
			if (BT[k]+mdt < BT[k+1]) 
			{
				ET[k] = BT[k]+mdt;
			}
			else
			{
				ET[k] = BT[k+1]-1;
			}
		}
	}

	for(k=0; k<(int)FileNamesVector.size(); k++)
	{
		bt = BT[k];
		et = ET[k];

		Str = VideoTimeToStr2(bt*(u64)10000)+
			  " --> "+
			  VideoTimeToStr2(et*(u64)10000);

		dt = et - bt;
		sec = (int)(dt/1000);
		msec = (int)(dt%1000);
		
		itoa(sec, str, 10);
		sec1 = str;

		itoa(msec, str, 10);
		if (msec < 10) msec1 = CString("00")+str; 
		else
		{
			if (msec < 100) msec1 = CString("0")+str; 
			else msec1 = str; 
		}

		fout << (k+1) << "\n" << Str << "\n" << "sub duration: " + sec1 + "," + msec1 << "\n\n";
	}

	fout.close();
}

void COCRPanel::OnBnClickedCreateSubFromTXTResults()
{
	CreateSubFromTXTResults();
}

void COCRPanel::OnBnClickedCreateSubFromClearedTXTImages()
{
	CString Str, Name, hour1, hour2, min1, min2, sec1, sec2, msec1, msec2;
	int i, j, k, kb, sec, msec;
	fstream fout;
	char str[30];
	u64 bt, et, dt, mdt;

	WIN32_FIND_DATA FindFileData;
	HANDLE hFind;
	BOOL FileFinded;
	vector<CString> FileNamesVector;
	vector<u64> BT, ET;

	hFind = FindFirstFile(m_pMainFrm->m_Dir+"\\TXTImages\\*.jpeg", &FindFileData);
	if (hFind != INVALID_HANDLE_VALUE) FileFinded = true;
	else return;
    
	do
	{
		FileNamesVector.push_back(FindFileData.cFileName);
		FileFinded = FindNextFile(hFind,&FindFileData);
	} 
	while (FileFinded);

	for (i=0; i<(int)FileNamesVector.size()-1; i++)
	for (j=i+1; j<(int)FileNamesVector.size(); j++)
	{
		if (FileNamesVector[i] > FileNamesVector[j])
		{
			Str = FileNamesVector[i];
			FileNamesVector[i] = FileNamesVector[j];
			FileNamesVector[j] = Str;
		}
	}

	m_MSD.GetWindowText(Str);
	mdt = (s64)atof(Str)*1000;

	k = 0;
	while (k < (int)FileNamesVector.size())
	{
		kb = k;
		i = 0;
		while( (k < (int)FileNamesVector.size()) &&
			   (FileNamesVector[kb].Mid(0, 11) == FileNamesVector[k].Mid(0, 11))
			 )
		{
			k++;
		}

		Str = FileNamesVector[kb];

		hour1 = Str.Mid(0,1);
		min1 = Str.Mid(2,2);
		sec1 = Str.Mid(5,2);
		msec1 = Str.Mid(8,3);

		hour2 = Str.Mid(13,1);
		min2 = Str.Mid(15,2);
		sec2 = Str.Mid(18,2);
		msec2 = Str.Mid(21,3);

		bt = (atoi(hour1)*3600 + atoi(min1)*60 + atoi(sec1))*1000 + atoi(msec1);
		et = (atoi(hour2)*3600 + atoi(min2)*60 + atoi(sec2))*1000 + atoi(msec2);

		BT.push_back(bt);
		ET.push_back(et);
	}

	for(k=0; k<(int)BT.size()-1; k++)
	{
		if (ET[k]-BT[k] < mdt)
		{
			if (BT[k]+mdt < BT[k+1]) 
			{
				ET[k] = BT[k]+mdt;
			}
			else
			{
				ET[k] = BT[k+1]-1;
			}
		}
	}

	fout.open(m_pMainFrm->m_Dir+"\\sub.srt", ios::out);

	for(k=0; k<(int)BT.size(); k++)
	{
		bt = BT[k];
		et = ET[k];

		Str = VideoTimeToStr2(bt*(u64)10000)+
			  " --> "+
			  VideoTimeToStr2(et*(u64)10000);

		dt = et - bt;
		sec = (int)(dt/1000);
		msec = (int)(dt%1000);
		
		itoa(sec, str, 10);
		sec1 = str;

		itoa(msec, str, 10);
		if (msec < 10) msec1 = CString("00")+str; 
		else
		{
			if (msec < 100) msec1 = CString("0")+str; 
			else msec1 = str; 
		}

		fout << (k+1) << "\n" << Str << "\n" << "sub duration: " + sec1 + "," + msec1 << "\n\n";
	}

	fout.close();
}

void COCRPanel::CreateSubFromTXTResults()
{
	CString Str, Name, hour1, hour2, min1, min2, sec1, sec2, msec1, msec2;
	int i, j, k, kb, sec, msec;
	fstream fout;
	//fstream fin;
	u64 bt, et, dt, mdt;
	char str[1000];

	WIN32_FIND_DATA FindFileData;
	HANDLE hFind;
	BOOL FileFinded;
	vector<CString> FileNamesVector;
	vector<CString> TXTVector;
	vector<u64> BT, ET;

	hFind = FindFirstFile(m_pMainFrm->m_Dir+"\\TXTResults\\*.txt", &FindFileData);
	if (hFind != INVALID_HANDLE_VALUE) FileFinded = true;
	else return;
    
	do
	{
		FileNamesVector.push_back(FindFileData.cFileName);
		FileFinded = FindNextFile(hFind,&FindFileData);
	} 
	while (FileFinded);

	for (i=0; i<(int)FileNamesVector.size()-1; i++)
	for (j=i+1; j<(int)FileNamesVector.size(); j++)
	{
		if (FileNamesVector[i] > FileNamesVector[j])
		{
			Str = FileNamesVector[i];
			FileNamesVector[i] = FileNamesVector[j];
			FileNamesVector[j] = Str;
		}
	}

	m_MSD.GetWindowText(Str);
	mdt = (s64)atof(Str)*1000;

	str[0] = '\0';

	k = 0;
	while (k < (int)FileNamesVector.size())
	{
		kb = k;
		Str = "";
		i = 0;
		while( (k < (int)FileNamesVector.size()) &&
			   (FileNamesVector[kb].Mid(0, 11) == FileNamesVector[k].Mid(0, 11))
			 )
		{
			Name = m_pMainFrm->m_Dir+"\\TXTResults\\"+FileNamesVector[k];

			FILE *fin = fopen(Name, "r");
			
			while (!feof(fin))
			{
				str[0] = '\0';
				fgets(str, 1000, fin);
				if (str[0] != '\0')
				{
					if (i > 0) Str += "\n";
					Str += str;
					i++;
				}
			}

			fclose(fin);

			k++;
		}
		TXTVector.push_back(Str);

		Str = FileNamesVector[kb];

		hour1 = Str.Mid(0,1);
		min1 = Str.Mid(2,2);
		sec1 = Str.Mid(5,2);
		msec1 = Str.Mid(8,3);

		hour2 = Str.Mid(13,1);
		min2 = Str.Mid(15,2);
		sec2 = Str.Mid(18,2);
		msec2 = Str.Mid(21,3);

		bt = (atoi(hour1)*3600 + atoi(min1)*60 + atoi(sec1))*1000 + atoi(msec1);
		et = (atoi(hour2)*3600 + atoi(min2)*60 + atoi(sec2))*1000 + atoi(msec2);

		BT.push_back(bt);
		ET.push_back(et);
	}

	k=0;
	while(k < (int)TXTVector.size()-1)
	{
		if (TXTVector[k] == "")
		{
			if (g_DontDeleteUnrecognizedImages2 == false)
			{
				for(i=k; i<(int)TXTVector.size()-1; i++)
				{
					BT[i] = BT[i+1];
					ET[i] = ET[i+1];
					TXTVector[i] = TXTVector[i+1];
				}
				BT.pop_back();
				ET.pop_back();
				TXTVector.pop_back();

				continue;
			}
			else
			{
				TXTVector[k] = CString("#unrecognized text#");
			}
		}

		if (BT[k+1]-ET[k] <= 333)
		{
			if (TXTVector[k+1] == TXTVector[k])
			{
				ET[k] = ET[k+1];

				for(i=k+1; i<(int)TXTVector.size()-1; i++)
				{
					BT[i] = BT[i+1];
					ET[i] = ET[i+1];
					TXTVector[i] = TXTVector[i+1];
				}
				BT.pop_back();
				ET.pop_back();
				TXTVector.pop_back();

				continue;
			}
		}

		k++;
	}

	for(k=0; k<(int)TXTVector.size()-1; k++)
	{
		if (ET[k]-BT[k] < mdt)
		{
			if (BT[k]+mdt < BT[k+1]) 
			{
				ET[k] = BT[k]+mdt;
			}
			else
			{
				ET[k] = BT[k+1]-1;
			}
		}
	}

	fout.open(m_pMainFrm->m_Dir+"\\sub.srt", ios::out);

	for(k=0; k<(int)TXTVector.size(); k++)
	{
		bt = BT[k];
		et = ET[k];

		Str = VideoTimeToStr2(bt*(u64)10000)+
			  " --> "+
			  VideoTimeToStr2(et*(u64)10000);

		dt = et - bt;
		sec = (int)(dt/1000);
		msec = (int)(dt%1000);
		
		itoa(sec, str, 10);
		sec1 = str;

		itoa(msec, str, 10);
		if (msec < 10) msec1 = CString("00")+str; 
		else
		{
			if (msec < 100) msec1 = CString("0")+str; 
			else msec1 = str; 
		}

		fout << (k+1) << "\n" << Str << "\n" << TXTVector[k] << "\n\n";
	}

	fout.close();
}

void COCRPanel::OnBnClickedTest()
{
	int w, h, S, i, j;
	vector<string> SavedFiles;
	CString Str;
	s64 CurPos;

	if (m_pMainFrm->m_VIsOpen == false) return;
	
	SetVideoWindowSettins(m_pMainFrm->m_pVideo, 
                          m_pMainFrm->m_pVideoBox->m_VBox.m_VSL1.m_pos, 
                          m_pMainFrm->m_pVideoBox->m_VBox.m_VSL2.m_pos, 
                          m_pMainFrm->m_pVideoBox->m_VBox.m_HSL1.m_pos, 
                          m_pMainFrm->m_pVideoBox->m_VBox.m_HSL2.m_pos);

	w = g_w;
	h = g_h;

	InitIPData((int)m_pMainFrm->m_pVideo->m_Width, (int)m_pMainFrm->m_pVideo->m_Height, 3);
	
	S = GetAndConvertImage(g_ImRGB, g_ImF[3], g_ImF[4], g_ImF[5], g_ImF[0], g_ImF[1], g_ImF[2], m_pMainFrm->m_pVideo, w, h);

	SavedFiles.clear();

	CurPos = m_pMainFrm->m_pVideo->GetPos();

	Str = m_pMainFrm->m_pVideo->m_MovieName.c_str();

	j = Str.GetLength()-1;
	while (Str[j] != '.') j--;
	i = j;
	while (Str[i] != '\\') i--;

	Str = Str.Mid(i+1, j-i-1);

	Str += CString(" -- ") + VideoTimeToStr(CurPos).c_str();

	SavedFiles.push_back(string(Str));

	g_show_results = 1;

	if (g_debug == 0) m_pMainFrm->ClearDir("TestImages");

	FindTextLines(g_ImRGB, g_ImF[5], g_ImF[3], SavedFiles, w, h);
}

void COCRPanel::OnBnClickedCreateClearedTextImages()
{
	if (g_IsCreateClearedTextImages == 0)
	{
		g_IsCreateClearedTextImages = 1;
		g_RunCreateClearedTextImages = 1;

		m_CCTI.SetWindowText("Stop CCTXTImages");

		m_hSearchThread = CreateThread(NULL, 0, ThreadCreateClearedTextImages, (PVOID)m_pMainFrm, 0, &m_dwSearchThreadID);
		//SetThreadPriority(m_hSearchThread, THREAD_PRIORITY_IDLE);
	}
	else
	{
		m_CCTI.SetWindowText("Create Cleared Text Images");
		g_RunCreateClearedTextImages = 0;
	}
}

DWORD WINAPI ThreadCreateClearedTextImages(PVOID pParam)
{	
	g_IsCreateClearedTextImages = 1;

	if (g_debug == 0) g_show_results = 0;
	else g_show_results = 1;

	CMainFrame* pMainFrm = (CMainFrame*)pParam;
	CString Str, dStr;
	char str[30];
	int i, j, k, w, h, val;
	
	int w1, h1, w2, h2, YB1, YB2, bln;
	CString hour1, hour2, min1, min2, sec1, sec2, msec1, msec2;
	u64 bt1, et1, bt2, et2;

	int *ImRES1 = NULL;
	int *ImRES2 = NULL;

	int res;

	WIN32_FIND_DATA FindFileData;
	HANDLE hFind;
	BOOL FileFinded;
	vector<CString> FileNamesVector;
	vector<string> SavedFiles, prevSavedFiles;
	vector<u64> BT, ET;

	hFind = FindFirstFile(pMainFrm->m_Dir+"\\RGBImages\\*.jpeg", &FindFileData);
	if (hFind != INVALID_HANDLE_VALUE) FileFinded = true;
	else
	{
		pMainFrm->m_pPanel->m_OCRPanel.m_CCTI.SetWindowText("Create Cleared TXT Images");

		g_IsCreateClearedTextImages = 0;

		return 0;
	}
    
	do
	{
		FileNamesVector.push_back(FindFileData.cFileName);
		FileFinded = FindNextFile(hFind,&FindFileData);
	} 
	while (FileFinded);

	for (i=0; i<(int)FileNamesVector.size()-1; i++)
	for (j=i+1; j<(int)FileNamesVector.size(); j++)
	{
		if (FileNamesVector[i] > FileNamesVector[j])
		{
			Str = FileNamesVector[i];
			FileNamesVector[i] = FileNamesVector[j];
			FileNamesVector[j] = Str;
		}
	}

	val = (int)FileNamesVector.size();
	sprintf(str, "%.4d", val);
	dStr = CString(" : ") + CString(str);

	for (k=0; k<(int)FileNamesVector.size(); k++)
	{
		if (g_RunCreateClearedTextImages == 0) break;

		Str = pMainFrm->m_Dir+"\\RGBImages\\"+FileNamesVector[k];
		
		LoadRGBImage(g_ImRGB, string(Str), w, h);		
		//pMainFrm->m_pVideoBox->ViewImage(ImRGB, w, h);

		if (k == 0)
		{
			g_W = w;
			g_H = h;

			g_xmin = 0;
			g_xmax = w-1;
			g_ymin = 0;
			g_ymax = h-1;
			
			InitIPData(w, h, 3);

			//ImRES1 = new int[w*h*3];
			//ImRES2 = new int[w*h*3];
		}

		GetTransformedImage(g_ImRGB, g_ImF[3], g_ImF[4], g_ImF[5], g_ImF[0], g_ImF[1], g_ImF[2], w, h);

		if (g_use_FRD_images == true) 
		{
			Str = FileNamesVector[k];
			Str = Str.Mid(0, Str.GetLength()-5);
			Str = pMainFrm->m_Dir+"\\FRDImages\\"+Str+"!.jpeg";
			LoadImage(g_ImF[5], string(Str), w, h);		
			//pMainFrm->m_pImageBox->ViewImage(ImSF, w, h);
		}
		
		Str = FileNamesVector[k];
		Str = Str.Mid(0, Str.GetLength()-5);
		SavedFiles.clear();
		SavedFiles.push_back(string(Str));

		val = k+1;
		sprintf(str, "%.4d", val);
		pMainFrm->m_pVideoBox->m_lblTIME.SetWindowText(CString(str) + dStr);

		res = FindTextLines(g_ImRGB, g_ImF[5], g_ImF[3], SavedFiles, w, h);

		if ( (res == 0) && (g_DontDeleteUnrecognizedImages1 == true) )
		{
			Str = FileNamesVector[k];
			Str = Str.Mid(0, Str.GetLength()-5);

			Str = CString("\\TXTImages\\") + Str;
		
			Str += CString("_") + CString("01");

			Str += CString("_") + CString("0000");
			
			Str += CString("_") + CString("0000");
			
			Str += CString(".jpeg");

			memset(g_ImRES1, 0, ((w*4)*(h/4))*sizeof(int));

			SaveImage(g_ImRES1, string(Str), w*4, h/4);
		}

		if ( (k>1) && (res == 1) && (g_ValidateAndCompareTXTImages == true) && (prevSavedFiles.size() == SavedFiles.size()) )
		{
			Str = prevSavedFiles[i].c_str();
			i = Str.GetLength()-1;
			while (Str[i] != '\\') i--;
			Str = Str.Mid(i+1);

			hour1 = Str.Mid(0,1);
			min1 = Str.Mid(2,2);
			sec1 = Str.Mid(5,2);
			msec1 = Str.Mid(8,3);

			hour2 = Str.Mid(13,1);
			min2 = Str.Mid(15,2);
			sec2 = Str.Mid(18,2);
			msec2 = Str.Mid(21,3);

			bt1 = (atoi(hour1)*3600 + atoi(min1)*60 + atoi(sec1))*1000 + atoi(msec1);
			et1 = (atoi(hour2)*3600 + atoi(min2)*60 + atoi(sec2))*1000 + atoi(msec2);

			Str = SavedFiles[i].c_str();
			i = Str.GetLength()-1;
			while (Str[i] != '\\') i--;
			Str = Str.Mid(i+1);

			hour1 = Str.Mid(0,1);
			min1 = Str.Mid(2,2);
			sec1 = Str.Mid(5,2);
			msec1 = Str.Mid(8,3);

			hour2 = Str.Mid(13,1);
			min2 = Str.Mid(15,2);
			sec2 = Str.Mid(18,2);
			msec2 = Str.Mid(21,3);

			bt2 = (atoi(hour1)*3600 + atoi(min1)*60 + atoi(sec1))*1000 + atoi(msec1);
			et2 = (atoi(hour2)*3600 + atoi(min2)*60 + atoi(sec2))*1000 + atoi(msec2);
			
			if (bt2-et1 < 300)
			{
				bln = 1;
				for (i=0; i<(int)SavedFiles.size(); i++)
				{
					LoadImage(g_ImF[0], prevSavedFiles[i], w1, h1);
					LoadImage(g_ImF[1], SavedFiles[i], w2, h2);

					Str = prevSavedFiles[i].c_str();
					i = Str.GetLength()-1;
					while (Str[i] != '_') i--;
					j = i;
					i--;
					while (Str[i] != '_') i--;
					Str = Str.Mid(i+1, j-i-1);
					YB1 = atoi(Str);

					Str = SavedFiles[i].c_str();
					i = Str.GetLength()-1;
					while (Str[i] != '_') i--;
					j = i;
					i--;
					while (Str[i] != '_') i--;
					Str = Str.Mid(i+1, j-i-1);
					YB2 = atoi(Str);

					bln = CompareTXTImages(g_ImF[0], g_ImF[1], w1, h1, w2, h2, YB1, YB2);
					if (bln == 0) break;
				}

				if (bln == 1)
				{
					for (i=0; i<(int)SavedFiles.size(); i++)
					{
						DeleteFile(prevSavedFiles[i].c_str());
						
						Str = prevSavedFiles[i].c_str();
						i = Str.GetLength()-1;
						while (Str[i] != '\\') i--;
						Str = Str.Mid(0,i+1+11)+CString(SavedFiles[i].c_str()).Mid(i+1+11);

						MoveFile(SavedFiles[i].c_str(), Str);
					}
				}
			}
		}

		prevSavedFiles = SavedFiles;
	}

	//if (ImRES1 != NULL) delete[] ImRES1;
	//if (ImRES2 != NULL) delete[] ImRES2;

	pMainFrm->m_pVideoBox->m_lblTIME.SetWindowText("00:00:00,000/00:00:00,000");

	pMainFrm->m_pPanel->m_OCRPanel.m_CCTI.SetWindowText("Create Cleared TXT Images");

	g_IsCreateClearedTextImages = 0;
	
	return 0;
}
