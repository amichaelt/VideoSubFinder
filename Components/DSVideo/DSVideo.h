                              //MyVideo.h//                                
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

#include "DataTypes.h"
#include "Video.h"
#include <strmif.h>
#include <control.h>
#include <uuids.h>
#include <errors.h>
#include <amvideo.h>
#include <qedit.h>
#include <dmodshow.h>
#include <fstream>
#include <vector>
#include <string>

using namespace std;

class MySampleGrabberCallback :	public ISampleGrabberCB
{
public:
	IMediaControl			*m_pMC; 
	bool					 m_ImageGeted;
	
	STDMETHODIMP_(ULONG) AddRef() { return 1; }
	
	STDMETHODIMP_(ULONG) Release() { return 2; }

	STDMETHODIMP QueryInterface(REFIID riid, void **ppvObject)
	{
		if (NULL == ppvObject) return E_POINTER;
		
		if (riid == __uuidof(IUnknown))
		{
			*ppvObject = static_cast<IUnknown*>(this);
			return S_OK;
		}
		
		if (riid == __uuidof(ISampleGrabberCB))
		{
			*ppvObject = static_cast<ISampleGrabberCB*>(this);
			return S_OK;
		}
		
		return E_NOTIMPL;
	}

	HRESULT STDMETHODCALLTYPE SampleCB(double SampleTime, IMediaSample *pSample)
	{
		return E_NOTIMPL;
	}

	HRESULT STDMETHODCALLTYPE BufferCB(double SampleTime, BYTE *pBuffer, long BufferLen)
	{
		if (!m_ImageGeted)
		{
			m_pMC->Pause();
			m_ImageGeted = true;
		}

		return S_OK;
	}
};

/////////////////////////////////////////////////////////////////////////////

class DSVideo: public CVideo
{
public:
	DSVideo();
	~DSVideo();
	
public:	

	int				m_ImRES[1024*768];

	IGraphBuilder	*m_pGB;
	IMediaControl	*m_pMC; 
	IMediaEventEx	*m_pME;
	IMediaSeeking	*m_pMS;
	IVideoWindow	*m_pVW;	
	IBasicVideo		*m_pBV;
	IBasicAudio     *m_pBA;
	IMediaFilter	*m_pMF;

	ISampleGrabber	*m_pGrabber;

	IBaseFilter		*m_pDecoder;
	IBaseFilter		*m_pSourceFilter;
	IBaseFilter		*m_pSampleGrabberFilter; 
	IBaseFilter		*m_pVideoRenderFilter;

	MySampleGrabberCallback m_SGCallback;

public:
	IBaseFilter* GetDecoder();
	IBaseFilter* GetSourceFilter();

	bool OpenMovieNormally(string csMovieName, HWND hWnd);
	bool OpenMovieAllDefault(string csMovieName, HWND hWnd);
	bool OpenMovieHard(string csMovieName, HWND hWnd);

	bool SetVideoWindowPlacement(HWND hWnd);
	bool SetNullRender();

	bool CloseMovie();
	
	void SetPos(s64 Pos);
	void SetPos(double pos);
	void SetPosFast(s64 Pos);

	void SetImageGeted(bool ImageGeted);

	void Run();
	void Pause();
	void StopFast();

	void RunWithTimeout(s64 timeout);

	void Stop();
    void OneStep();
	s64  OneStepWithTimeout();
	s64  GetPos();
    void GetRGBImage(int *ImRGB, int xmin, int xmax, int ymin, int ymax);

	s64 PosToMilliSeconds(s64 pos);

	void SetVideoWindowPosition(int left, int top, int width, int height);

public:
	HRESULT CleanUp();
	HRESULT ConnectFilters(IGraphBuilder *pGraph,IBaseFilter *pFirst,IBaseFilter *pSecond);
	HRESULT GetPin(IBaseFilter *pFilter, PIN_DIRECTION PinDir, IPin **ppPin);
};

LPCWSTR StringToLPCWSTR(string csStr);
string IntToCStr(int n);
string WCSToStr(WCHAR *wstr);