
//////////////////////////////////////////////////////////////////////////////////
//							MainFrm.cpp  Version 1.75							//
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
#include ".\mainfrm.h"

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
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////

CMainFrame::CMainFrame()
{
	m_WasInited = false;
	m_VIsOpen = false;
	m_VTimerFuncID = 0;

	char dir[300];
	GetCurrentDirectory(300,dir);
	m_Dir = dir;

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

	this->SetTitle("VideoSubFinder 1.75 beta version");
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

	g_pMF = this;

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
	s64 Cur, Stop;
	long evCode;
	bool bln;
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
	ofn.lpstrFilter = "Video Files (*.avi;*.mpg;*.mpeg;*.mpv;*.m1v;*.dat;*.avs;*.vdr;*.asf;*.asx;*.wmv;*.mkv;*.ogm)\0*.avi;*.mpg;*.mpeg;*.mpv;*.m1v;*.dat;*.avs;*.vdr;*.asf;*.asx;*.wmv;*.mkv;*.ogm\0All Files (*.*)\0*.*\0";
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

	if (type == 0)
	{
		bln = m_Video.OpenMovieNormally(m_FileName, m_pVideoBox->m_VBox.m_VideoWnd.m_hWnd);
	}
	else if (type == 1)
	{
		bln = m_Video.OpenMovieAllDefault(m_FileName, m_pVideoBox->m_VBox.m_VideoWnd.m_hWnd);
	}
	else if (type == 2)
	{
		bln = m_Video.OpenMovieHard(m_FileName, m_pVideoBox->m_VBox.m_VideoWnd.m_hWnd);
	}
	
	fstream fout;
	fout.open(m_Dir+"\\report.log", ios::out);
	fout <<	m_Video.m_log;
	fout.close();

	if (bln == false) 
	{
		m_VIsOpen = false;
		m_pVideoBox->m_lblVB.SetWindowText("VideoBox");
		m_FileName = "";
		m_blnReopenVideo = false;
		return;
	}
	
	InitIPData((int)m_Video.m_Width, (int)m_Video.m_Height, 1);

	m_pVideoBox->m_TB.SetScrollPos(0);
	m_pVideoBox->m_TB.SetScrollRange(0, (int)(m_Video.m_Duration/(s64)10000));

	i=csFileName.GetLength()-1;
	while (csFileName[i] != '\\') i--;

	m_pVideoBox->m_lblVB.SetWindowText("VideoBox \""+csFileName.Mid(i+1, csFileName.GetLength()-i-1)+"\"");

	if (m_blnReopenVideo == false) 
	{
		m_BegTime = 0;
		m_EndTime = m_Video.m_Duration;
	}

	m_pPanel->m_SHPanel.m_lblBTA1.SetWindowText(ConvertVideoTime(m_BegTime));
	m_pPanel->m_SHPanel.m_lblBTA2.SetWindowText(ConvertVideoTime(m_EndTime));

	m_w = m_Video.m_Width;
	m_h = m_Video.m_Height;
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

	m_Video.m_SGCallback.m_ImageGeted = false;
	m_Video.m_pMC->Run();
	m_Video.m_pME->WaitForCompletion(100, &evCode);
	m_Video.m_pMC->Pause();
	
	m_Video.m_pMS->GetPositions(&Cur, &Stop);
	m_dt = Cur;

	m_Video.m_SGCallback.m_ImageGeted = false;
	m_Video.m_pMC->Run();
	m_Video.m_pME->WaitForCompletion(100, &evCode);
	m_Video.m_pMC->Pause();

	m_Video.m_pMS->GetPositions(&Cur, &Stop);
	m_dt = Cur-m_dt;

	if (m_dt == 0)
	{
		m_dt = Cur;

		m_Video.m_SGCallback.m_ImageGeted = false;
		m_Video.m_pMC->Run();
		m_Video.m_pME->WaitForCompletion(100, &evCode);
		m_Video.m_pMC->Pause();

		m_Video.m_pMS->GetPositions(&Cur, &Stop);
		m_dt = Cur-m_dt;
	}

	if ((m_dt >= 10000000/10) || (m_dt == 0)) m_dt = 10000000/25;

	Cur = m_BegTime;
	m_Video.m_pMS->SetPositions(&Cur,AM_SEEKING_AbsolutePositioning,&Stop,AM_SEEKING_AbsolutePositioning);
	
	m_Video.m_SGCallback.m_ImageGeted = false;
	m_Video.m_pMC->Run();
	m_Video.m_pME->WaitForCompletion(100, &evCode);
	m_Video.m_pMC->Pause();

	m_vs = Pause;
	m_pVideoBox->m_VBar.SetButtonStyle(0, TBBS_CHECKBOX);
	m_pVideoBox->m_VBar.SetButtonStyle(1, TBBS_CHECKBOX | TBBS_CHECKED);
	m_pVideoBox->m_VBar.SetButtonStyle(2, TBBS_CHECKBOX);

	m_pPanel->m_SSPanel.OnBnClickedTest();

	m_EndTimeStr = CString("/") + ConvertVideoTime(m_Video.m_Duration);

	if (m_VTimerFuncID == 0) 
	{
		m_VTimerFuncID = 1;
		m_ct = -1;
		this->SetTimer(m_VTimerFuncID, 100, (TIMERPROC)VTimerFunc);
	}

	if (m_blnReopenVideo == false)
	{
		Cur = m_dt;
		m_Video.m_pMS->SetPositions(&Cur,AM_SEEKING_AbsolutePositioning,&Stop,AM_SEEKING_AbsolutePositioning);
	}

	m_blnReopenVideo = false;
}

void CMainFrame::GetImage(int *Im, int *Temp)
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
}

void CMainFrame::OnPlayPause()
{
	if (m_VIsOpen)	
	{
		if (m_vs == Play)
		{
			m_Video.m_pMC->Pause();
			m_vs = Pause;

			m_pVideoBox->m_VBar.SetButtonStyle(0, TBBS_CHECKBOX);
			m_pVideoBox->m_VBar.SetButtonStyle(1, TBBS_CHECKBOX | TBBS_CHECKED);
			m_pVideoBox->m_VBar.SetButtonStyle(2, TBBS_CHECKBOX);
		}
		else
		{
			m_Video.m_pMC->Run();
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
		m_Video.m_pMC->Stop();
		m_vs = Stop;
		
		s64 Pos, endPos;
		endPos = m_Video.m_Duration;
		Pos = 0;
		m_Video.m_pMS->SetPositions(&Pos,AM_SEEKING_AbsolutePositioning,&endPos,AM_SEEKING_AbsolutePositioning);

		m_Video.m_pMC->Run();
		m_Video.m_pMC->Pause();
		m_vs = Pause;

		m_pVideoBox->m_VBar.SetButtonStyle(0, TBBS_CHECKBOX);
		m_pVideoBox->m_VBar.SetButtonStyle(1, TBBS_CHECKBOX);
		m_pVideoBox->m_VBar.SetButtonStyle(2, TBBS_CHECKBOX | TBBS_CHECKED);
	}
}

void CMainFrame::PauseVideo()
{
	if (m_VIsOpen && (m_vs != Pause))	
	{
		m_Video.m_pMC->Pause();
		m_vs = Pause;

		m_pVideoBox->m_VBar.SetButtonStyle(0, TBBS_CHECKBOX);
		m_pVideoBox->m_VBar.SetButtonStyle(1, TBBS_CHECKBOX | TBBS_CHECKED);
		m_pVideoBox->m_VBar.SetButtonStyle(2, TBBS_CHECKBOX);
	}
}

void CMainFrame::SaveRGBImage(int *Im, CString name, int w, int h)
{
	FILE * outfile;
	jpeg_compress_struct cinfo;
	jpeg_error_mgr jerr;
	JSAMPROW row_pointer[1];
	int row_stride, i, bi, ei, j;	
	JSAMPLE row[1024*3*4];
	JSAMPLE *color;

	if ((outfile = fopen(m_Dir+name, "wb")) == NULL) return;

	cinfo.err = jpeg_std_error(&jerr);
	jpeg_create_compress(&cinfo);

	jpeg_stdio_dest(&cinfo, outfile);

	cinfo.image_width = w; 	
	cinfo.image_height = h;
	cinfo.input_components = 3;	
	cinfo.in_color_space = JCS_RGB; 

	jpeg_set_defaults(&cinfo);
	jpeg_set_quality(&cinfo, 100, TRUE);

	jpeg_start_compress(&cinfo, TRUE);

	row_stride = w*3;

	row_pointer[0] = (JSAMPLE*)row;

	bi = 0;
	ei = w;
	while (cinfo.next_scanline < cinfo.image_height) 
	{
	    for(i=bi, j=0; i<ei; i++, j+=3)
		{
			color = (JSAMPLE*)(&Im[i]);
			row[j] = color[2];
			row[j+1] = color[1];
			row[j+2] = color[0];	
		}
		
	    jpeg_write_scanlines(&cinfo, row_pointer, 1);
		
		bi += w;
		ei += w;
	}

	jpeg_finish_compress(&cinfo);
	jpeg_destroy_compress(&cinfo);

	fclose(outfile);
}

void CMainFrame::LoadRGBImage(int *Im, CString name, int &w, int &h)
{
	FILE *fin;
	jpeg_decompress_struct cinfo;
	jpeg_error_mgr jerr;
	JSAMPROW row_pointer[1];
	int row_stride, i, j, bi, ei;	
	JSAMPLE row[1024*3*4];
	int color = 0;
	u8 *pColor = (u8*)(&color);

	if ((fin = fopen(name, "rb")) == NULL) return;

	cinfo.err = jpeg_std_error(&jerr);

	jpeg_create_decompress(&cinfo);

	jpeg_stdio_src(&cinfo, fin);

	jpeg_read_header(&cinfo, TRUE);

	cinfo.out_color_space = JCS_RGB;
	cinfo.out_color_components = 3;

	w = cinfo.image_width; 	
	h = cinfo.image_height;

	jpeg_start_decompress(&cinfo);

	row_stride = w*3;

	row_pointer[0] = (JSAMPLE*)row;

	bi = 0;
	ei = w;
	while (cinfo.output_scanline < cinfo.output_height) 
	{
		jpeg_read_scanlines(&cinfo, row_pointer, 1);

		for(i=bi, j=0; i<ei; i++, j+=3)
		{
			pColor[2] = row[j];
			pColor[1] = row[j+1];
			pColor[0] = row[j+2];
			Im[i] = color;
		}			
		
		bi += w;
		ei += w;
	}

	jpeg_finish_decompress(&cinfo);
	jpeg_destroy_decompress(&cinfo);

	fclose(fin);
}

void CMainFrame::SaveImage(int *Im, CString name, int w, int h)
{
	FILE * outfile;
	jpeg_compress_struct cinfo;
	jpeg_error_mgr jerr;
	JSAMPROW row_pointer[1];
	int row_stride, i, bi, ei, j;	
	JSAMPLE row[1024*4];
	JSAMPLE *color;

	if ((outfile = fopen(m_Dir+name, "wb")) == NULL) return;

	cinfo.err = jpeg_std_error(&jerr);
	jpeg_create_compress(&cinfo);

	jpeg_stdio_dest(&cinfo, outfile);

	cinfo.image_width = w; 	
	cinfo.image_height = h;
	cinfo.input_components = 1;	
	cinfo.in_color_space = JCS_GRAYSCALE; 

	jpeg_set_defaults(&cinfo);
	//jpeg_set_quality(&cinfo, 100, TRUE);

	jpeg_start_compress(&cinfo, TRUE);

	row_stride = w;

	row_pointer[0] = (JSAMPLE*)row;

	bi = 0;
	ei = w;
	while (cinfo.next_scanline < cinfo.image_height) 
	{
	    for(i=bi, j=0; i<ei; i++, j++)
		{
			color = (JSAMPLE*)(&Im[i]);
			row[j] = color[0];
		}
		
	    jpeg_write_scanlines(&cinfo, row_pointer, 1);
		
		bi += w;
		ei += w;
	}

	jpeg_finish_compress(&cinfo);
	jpeg_destroy_compress(&cinfo);

	fclose(outfile);
}

void CMainFrame::LoadImage(int *Im, CString name, int &w, int &h)
{
	FILE *fin;
	jpeg_decompress_struct cinfo;
	jpeg_error_mgr jerr;
	JSAMPROW row_pointer[1];
	int row_stride, i, j, bi, ei;	
	JSAMPLE row[1024*4];

	if ((fin = fopen(name, "rb")) == NULL) return;

	cinfo.err = jpeg_std_error(&jerr);

	jpeg_create_decompress(&cinfo);

	jpeg_stdio_src(&cinfo, fin);

	jpeg_read_header(&cinfo, TRUE);

	cinfo.out_color_space = JCS_GRAYSCALE;
	cinfo.out_color_components = 1;

	w = cinfo.image_width; 	
	h = cinfo.image_height;

	jpeg_start_decompress(&cinfo);

	row_stride = w;

	row_pointer[0] = (JSAMPLE*)row;

	bi = 0;
	ei = w;
	while (cinfo.output_scanline < cinfo.output_height) 
	{
		jpeg_read_scanlines(&cinfo, row_pointer, 1);

		for(i=bi, j=0; i<ei; i++, j++)
		{
			if (row[j] < 100) 
			{
				Im[i] = 0;
			}
			else
			{
				Im[i] = 255;
			}
		}			
		
		bi += w;
		ei += w;
	}

	jpeg_finish_decompress(&cinfo);
	jpeg_destroy_decompress(&cinfo);

	fclose(fin);
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
		s64 Cur, Stop;
	
		m_Video.m_pMS->GetPositions(&Cur, &Stop);

		m_pPanel->m_SHPanel.m_lblBTA1.SetWindowText(ConvertVideoTime(Cur));

		m_BegTime = Cur;
	}
}

void CMainFrame::OnEditSetEndTime()
{
	if (m_VIsOpen)
	{
		s64 Cur, Stop;
	
		m_Video.m_pMS->GetPositions(&Cur, &Stop);

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
	
	s64 Cur, Stop;
	
	pMF->m_Video.m_pMS->GetPositions(&Cur, &Stop);

	if (Cur != pMF->m_ct) 
	{
		pMF->m_pVideoBox->m_lblTIME.SetWindowText(ConvertVideoTime(Cur) + pMF->m_EndTimeStr);
		pMF->m_ct = Cur;
	}

	pMF->m_pVideoBox->m_TB.SetScrollPos((int)(Cur/(s64)10000));
}

CString VideoTimeToStr(s64 pos)
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
	Str += CString(str)+"_";
	
	itoa(min,str,10);
	if (min<=9)
	{
		Str += "0"+CString(str)+"_";
	}
	else Str += CString(str)+"_";

	itoa(sec,str,10);
	if (sec<=9)
	{
		Str += "0"+CString(str)+"_";
	}
	else Str += CString(str)+"_";

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
		SetThreadPriority(m_pPanel->m_SHPanel.hSearchThread, THREAD_PRIORITY_HIGHEST);
	}

	while( (IsSearching == 1) || (g_IsCreateClearedTextImages == 1) ){}

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
