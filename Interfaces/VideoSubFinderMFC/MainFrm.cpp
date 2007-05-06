                              //MainFrm.cpp//                                
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
#include "VideoSubFinder.h"
#include "MainFrm.h"
#include ".\mainfrm.h"

CMainFrame *g_pMF;

/////////////////////////////////////////////////////////////////////////////

void  ViewImageInImageBox(int *Im, int w, int h)
{
	g_pMF->m_pImageBox->ViewImage(Im, w, h);	
}

/////////////////////////////////////////////////////////////////////////////

void  ViewImageInVideoBox(int *Im, int w, int h)
{
	g_pMF->m_pVideoBox->ViewImage(Im, w, h);	
}

/////////////////////////////////////////////////////////////////////////////

void ViewRGBImage(int *Im, int w, int h)
{
	g_pMF->m_pImageBox->ViewRGBImage(Im, w, h);	
}

/////////////////////////////////////////////////////////////////////////////

BEGIN_MESSAGE_MAP(CMainFrame, CMDIFrameWnd)
	ON_WM_SIZE()
	ON_COMMAND(ID_PLAY_PAUSE, OnPlayPause)
	ON_COMMAND(ID_PLAY_STOP, OnStop)
	ON_COMMAND(ID_FILE_REOPENVIDEO, OnFileReOpenVideo)
	ON_COMMAND(ID_FILE_OPENVIDEONORMALLY, OnFileOpenVideoNormally)
	ON_COMMAND(ID_FILE_OPENVIDEOALLDEFAULT, OnFileOpenVideoAllDefault)
	ON_COMMAND(ID_FILE_OPENVIDEOHARD, OnFileOpenVideoHard)
	ON_COMMAND(ID_EDIT_SETBEGINTIME, OnEditSetBeginTime)
	ON_COMMAND(ID_EDIT_SETENDTIME, OnEditSetEndTime)
	ON_COMMAND(IDM_FILE_SAVESETTINGS, OnFileSaveSettings)
	ON_COMMAND(ID_FILE_LOADSETTINGS, OnFileLoadSettings)
	ON_COMMAND(ID_FILE_SAVESETTINGSAS, OnFileSaveSettingsAs)
	ON_WM_CLOSE()
	ON_COMMAND(ID_FILE_OPENPREVIOUSVIDEO, OnFileOpenPreviousVideo)
	ON_COMMAND(ID_SETPRIORITY_IDLE, OnSetpriorityIdle)
	ON_COMMAND(ID_SETPRIORITY_NORMAL, OnSetpriorityNormal)
	ON_COMMAND(ID_SETPRIORITY_BELOWNORMAL, OnSetpriorityBelownormal)
	ON_COMMAND(ID_SETPRIORITY_ABOVENORMAL, OnSetpriorityAbovenormal)
	ON_COMMAND(ID_SETPRIORITY_HIGH, OnSetpriorityHigh)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////

CMainFrame::CMainFrame()
{
	m_WasInited = false;
	m_VIsOpen = false;
	m_VTimerFuncID = 0;

	m_pVideo = GetDSVideoObject();

	char dir[300];
	GetCurrentDirectory(300,dir);
	m_Dir = dir;

	g_dir = m_Dir;
	m_pVideo->m_Dir = m_Dir;

	g_pMF = this;
	g_pViewImage[0] = ViewImageInVideoBox;
	g_pViewImage[1] = ViewImageInImageBox;
	g_pViewRGBImage = ViewRGBImage;

	m_blnReopenVideo = false;

	m_FileName = "";
	m_dt = 0;

	m_type = 0;
}

CMainFrame::~CMainFrame()
{
}

void CMainFrame::Init()
{
	m_Menu.LoadMenu(IDR_MENU);
	this->SetMenu(&m_Menu);

	this->SetTitle("VideoSubFinder 1.76 beta version");
	this->MoveWindow(0, 0, 1024, 768-30);

	if (IsMMX_and_SSE() == true)
	{
		g_MMX_SSE = true;
	}
	else
	{
		g_MMX_SSE = false;
	}

	m_pPanel = new CSSOWnd();
	m_pPanel->Init(this);

	m_pImageBox = new CImageBox();
	m_pImageBox->Init(this);
	m_pImageBox->MoveWindow(508, 22, 408, 354);

	m_pVideoBox = new CVideoBox();
	m_pVideoBox->Init(this);
	m_pVideoBox->MoveWindow(80, 22, 408, 404);

	ResizeControls();

	m_SettingsFileName = m_Dir+"\\settings.cfg";
	LoadSettings(m_SettingsFileName);

	m_WasInited = true;
}

void CMainFrame::ResizeControls()
{
	CRect rcP, rcClP, rect;
	int dw, dh;

	this->GetClientRect(rect);
	m_pPanel->GetClientRect(rcClP);
	m_pPanel->GetWindowRect(rcP);
	dw = rcP.Width() - rcClP.Width();
	dh = rcP.Height() - rcClP.Height();

	rcP.top = rect.bottom-250-dh;
	rcP.bottom = rect.bottom;
	rcP.left = -1;
	rcP.right = rect.right;

	m_pPanel->MoveWindow(rcP);
}

void CMainFrame::OnSize(UINT nType, int cx, int cy)
{
	CMDIFrameWnd::OnSize(nType, cx, cy);

	if (m_WasInited) ResizeControls();
}

void CMainFrame::OnFileReOpenVideo()
{
	m_blnReopenVideo = true;
	OnFileOpenVideo(m_type);
}

void CMainFrame::OnFileOpenVideoNormally()
{
	OnFileOpenVideo(0);
}

void CMainFrame::OnFileOpenVideoAllDefault()
{
	OnFileOpenVideo(1);
}

void CMainFrame::OnFileOpenVideoHard()
{
	OnFileOpenVideo(2);
}

void CMainFrame::OnFileOpenVideo(int type)
{
	CString csFileName;
	s64 Cur;
	bool was_open_before = false;
	int i;

	if (m_VTimerFuncID == 1) 
	{
		this->KillTimer(m_VTimerFuncID);
		m_VTimerFuncID = 0;
		was_open_before = true;
	}

	m_type = type;
	csFileName = m_FileName;

	OPENFILENAME ofn;       // common dialog box structure
	char szFile[260];       // buffer for file name

	// Initialize OPENFILENAME
	ZeroMemory(&ofn, sizeof(ofn));
	ofn.lStructSize = sizeof(ofn);
	ofn.hwndOwner = this->m_hWnd;
	ofn.lpstrFile = szFile;

	ofn.lpstrFile[0] = '\0';
	strcpy(ofn.lpstrFile, (char*)csFileName.GetString());
	ofn.nMaxFile = sizeof(szFile);
	ofn.lpstrFilter = "Video Files (*.avi;*.mp4;*.mpg;*.mpeg;*.mpv;*.m1v;*.dat;*.avs;*.vdr;*.asf;*.asx;*.wmv;*.mkv;*.ogm)\0*.avi;*.mp4;*.mpg;*.mpeg;*.mpv;*.m1v;*.dat;*.avs;*.vdr;*.asf;*.asx;*.wmv;*.mkv;*.ogm\0All Files (*.*)\0*.*\0";
	ofn.lpstrTitle = "Open Video File";
	ofn.nFilterIndex = 1;
	ofn.lpstrFileTitle = NULL;
	ofn.nMaxFileTitle = 0;
	ofn.lpstrInitialDir = NULL;
	ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;

	if (m_blnReopenVideo == false)
	{
		if (FALSE == GetOpenFileName( &ofn )) 
		{
			if (was_open_before) 
			{
				m_VTimerFuncID = 1;
				m_ct = -1;
				this->SetTimer(m_VTimerFuncID, 100, (TIMERPROC)VTimerFunc);
			}
			return;
		}
		csFileName = ofn.lpstrFile;
	}

	m_FileName = csFileName;

	//m_blnOpenVideoThreadStateFlag = false;
	//m_hOpenVideoThread = CreateThread(NULL, 0, ThreadOpenVideo, (PVOID)this, 0, &m_dwOpenVideoThreadID);
	//SetThreadPriority(m_hOpenVideoThread, THREAD_PRIORITY_HIGHEST);
	//while (m_blnOpenVideoThreadStateFlag == false) {}

	if (type == 0)
	{
		m_blnOpenVideoResult = m_pVideo->OpenMovieNormally(string(m_FileName), m_pVideoBox->m_VBox.m_VideoWnd.m_hWnd);
	}
	else if (type == 1)
	{
		m_blnOpenVideoResult = m_pVideo->OpenMovieAllDefault(string(m_FileName), m_pVideoBox->m_VBox.m_VideoWnd.m_hWnd);
	}
	else if (type == 2)
	{
		m_blnOpenVideoResult = m_pVideo->OpenMovieHard(string(m_FileName), m_pVideoBox->m_VBox.m_VideoWnd.m_hWnd);
	}

	fstream fout;
	fout.open(m_Dir+"\\report.log", ios::out);
	fout <<	m_pVideo->m_log;
	fout.close();

	if (m_blnOpenVideoResult == false) 
	{
		m_VIsOpen = false;
		m_pVideoBox->m_lblVB.SetWindowText("VideoBox");
		m_FileName = "";
		m_blnReopenVideo = false;
		return;
	}
	
	//m_Video.SetVideoWindowPlacement(m_pVideoBox->m_VBox.m_VideoWnd.m_hWnd);

	InitIPData((int)m_pVideo->m_Width, (int)m_pVideo->m_Height, 1);

	m_pVideoBox->m_TB.SetScrollPos(0);
	m_pVideoBox->m_TB.SetScrollRange(0, (int)(m_pVideo->m_Duration/(s64)10000));

	i=csFileName.GetLength()-1;
	while (csFileName[i] != '\\') i--;

	m_pVideoBox->m_lblVB.SetWindowText("VideoBox \""+csFileName.Mid(i+1, csFileName.GetLength()-i-1)+"\"");

	if (m_blnReopenVideo == false) 
	{
		m_BegTime = 0;
		m_EndTime = m_pVideo->m_Duration;
	}

	m_pPanel->m_SHPanel.m_lblBTA1.SetWindowText(ConvertVideoTime(m_BegTime));
	m_pPanel->m_SHPanel.m_lblBTA2.SetWindowText(ConvertVideoTime(m_EndTime));

	m_w = m_pVideo->m_Width;
	m_h = m_pVideo->m_Height;
	m_BufferSize = m_w*m_h*sizeof(int);
	m_VIsOpen = true;

	CRect rc, rcP, rcVB, rVB, rcVW, rVW, rcIB, rImB;
	int w, wmax, h, ww, hh, dw, dh, dwi, dhi;

	this->GetClientRect(rc);
	m_pPanel->GetWindowRect(rcP);
	this->ScreenToClient(rcP);

	//m_pVideoBox->MoveWindow(5, 5, 200, 200);

	m_pVideoBox->GetClientRect(rcVB);
	m_pVideoBox->GetWindowRect(rVB);
	m_pVideoBox->m_VBox.m_VideoWnd.GetClientRect(rcVW);
	m_pVideoBox->m_VBox.m_VideoWnd.GetWindowRect(rVW);
	
	dw = rVB.Width() - rcVW.Width();
	dh = rVB.Height() - rcVW.Height();

	ww = (int)((double)rc.Width()*0.49);
	hh = (int)((double)rcP.top*0.98);

	wmax = ((hh-dh)*m_w)/m_h;
	if (ww-dw < wmax) wmax = ww-dw;
	
	w = wmax;
	h = ((w*m_h)/m_w);

	m_pVideoBox->MoveWindow(((rc.Width()/2-(w+dw))*3)/4, 5, (w+dw+10), (h+dh+10));
	m_pVideoBox->MoveWindow(((rc.Width()/2-(w+dw))*3)/4, 5, (w+dw), (h+dh));

	m_pImageBox->m_IBox.GetClientRect(rcIB);
	m_pImageBox->GetWindowRect(rImB);
	
	dwi = rImB.Width() - rcIB.Width();
	dhi = rImB.Height() - rcIB.Height();

	m_pImageBox->MoveWindow(rc.Width()/2+(rc.Width()/2-(w+dwi))/4, 5, w+dwi, h+dhi);

	/*m_pVideoBox->MoveWindow((rc.Width()/2-w)*3/4, 5, w, h);
	m_pImageBox->MoveWindow(rc.Width()/2+(rc.Width()/2-w)/4, 5, w, h);

	m_pVideoBox->m_VBox.m_VideoWnd.GetClientRect(rcVB);
	hh = h + (rcVB.Width()*m_h)/m_w - rcVB.Height();
	m_pVideoBox->MoveWindow((rc.Width()/2-w)*3/4, 5, w, hh);

	m_pImageBox->m_IBox.GetClientRect(rcIB);
	hh = h + (rcIB.Width()*m_h)/m_w - rcIB.Height();
	m_pImageBox->MoveWindow(rc.Width()/2+(rc.Width()/2-w)/4, 5, w, hh);

	m_pVideoBox->GetWindowRect(rcVB);

	if (rcVB.bottom >= rcP.top)
	{
		h = rcVB.Height() - (rcVB.bottom-rcP.top) - 5;
		m_pVideoBox->MoveWindow((rc.Width()/2-w)*3/4, 5, w, h);

		m_pVideoBox->m_VBox.m_VideoWnd.GetClientRect(rcVB);
		w += (rcVB.Height()*m_w)/m_h - rcVB.Width();
		m_pVideoBox->MoveWindow((rc.Width()/2-w)*3/4, 5, w, h);

		m_pImageBox->MoveWindow(rc.Width()/2+(rc.Width()/2-w)/4, 5, w, h);

		m_pImageBox->m_IBox.GetClientRect(rcIB);
		h += (rcIB.Width()*m_h)/m_w - rcIB.Height();
		m_pImageBox->MoveWindow(rc.Width()/2+(rc.Width()/2-w)/4, 5, w, h);
	}*/
	
	this->RedrawWindow();

	m_pVideo->SetImageGeted(false);
	m_pVideo->RunWithTimeout(100);
	m_pVideo->Pause();
	
	m_dt = Cur = m_pVideo->GetPos();

	m_pVideo->SetImageGeted(false);
	m_pVideo->RunWithTimeout(100);
	m_pVideo->Pause();

	Cur = m_pVideo->GetPos();
	m_dt = Cur-m_dt;

	if (m_dt == 0)
	{
		m_dt = Cur;

		m_pVideo->SetImageGeted(false);
		m_pVideo->RunWithTimeout(100);
		m_pVideo->Pause();

		Cur = m_pVideo->GetPos();
		m_dt = Cur-m_dt;
	}

	if ( (m_dt >= 10000000/10) || (m_dt <= 0) ) m_dt = 10000000/25;

	Cur = m_BegTime;
	m_pVideo->SetPos(Cur);
	
	m_pVideo->SetImageGeted(false);
	m_pVideo->RunWithTimeout(100);
	m_pVideo->Pause();

	m_vs = Pause;
	m_pVideoBox->m_VBar.SetButtonStyle(0, TBBS_CHECKBOX);
	m_pVideoBox->m_VBar.SetButtonStyle(1, TBBS_CHECKBOX | TBBS_CHECKED);
	m_pVideoBox->m_VBar.SetButtonStyle(2, TBBS_CHECKBOX);

	m_pPanel->m_SSPanel.OnBnClickedTest();

	m_EndTimeStr = CString("/") + ConvertVideoTime(m_pVideo->m_Duration);

	if (m_VTimerFuncID == 0) 
	{
		m_VTimerFuncID = 1;
		m_ct = -1;
		this->SetTimer(m_VTimerFuncID, 100, (TIMERPROC)VTimerFunc);
	}

	if (m_blnReopenVideo == false)
	{
		Cur = m_dt;
		m_pVideo->SetPos(Cur);
	}

	m_blnReopenVideo = false;
}

/*void CMainFrame::GetImage(int *Im, int *Temp)
{
	int x, y, i, j;
	int w = m_w, h = m_h, w2 = m_w*2;

	HRESULT hr = m_Video.m_pGrabber->GetCurrentBuffer((long*)(&m_BufferSize),(long*)(Temp));
	
	i=0;
	j=(h-1)*w;
	for(y=0; y<h; y++)
	{
		for(x=0; x<w; x++)
		{
			Im[j] = Temp[i];
			i++;
			j++;
		}
		j = j-w2; 
	}
}*/

void CMainFrame::OnPlayPause()
{
	if (m_VIsOpen)	
	{
		if (m_vs == Play)
		{
			m_pVideo->Pause();
			m_vs = Pause;

			m_pVideoBox->m_VBar.SetButtonStyle(0, TBBS_CHECKBOX);
			m_pVideoBox->m_VBar.SetButtonStyle(1, TBBS_CHECKBOX | TBBS_CHECKED);
			m_pVideoBox->m_VBar.SetButtonStyle(2, TBBS_CHECKBOX);
		}
		else
		{
			m_pVideo->Run();
			m_vs = Play;

			m_pVideoBox->m_VBar.SetButtonStyle(0, TBBS_CHECKBOX | TBBS_CHECKED);
			m_pVideoBox->m_VBar.SetButtonStyle(1, TBBS_CHECKBOX);
			m_pVideoBox->m_VBar.SetButtonStyle(2, TBBS_CHECKBOX);
		}
	}
}

void CMainFrame::OnStop()
{
	if (m_VIsOpen)	
	{
		m_pVideo->StopFast();
		m_vs = Stop;
		
		m_pVideoBox->m_VBar.SetButtonStyle(0, TBBS_CHECKBOX);
		m_pVideoBox->m_VBar.SetButtonStyle(1, TBBS_CHECKBOX);
		m_pVideoBox->m_VBar.SetButtonStyle(2, TBBS_CHECKBOX | TBBS_CHECKED);
	}
}

void CMainFrame::PauseVideo()
{
	if (m_VIsOpen && (m_vs != Pause))	
	{
		m_pVideo->Pause();
		m_vs = Pause;

		m_pVideoBox->m_VBar.SetButtonStyle(0, TBBS_CHECKBOX);
		m_pVideoBox->m_VBar.SetButtonStyle(1, TBBS_CHECKBOX | TBBS_CHECKED);
		m_pVideoBox->m_VBar.SetButtonStyle(2, TBBS_CHECKBOX);
	}
}

void CMainFrame::LoadSettings(CString fname)
{
	ifstream fin;

	fin.open(fname, ios::in);
	
	ReadProperty(fin, g_mthr, "moderate_threshold");
	ReadProperty(fin, g_mvthr, "moderate_threshold_for_VEdges");
	ReadProperty(fin, g_mhthr, "moderate_threshold_for_HEdges");
	ReadProperty(fin, g_mnthr, "moderate_threshold_for_NEdges");
	ReadProperty(fin, g_segw, "segment_width");
	ReadProperty(fin, g_segh, "segment_height");
	ReadProperty(fin, g_msegc, "minimum_segments_count");
	ReadProperty(fin, g_scd, "min_sum_color_diff");
	ReadProperty(fin, g_smcd, "min_sum_multiple_color_diff");
	ReadProperty(fin, g_btd, "between_text_distace");
	ReadProperty(fin, g_tco, "text_centre_offset");
	ReadProperty(fin, g_tcpo, "text_centre_percent_offset");

	ReadProperty(fin, g_mpn, "min_points_number");
	ReadProperty(fin, g_mpd, "min_points_density");
	ReadProperty(fin, g_mpvd, "min_VEdges_points_density_(per_full_line)");
	ReadProperty(fin, g_mphd, "min_HEdges_points_density_(per_full_line)");
	ReadProperty(fin, g_mpnd, "min_NEdges_points_density_(per_full_line)");
	ReadProperty(fin, g_mpved, "min_VEdges_points_density");
	ReadProperty(fin, g_mpned, "min_NEdges_points_density");

	ReadProperty(fin, g_DL, "sub_frame_length");
	ReadProperty(fin, g_tp, "text_procent");
	ReadProperty(fin, g_mtpl, "min_text_len_(in_procent)");
	ReadProperty(fin, g_sse, "sub_square_error");
	ReadProperty(fin, g_veple, "vedges_points_line_error");

	fin.close();

	m_pPanel->m_SSPanel.RedrawWindow();
}

void CMainFrame::SaveSettings(CString fname)
{
	ofstream fout;

	fout.open(fname, ios::out);

	WriteProperty(fout, g_mthr, "moderate_threshold");
	WriteProperty(fout, g_mvthr, "moderate_threshold_for_VEdges");
	WriteProperty(fout, g_mhthr, "moderate_threshold_for_HEdges");
	WriteProperty(fout, g_mnthr, "moderate_threshold_for_NEdges");
	WriteProperty(fout, g_segw, "segment_width");
	WriteProperty(fout, g_segh, "segment_height");
	WriteProperty(fout, g_msegc, "minimum_segments_count");
	WriteProperty(fout, g_scd, "min_sum_color_diff");
	WriteProperty(fout, g_smcd, "min_sum_multiple_color_diff");
	WriteProperty(fout, g_btd, "between_text_distace");
	WriteProperty(fout, g_tco, "text_centre_offset");
	WriteProperty(fout, g_tcpo, "text_centre_percent_offset");

	WriteProperty(fout, g_mpn, "min_points_number");
	WriteProperty(fout, g_mpd, "min_points_density");
	WriteProperty(fout, g_mpvd, "min_VEdges_points_density_(per_full_line)");
	WriteProperty(fout, g_mphd, "min_HEdges_points_density_(per_full_line)");
	WriteProperty(fout, g_mpnd, "min_NEdges_points_density_(per_full_line)");
	WriteProperty(fout, g_mpved, "min_VEdges_points_density");
	WriteProperty(fout, g_mpned, "min_NEdges_points_density");

	WriteProperty(fout, g_DL, "sub_frame_length");
	WriteProperty(fout, g_tp, "text_procent");
	WriteProperty(fout, g_mtpl, "min_text_len_(in_procent)");
	WriteProperty(fout, g_sse, "sub_square_error");
	WriteProperty(fout, g_veple, "vedges_points_line_error");

	fout.close();
}

void CMainFrame::OnEditSetBeginTime()
{
	if (m_VIsOpen)
	{
		s64 Cur;
	
		Cur = m_pVideo->GetPos();

		m_pPanel->m_SHPanel.m_lblBTA1.SetWindowText(ConvertVideoTime(Cur));

		m_BegTime = Cur;
	}
}

void CMainFrame::OnEditSetEndTime()
{
	if (m_VIsOpen)
	{
		s64 Cur;
	
		Cur = m_pVideo->GetPos();

		m_pPanel->m_SHPanel.m_lblBTA2.SetWindowText(ConvertVideoTime(Cur));

		m_EndTime = Cur;
	}
}

void CMainFrame::OnFileLoadSettings()
{
	CString csFileName;

	csFileName = m_SettingsFileName;

	OPENFILENAME ofn;       // common dialog box structure
	char szFile[260];       // buffer for file name

	// Initialize OPENFILENAME
	ZeroMemory(&ofn, sizeof(ofn));
	ofn.lStructSize = sizeof(ofn);
	ofn.hwndOwner = this->m_hWnd;
	ofn.lpstrFile = szFile;

	ofn.lpstrFile[0] = '\0';
	strcpy(ofn.lpstrFile, (char*)csFileName.GetString());
	ofn.nMaxFile = sizeof(szFile);
	ofn.lpstrFilter = "Settings Files (*.cfg)\0*.cfg\0";
	ofn.lpstrTitle = "Open Settings File";
	ofn.nFilterIndex = 1;
	ofn.lpstrFileTitle = NULL;
	ofn.nMaxFileTitle = 0;
	ofn.lpstrInitialDir = NULL;
	ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;

	if (m_blnReopenVideo == false)
	{
		if (FALSE == GetOpenFileName( &ofn )) return;
		csFileName = ofn.lpstrFile;
	}

	m_SettingsFileName = csFileName;
	LoadSettings(m_SettingsFileName);
}

void CMainFrame::OnFileSaveSettingsAs()
{
	CString csFileName;

	csFileName = m_SettingsFileName;

	OPENFILENAME ofn;       // common dialog box structure
	char szFile[260];       // buffer for file name

	// Initialize OPENFILENAME
	ZeroMemory(&ofn, sizeof(ofn));
	ofn.lStructSize = sizeof(ofn);
	ofn.hwndOwner = this->m_hWnd;
	ofn.lpstrFile = szFile;

	ofn.lpstrFile[0] = '\0';
	strcpy(ofn.lpstrFile, (char*)csFileName.GetString());
	ofn.nMaxFile = sizeof(szFile);
	ofn.lpstrFilter = "Settings Files (*.cfg)\0*.cfg\0";
	ofn.lpstrTitle = "Save Settings File";
	ofn.nFilterIndex = 1;
	ofn.lpstrFileTitle = NULL;
	ofn.nMaxFileTitle = 0;
	ofn.lpstrInitialDir = NULL;
	ofn.Flags = OFN_OVERWRITEPROMPT; 

	if (m_blnReopenVideo == false)
	{
		if (FALSE == GetSaveFileName( &ofn )) return;
		csFileName = ofn.lpstrFile;
	}

	m_SettingsFileName = csFileName;
	SaveSettings(m_SettingsFileName);
}

void CMainFrame::OnFileSaveSettings()
{
	SaveSettings(m_SettingsFileName);
}

void VTimerFunc(HWND hwnd, UINT uMsg, UINT_PTR idEvent, DWORD dwTime)
{
	CMainFrame *pMF = (CMainFrame*)(CWnd::FromHandle(hwnd));
	
	s64 Cur;
	
	Cur = pMF->m_pVideo->GetPos();

	if (Cur != pMF->m_ct) 
	{
		pMF->m_pVideoBox->m_lblTIME.SetWindowText(ConvertVideoTime(Cur) + pMF->m_EndTimeStr);
		pMF->m_ct = Cur;
	}

	pMF->m_pVideoBox->m_TB.SetScrollPos((int)(Cur/(s64)10000));
}

DWORD WINAPI ThreadOpenVideo(PVOID pParam)
{
	CMainFrame* pMF = (CMainFrame*)pParam;

	pMF->m_blnOpenVideoThreadStateFlag = false;

	if (pMF->m_type == 0)
	{
		pMF->m_blnOpenVideoResult = pMF->m_pVideo->OpenMovieNormally(string(pMF->m_FileName), pMF->m_pVideoBox->m_VBox.m_VideoWnd.m_hWnd);
	}
	else if (pMF->m_type == 1)
	{
		pMF->m_blnOpenVideoResult = pMF->m_pVideo->OpenMovieAllDefault(string(pMF->m_FileName), pMF->m_pVideoBox->m_VBox.m_VideoWnd.m_hWnd);
	}
	else if (pMF->m_type == 2)
	{
		pMF->m_blnOpenVideoResult = pMF->m_pVideo->OpenMovieHard(string(pMF->m_FileName), pMF->m_pVideoBox->m_VBox.m_VideoWnd.m_hWnd);
	}

	pMF->m_blnOpenVideoThreadStateFlag = true;

	return 0;
}

//void TimerFunctionForOpeningVideo(HWND hwnd, UINT uMsg, UINT_PTR idEvent, DWORD dwTime)
//{
//	throw(1);
//}

CString VideoTimeToStr2(s64 pos)
{
	CString Str;
	static char str[100];
	int hour, min, sec, sec_1000, vl;
	
	vl = (int)(pos/10000000);
	hour = vl/3600;
	vl -= hour*3600;
	min = vl/60;
	vl -= min*60;
	sec = vl;
	
	itoa(hour,str,10);
	Str += "0"+CString(str)+":";
	
	itoa(min,str,10);
	if (min<=9)
	{
		Str += "0"+CString(str)+":";
	}
	else Str += CString(str)+":";

	itoa(sec,str,10);
	if (sec<=9)
	{
		Str += "0"+CString(str)+",";
	}
	else Str += CString(str)+",";

	sec_1000 = (int)((pos%10000000)/10000);
	itoa(sec_1000,str,10);
	if (sec_1000<=9)
	{
		Str += "00"+CString(str);
	}
	else 
	{
		if (sec_1000<=99)
		{
			Str += "0"+CString(str);
		}
		else Str += CString(str);
	}

	return Str;
}

s64 GetVideoTime(int minute, int sec, int mili_sec)
{
	s64 res;
	res = (s64)((minute*60+sec)*1000+mili_sec)*(s64)10000;
	return res;
}

CString ConvertVideoTime(s64 pos)
{
	CString Str;
	static char str[100];
	int hour, min, sec, sec_1000, vl;
	
	vl = (int)(pos/10000000);
	hour = vl/3600;
	vl -= hour*3600;
	min = vl/60;
	vl -= min*60;
	sec = vl;
	
	itoa(hour,str,10);
	if (hour<10) Str = "0";
	Str += CString(str)+":";
	
	itoa(min,str,10);
	if (min<10)	Str += "0";
	Str += CString(str)+":";

	itoa(sec,str,10);
	if (sec<10) Str += "0";
	Str += CString(str)+",";

	sec_1000 = (int)((pos%10000000)/10000);
	itoa(sec_1000,str,10);
	if (sec_1000<100) Str += "0";
	if (sec_1000<10) Str += "0";
	Str += CString(str);

	return Str;
}

void CMainFrame::OnClose()
{
	if (m_VTimerFuncID == 1) 
	{
		this->KillTimer(m_VTimerFuncID);
	}

	if (g_IsCreateClearedTextImages == 1)
	{
		g_RunCreateClearedTextImages = 0;
		SetThreadPriority(m_pPanel->m_OCRPanel.m_hSearchThread, THREAD_PRIORITY_HIGHEST);
	}

	if ( (IsSearching == 0) && (m_FileName != "") )
	{
		fstream fout;

		fout.open(m_Dir+"\\previous_video.inf", ios::out);

		fout <<	m_FileName << '\n';

		fout <<	m_BegTime << '\n';

		fout <<	m_EndTime << '\n';

		fout <<	m_type << '\n';

		fout.close();
	}

	if (IsSearching == 1)
	{
		IsClose = 1;
		g_RunSubSearch = 0;
		SetThreadPriority(m_pPanel->m_SHPanel.m_hSearchThread, THREAD_PRIORITY_HIGHEST);
	}

	clock_t start_t = clock();

	while( ((clock() - start_t) < 2000) && ( (IsSearching == 1) || (g_IsCreateClearedTextImages == 1) ) ){}

	if (IsSearching == 1)
	{
		TerminateThread(m_pPanel->m_SHPanel.m_hSearchThread, 0);
	}

	if (g_IsCreateClearedTextImages == 1)
	{
		TerminateThread(m_pPanel->m_OCRPanel.m_hSearchThread, 0);
	}

	ReleaseIPData();

	CMDIFrameWnd::OnClose();
}

void CMainFrame::OnFileOpenPreviousVideo()
{
	char str[300];
	fstream fin;

	fin.open(m_Dir+"\\previous_video.inf", ios::in);
	
	fin.getline(str, 300);
	m_FileName = str;

	fin.getline(str, 300);
	m_BegTime = (s64)strtod(str, NULL);

	fin.getline(str, 300);
	m_EndTime = (s64)strtod(str, NULL);

	fin.getline(str, 300);
	m_type = (int)strtod(str, NULL);

	fin.close();

	m_blnReopenVideo = true;

	OnFileOpenVideo(m_type);
}

void CMainFrame::ClearDir(CString DirName)
{
	CString Str;
	WIN32_FIND_DATA FindFileData;
	HANDLE hFind;
	BOOL FileFinded;
	vector<CString> FileNamesVector;
	int i;
	
	Str = m_Dir + CString("\\") + DirName + CString("\\");

	hFind = FindFirstFile(Str + "*.*", &FindFileData);
	if (hFind != INVALID_HANDLE_VALUE) FileFinded = true;
	else return;
	
	do
	{
		if ( (FindFileData.cFileName != CString(".")) && 
			 (FindFileData.cFileName != CString("..")) )
		{
			FileNamesVector.push_back(FindFileData.cFileName);
		}

		FileFinded = FindNextFile(hFind,&FindFileData);
	} 
	while (FileFinded);

	for(i=0; i<(int)FileNamesVector.size(); i++)
	{
		DeleteFile(Str + FileNamesVector[i]);
	}
	
	FileNamesVector.clear();
}

void WriteProperty(ofstream &fout, int val, CString Name)
{
	fout << Name << " = " << val << '\n';
}

void WriteProperty(ofstream &fout, double val, CString Name)
{
	fout << Name << " = " << val << '\n';
}

void ReadProperty(ifstream &fin, int &val, CString Name)
{
	char name[100], str[100];

	fin.seekg(0);
	do
	{
		fin >> name;
		fin >> str;
		fin >> str;
	} while((Name != CString(name)) && !fin.eof());
	
	if (!fin.eof()) 
	{
		val = (int)strtod(str, NULL);
	}
}

void ReadProperty(ifstream &fin, double &val, CString Name)
{
	char name[100], str[100];

	fin.seekg(0);
	do
	{
		fin >> name;
		fin >> str;
		fin >> str;
	} while((Name != CString(name)) && !fin.eof());
	
	if (!fin.eof()) 
	{
		val = strtod(str, NULL);
	}
}

const DWORD _MMX_FEATURE_BIT = 0x00800000;
const DWORD _SSE2_FEATURE_BIT = 0x04000000;

static bool _IsFeature(DWORD nFeature);

bool IsMMX_and_SSE()
{
	static bool bMMX = _IsFeature(_MMX_FEATURE_BIT);
	return(bMMX);
}

bool IsSSE2()
{
	static bool bSSE2 = _IsFeature(_SSE2_FEATURE_BIT);
	return(bSSE2);
}

static bool _IsFeature(DWORD dwRequestFeature)
{
	// This	bit	flag can get set on	calling	cpuid
	// with	register eax set to	1
	DWORD dwFeature	= 0;
	__try {
			_asm {
				mov	eax,1
				cpuid
				mov	dwFeature,edx
			}
	} __except ( EXCEPTION_EXECUTE_HANDLER)	{
			return false;
	}
	if ((dwRequestFeature == _MMX_FEATURE_BIT) &&
		(dwFeature & _MMX_FEATURE_BIT)) {
		__try {
			__asm {
				pxor mm0, mm0
				pmaxsw mm0, mm0
				emms
			}
		} __except (EXCEPTION_EXECUTE_HANDLER) {
			return (0);
		}
		return(true);
	}
	else if ((dwRequestFeature == _SSE2_FEATURE_BIT) &&
		(dwFeature & _SSE2_FEATURE_BIT)) {
		__try {
			__asm {
				xorpd xmm0, xmm0
			}
		} __except (EXCEPTION_EXECUTE_HANDLER) {
			return (0);
		}
		return(true);
	}

	return false;
}
void CMainFrame::OnSetpriorityIdle()
{
	m_Menu.CheckMenuItem(ID_SETPRIORITY_IDLE, MF_CHECKED);
	m_Menu.CheckMenuItem(ID_SETPRIORITY_BELOWNORMAL, MF_UNCHECKED);
	m_Menu.CheckMenuItem(ID_SETPRIORITY_NORMAL, MF_UNCHECKED);
	m_Menu.CheckMenuItem(ID_SETPRIORITY_ABOVENORMAL, MF_UNCHECKED);
	m_Menu.CheckMenuItem(ID_SETPRIORITY_HIGH, MF_UNCHECKED);

	HANDLE m_hCurrentProcess = GetCurrentProcess();

	BOOL res = SetPriorityClass(m_hCurrentProcess, IDLE_PRIORITY_CLASS);

	res = res;
}

void CMainFrame::OnSetpriorityBelownormal()
{
	m_Menu.CheckMenuItem(ID_SETPRIORITY_IDLE, MF_UNCHECKED);
	m_Menu.CheckMenuItem(ID_SETPRIORITY_BELOWNORMAL, MF_CHECKED);
	m_Menu.CheckMenuItem(ID_SETPRIORITY_NORMAL, MF_UNCHECKED);
	m_Menu.CheckMenuItem(ID_SETPRIORITY_ABOVENORMAL, MF_UNCHECKED);
	m_Menu.CheckMenuItem(ID_SETPRIORITY_HIGH, MF_UNCHECKED);

	HANDLE m_hCurrentProcess = GetCurrentProcess();

	BOOL res = SetPriorityClass(m_hCurrentProcess, BELOW_NORMAL_PRIORITY_CLASS);

	res = res;
}

void CMainFrame::OnSetpriorityNormal()
{
	m_Menu.CheckMenuItem(ID_SETPRIORITY_IDLE, MF_UNCHECKED);
	m_Menu.CheckMenuItem(ID_SETPRIORITY_BELOWNORMAL, MF_UNCHECKED);
	m_Menu.CheckMenuItem(ID_SETPRIORITY_NORMAL, MF_CHECKED);
	m_Menu.CheckMenuItem(ID_SETPRIORITY_ABOVENORMAL, MF_UNCHECKED);
	m_Menu.CheckMenuItem(ID_SETPRIORITY_HIGH, MF_UNCHECKED);

	HANDLE m_hCurrentProcess = GetCurrentProcess();

	BOOL res = SetPriorityClass(m_hCurrentProcess, NORMAL_PRIORITY_CLASS);

	res = res;
}

void CMainFrame::OnSetpriorityAbovenormal()
{
	m_Menu.CheckMenuItem(ID_SETPRIORITY_IDLE, MF_UNCHECKED);
	m_Menu.CheckMenuItem(ID_SETPRIORITY_BELOWNORMAL, MF_UNCHECKED);
	m_Menu.CheckMenuItem(ID_SETPRIORITY_NORMAL, MF_UNCHECKED);
	m_Menu.CheckMenuItem(ID_SETPRIORITY_ABOVENORMAL, MF_CHECKED);
	m_Menu.CheckMenuItem(ID_SETPRIORITY_HIGH, MF_UNCHECKED);

	HANDLE m_hCurrentProcess = GetCurrentProcess();

	BOOL res = SetPriorityClass(m_hCurrentProcess, ABOVE_NORMAL_PRIORITY_CLASS);

	res = res;
}

void CMainFrame::OnSetpriorityHigh()
{
	m_Menu.CheckMenuItem(ID_SETPRIORITY_IDLE, MF_UNCHECKED);
	m_Menu.CheckMenuItem(ID_SETPRIORITY_BELOWNORMAL, MF_UNCHECKED);
	m_Menu.CheckMenuItem(ID_SETPRIORITY_NORMAL, MF_UNCHECKED);
	m_Menu.CheckMenuItem(ID_SETPRIORITY_ABOVENORMAL, MF_UNCHECKED);
	m_Menu.CheckMenuItem(ID_SETPRIORITY_HIGH, MF_CHECKED);

	HANDLE m_hCurrentProcess = GetCurrentProcess();

	BOOL res = SetPriorityClass(m_hCurrentProcess, HIGH_PRIORITY_CLASS);

	res = res;
}
