
//////////////////////////////////////////////////////////////////////////////////
//							MyVideo.h  Version 1.75								//
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

#include "myresource.h"
#include <strmif.h>
#include <control.h>
#include <uuids.h>
#include <errors.h>
#include <amvideo.h>
#include <qedit.h>
#include <dmodshow.h>

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

class CMyVideo
{
public:
	CMyVideo();
	~CMyVideo();
	
public:	
	CString		m_MovieName;
	bool		m_Inited;

	long		m_Width;
	long		m_Height;

	s64			m_Duration;

	CString		m_log;

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
	bool OpenMovieNormally(CString csMovieName, HWND hWnd);
	bool OpenMovieAllDefault(CString csMovieName, HWND hWnd);
	bool OpenMovieHard(CString csMovieName, HWND hWnd);

	bool CloseMovie();
	bool SetNullRender();
	void SetPos(s64 Pos);
	void SetPos(double pos);

protected:
	HRESULT CleanUp();
	HRESULT ConnectFilters(IGraphBuilder *pGraph,IBaseFilter *pFirst,IBaseFilter *pSecond);
	HRESULT GetPin(IBaseFilter *pFilter, PIN_DIRECTION PinDir, IPin **ppPin);
};

LPCWSTR CStringToLPCWSTR(CString csStr);