                              //MainFrm.h//                                
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
#include "VideoBox.h"
#include "ImageBox.h"
#include "DSVideoLoader.h"
#include "SSAlgorithms.h"
#include "IPAlgorithms.h"
#include <time.h>
#include <fstream>

using namespace std;

s64 GetVideoTime(int minute, int sec, int mili_sec);
CString ConvertVideoTime(s64 pos);
CString VideoTimeToStr2(s64 pos);

class CMainFrame : public CMDIFrameWnd
{
public:
	CMainFrame();
	~CMainFrame();

public:
	CString		m_Dir;

	CMenu		m_Menu;
	bool		m_WasInited;

	CSSOWnd		*m_pPanel;
	CVideoBox	*m_pVideoBox;
	CImageBox	*m_pImageBox;

	CVideo		*m_pVideo;

	bool        m_VIsOpen;
	CDocManager m_DocManager;
	CString     m_FileName;

	CString     m_EndTimeStr;

	UINT_PTR	m_VTimerFuncID;

	enum {Play, Pause, Stop} m_vs;

	int			m_BufferSize;
	int			m_w;
	int			m_h;

	s64			m_dt;
	s64         m_ct;

	s64			m_BegTime;
	s64			m_EndTime;

	CString		m_SettingsFileName;

	bool		m_blnReopenVideo;

	int			m_type;

	DWORD		m_dwOpenVideoThreadID; 
	HANDLE		m_hOpenVideoThread;
	bool		m_blnOpenVideoThreadStateFlag;
	bool		m_blnOpenVideoResult;

public:
	void Init();
	void ResizeControls();

	//void GetImage(int *Im, int *Temp);
	
	void PauseVideo();

	void LoadSettings(CString fname);
	void SaveSettings(CString fname);

	void OnFileOpenVideo(int type);
	
	void ClearDir(CString DirName);

protected:
	DECLARE_MESSAGE_MAP()

public:
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnPlayPause();
	afx_msg void OnStop();
	afx_msg void OnFileReOpenVideo();
	afx_msg void OnFileOpenVideoNormally();
	afx_msg void OnFileOpenVideoAllDefault();
	afx_msg void OnFileOpenVideoHard();
	afx_msg void OnEditSetBeginTime();
	afx_msg void OnEditSetEndTime();
	afx_msg void OnFileSaveSettings();
	afx_msg void OnFileLoadSettings();
	afx_msg void OnFileSaveSettingsAs();
	afx_msg void OnClose();
	afx_msg void OnFileOpenPreviousVideo();
	afx_msg void OnSetpriorityIdle();
//	afx_msg void OnRetertErtrt();
	afx_msg void OnSetpriorityNormal();
	afx_msg void OnSetpriorityBelownormal();
	afx_msg void OnSetpriorityAbovenormal();
	afx_msg void OnSetpriorityHigh();
};

void VTimerFunc(HWND hwnd, UINT uMsg, UINT_PTR idEvent, DWORD dwTime);

int GetEncoderClsid(const WCHAR* format, CLSID* pClsid);

void WriteProperty(ofstream &fout, int val, CString Name);
void WriteProperty(ofstream &fout, bool val, CString Name);
void WriteProperty(ofstream &fout, double val, CString Name);
void ReadProperty(ifstream &fin, int &val, CString Name);
void ReadProperty(ifstream &fin, bool &val, CString Name);
void ReadProperty(ifstream &fin, double &val, CString Name);

bool IsMMX_and_SSE();
bool IsSSE2();

DWORD WINAPI ThreadOpenVideo(PVOID pParam);