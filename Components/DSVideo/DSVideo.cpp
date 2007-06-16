                              //MyVideo.cpp//                                
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

//#include "stdafx.h"
#include "DSVideo.h"

/////////////////////////////////////////////////////////////////////////////

string WCSToStr(WCHAR *wstr)
{
	static char str[100];

	wcstombs( str, wstr, 100 );

	return string(str);
}

/////////////////////////////////////////////////////////////////////////////

LPCWSTR StringToLPCWSTR(string csStr)
{
	static WCHAR wName[500]; 

	int len = MultiByteToWideChar( CP_ACP, 0, csStr.c_str(), -1, NULL, 0);

	MultiByteToWideChar( CP_ACP, 0, csStr.c_str(), -1, wName, len);

	wName[len] = L'\0';

	return wName;
}

/////////////////////////////////////////////////////////////////////////////

string IntToCStr(int n)
{
	char str[100];

	itoa(n, str, 10);

	return string(str);
}

/////////////////////////////////////////////////////////////////////////////

HRESULT STDMETHODCALLTYPE MySampleGrabberCallback::BufferCB(double SampleTime, BYTE *pBuffer, long BufferLen)
{
	m_st = (s64)(SampleTime*10000000.0);

	if (!m_ImageGeted)
	{
		m_pVideo->m_pMC->Pause();
		m_ImageGeted = true;
	}

	return S_OK;
}

/////////////////////////////////////////////////////////////////////////////

HRESULT DSVideo::GetPin(IBaseFilter *pFilter, PIN_DIRECTION PinDir, IPin **ppPin)
{
    if (!pFilter || !ppPin) return E_POINTER;
    *ppPin = NULL;

    IEnumPins  *pEnum;
    IPin       *pPin;
    HRESULT hr = pFilter->EnumPins(&pEnum);
    if (FAILED(hr)) return hr;

    while(pEnum->Next(1, &pPin, 0) == S_OK)
    {
        PIN_DIRECTION PinDirThis;
        hr = pPin->QueryDirection(&PinDirThis);
        if (FAILED(hr)) break;

        if (PinDir == PinDirThis)
        {
            pEnum->Release();
            *ppPin = pPin;
            return S_OK;
        }
        pPin->Release();
    }
    pEnum->Release();
    return E_FAIL;  
}

/////////////////////////////////////////////////////////////////////////////

HRESULT DSVideo::ConnectFilters(IGraphBuilder *pGraph,IBaseFilter *pFirst,IBaseFilter *pSecond)
{
    HRESULT hr;

	if (!pGraph || !pFirst || !pSecond) return E_POINTER;

    IPin *pOut = NULL, *pIn = NULL;
    
	hr = pFirst->FindPin(L"Raw Video 1",&pOut);
	if (FAILED(hr)) hr = pFirst->FindPin(L"Video",&pOut);
	if (FAILED(hr)) hr = pFirst->FindPin(L"Video 0",&pOut);
	if (FAILED(hr)) hr = pFirst->FindPin(L"Stream 2",&pOut);
	if (FAILED(hr)) hr = GetPin(pFirst, PINDIR_OUTPUT, &pOut);
    
	if (FAILED(hr)) return hr;

    hr = GetPin(pSecond, PINDIR_INPUT, &pIn);

	if (FAILED(hr)) 
    {
        pOut->Release();
        return E_FAIL;
    }

    hr = pGraph->Connect(pOut, pIn);
    pIn->Release();
    pOut->Release();
    return hr;
}

/////////////////////////////////////////////////////////////////////////////

DSVideo::DSVideo()
{
	m_Inited=false;
	m_pGB=NULL;
	m_pMC=NULL;
	m_pME=NULL;
	m_pMS=NULL;
	m_pVW=NULL;
	m_pBV=NULL;
	m_pBA=NULL;
	m_pMF=NULL;
	m_pDecoder=NULL;
	m_pGrabber=NULL;
	m_pSourceFilter=NULL;
	m_pSampleGrabberFilter=NULL;
	m_pVideoRenderFilter=NULL;
	m_pBuilder=NULL;
	m_pBuffer=NULL;
	m_IsMSSuported = true;
	m_SGCallback.m_st = 0;
}

/////////////////////////////////////////////////////////////////////////////

DSVideo::~DSVideo()
{
	if (m_Inited) CleanUp();
}

/////////////////////////////////////////////////////////////////////////////

bool DSVideo::OpenMovieNormally(string csMovieName, void *pHWnd)
{ 	
	HRESULT hr;
	string Str;
	
	IPin *pOutGB, *pInGB, *pOutD, *pInVR;
	
	m_MovieName = csMovieName;

	if (m_Inited) 
	{
		hr = CleanUp();
	}

	Str = m_MovieName.substr(m_MovieName.size()-3, 3);
		
	if ( (Str == string("avi")) || (Str == string("mp4")) || (Str == string("mkv") ) )
	{
		hr = CoInitialize(NULL);

		hr = CoCreateInstance(CLSID_FilterGraph, NULL, CLSCTX_INPROC_SERVER, 
								IID_IGraphBuilder, (void **)&m_pGB);

		hr = CoCreateInstance(CLSID_SampleGrabber, NULL, CLSCTX_INPROC_SERVER,
								IID_IBaseFilter, (void**)&m_pSampleGrabberFilter);

		hr = CoCreateInstance(CLSID_VideoMixingRenderer, NULL, CLSCTX_INPROC_SERVER,
								IID_IBaseFilter,(void**)&m_pVideoRenderFilter);
		
		hr = m_pGB->AddSourceFilter(StringToLPCWSTR(csMovieName), L"Source", &m_pSourceFilter);
		if (hr != S_OK) { CleanUp(); return false; }
	
		hr = m_pGB->QueryInterface(IID_IMediaControl, (void **)&m_pMC);

		hr = m_pGB->QueryInterface(IID_IMediaEventEx, (void **)&m_pME);

		hr = m_pGB->QueryInterface(IID_IMediaSeeking,(void **)&m_pMS);

		hr = m_pGB->QueryInterface(IID_IMediaFilter, (void **)&m_pMF); 

		hr = m_pGB->AddFilter(m_pSampleGrabberFilter, L"SampleGrabber");

		hr = m_pGB->AddFilter(m_pVideoRenderFilter, L"Video Renderer");
		
		hr = m_pSampleGrabberFilter->QueryInterface(IID_ISampleGrabber, (void**)&m_pGrabber);

		AM_MEDIA_TYPE mt;
		ZeroMemory(&mt, sizeof(AM_MEDIA_TYPE));
		mt.majortype = MEDIATYPE_Video;
		mt.subtype = MEDIASUBTYPE_RGB32;
		mt.bTemporalCompression = false;

		hr = m_pGrabber->SetMediaType(&mt);
		hr = m_pGrabber->SetOneShot(FALSE);
		hr = m_pGrabber->SetBufferSamples(TRUE);
		hr = m_pGrabber->SetCallback(&m_SGCallback,1);
		m_SGCallback.m_pVideo = this;
		m_SGCallback.m_ImageGeted = false;

		hr = ConnectFilters(m_pGB, m_pSourceFilter, m_pSampleGrabberFilter);

		hr = ConnectFilters(m_pGB, m_pSampleGrabberFilter, m_pVideoRenderFilter);
	
		hr = m_pMF->SetSyncSource(NULL);

		hr = m_pVideoRenderFilter->QueryInterface(IID_IVideoWindow,(void **)&m_pVW);

		hr = m_pVideoRenderFilter->QueryInterface(IID_IBasicVideo,(void **)&m_pBV);
	}
	else
	{
		hr = CoInitialize(NULL);

		hr = CoCreateInstance(CLSID_FilterGraph, NULL, CLSCTX_INPROC_SERVER, 
							IID_IGraphBuilder, (void **)&m_pGB);

		hr = m_pGB->RenderFile(StringToLPCWSTR(m_MovieName), NULL);
		if (hr != S_OK) { CleanUp(); return false; }

		hr = m_pGB->QueryInterface(IID_IMediaControl, (void **)&m_pMC);

		hr = m_pGB->QueryInterface(IID_IMediaEventEx, (void **)&m_pME);

		hr = m_pGB->QueryInterface(IID_IMediaSeeking,(void **)&m_pMS);

		hr = m_pGB->QueryInterface(IID_IMediaFilter, (void **)&m_pMF); 

		hr = CoCreateInstance(CLSID_SampleGrabber, NULL, CLSCTX_INPROC_SERVER,
								IID_IBaseFilter, (void**)&m_pSampleGrabberFilter);

		hr = m_pGB->AddFilter(m_pSampleGrabberFilter, L"SampleGrabber");

		hr = m_pSampleGrabberFilter->QueryInterface(IID_ISampleGrabber, (void**)&m_pGrabber);

		AM_MEDIA_TYPE mt;
		ZeroMemory(&mt, sizeof(AM_MEDIA_TYPE));
		mt.majortype = MEDIATYPE_Video;
		mt.subtype = MEDIASUBTYPE_RGB32;
		mt.bTemporalCompression = false;

		hr = m_pGrabber->SetMediaType(&mt);
		hr = m_pGrabber->SetOneShot(FALSE);
		hr = m_pGrabber->SetBufferSamples(TRUE);
		hr = m_pGrabber->SetCallback(&m_SGCallback,1);
		m_SGCallback.m_pVideo = this;
		m_SGCallback.m_ImageGeted = false;

		hr = m_pGB->FindFilterByName(L"Video Renderer", &m_pVideoRenderFilter);
		if (hr != S_OK) { CleanUp(); return false; }

		hr = GetPin(m_pVideoRenderFilter, PINDIR_INPUT, &pInVR);
		if (hr != S_OK) { CleanUp(); return false; }

		hr = pInVR->ConnectedTo(&pOutD);
		if (hr != S_OK) { CleanUp(); return false; }

		hr = GetPin(m_pSampleGrabberFilter, PINDIR_INPUT, &pInGB);
		if (hr != S_OK) { CleanUp(); return false; }

		hr = GetPin(m_pSampleGrabberFilter, PINDIR_OUTPUT, &pOutGB);
		if (hr != S_OK) { CleanUp(); return false; }

		hr = pInVR->Disconnect();
		
		hr = pOutD->Disconnect();

		hr = m_pGB->Connect(pOutD, pInGB);
		if (hr != S_OK) { CleanUp(); return false; }

		hr = m_pGB->Connect(pOutGB, pInVR);
		if (hr != S_OK) { CleanUp(); return false; }

		hr = m_pMF->SetSyncSource(NULL);
		
		hr = m_pGB->QueryInterface(IID_IVideoWindow,(void **)&m_pVW);

		hr = m_pGB->QueryInterface(IID_IBasicVideo,(void **)&m_pBV);
		
		hr = m_pGB->QueryInterface(IID_IBasicAudio,(void **)&m_pBA);

		hr = m_pBA->put_Volume(-10000);
	}

	hr = m_pMF->SetSyncSource(NULL);


	hr = m_pVW->put_WindowStyle(WS_CHILD | WS_CLIPSIBLINGS);

	hr = m_pVW->put_Owner(*((OAHWND*)pHWnd));
	
	hr = m_pVW->put_MessageDrain(*((OAHWND*)pHWnd));

	hr = m_pBV->GetVideoSize(&m_Width,&m_Height);

	hr = m_pMS->GetStopPosition(&m_Duration);

	
	hr = m_pVW->put_WindowStyle(WS_CHILD | WS_CLIPSIBLINGS);

	hr = m_pVW->put_Owner(*((OAHWND*)pHWnd));
	
	hr = m_pVW->put_MessageDrain(*((OAHWND*)pHWnd));

	hr = m_pBV->GetVideoSize(&m_Width,&m_Height);

	hr = m_pMS->GetStopPosition(&m_Duration);

	
	m_Inited = true;

	//////////////////////////////

	this->OneStep();
	this->GetPos();
	this->OneStep();
	this->GetPos();

	/////////////////////////////

	return true;
}

bool DSVideo::OpenMovieAllDefault(string csMovieName, void *pHWnd)
{ 	
	HRESULT hr;
	string Str;
	
	m_MovieName = csMovieName;

	if (m_Inited) 
	{
		hr = CleanUp();
	}

	hr = CoInitialize(NULL);

	hr = CoCreateInstance(CLSID_FilterGraph, NULL, CLSCTX_INPROC_SERVER, 
							IID_IGraphBuilder, (void **)&m_pGB);

	hr = CoCreateInstance(CLSID_CaptureGraphBuilder2, NULL,
							CLSCTX_INPROC_SERVER, IID_ICaptureGraphBuilder2, 
							(void **)&m_pBuilder);
    
    hr = m_pBuilder->SetFiltergraph(m_pGB);

	hr = CoCreateInstance(CLSID_SampleGrabber, NULL, CLSCTX_INPROC_SERVER,
							IID_IBaseFilter, (void**)&m_pSampleGrabberFilter);

	hr = CoCreateInstance(CLSID_VideoMixingRenderer, NULL, CLSCTX_INPROC_SERVER,
							IID_IBaseFilter,(void**)&m_pVideoRenderFilter);

	hr = m_pGB->QueryInterface(IID_IMediaControl, (void **)&m_pMC);

	hr = m_pGB->QueryInterface(IID_IMediaEventEx, (void **)&m_pME);

	hr = m_pGB->QueryInterface(IID_IMediaSeeking,(void **)&m_pMS);

	hr = m_pGB->AddSourceFilter(StringToLPCWSTR(m_MovieName), L"Source1", &m_pSourceFilter);

	hr = m_pGB->AddFilter(m_pSampleGrabberFilter, L"SampleGrabber");

	hr = m_pGB->AddFilter(m_pVideoRenderFilter, L"Video Renderer");

	hr = m_pSampleGrabberFilter->QueryInterface(IID_ISampleGrabber, (void**)&m_pGrabber);

	AM_MEDIA_TYPE mt;
	ZeroMemory(&mt, sizeof(AM_MEDIA_TYPE));
	mt.majortype = MEDIATYPE_Video;
	mt.subtype = MEDIASUBTYPE_RGB32;
	mt.bTemporalCompression = false;

	hr = m_pGrabber->SetMediaType(&mt);

	hr = m_pBuilder->RenderStream(0, 0, m_pSourceFilter, m_pSampleGrabberFilter, m_pVideoRenderFilter);

	hr = m_pGrabber->SetOneShot(FALSE);
	hr = m_pGrabber->SetBufferSamples(TRUE);
	hr = m_pGrabber->SetCallback(&m_SGCallback,1);
	m_SGCallback.m_pVideo = this;
	m_SGCallback.m_ImageGeted = false;
	
	hr = m_pGB->QueryInterface(IID_IMediaFilter, (void **)&m_pMF); 

	hr = m_pGB->QueryInterface(IID_IVideoWindow,(void **)&m_pVW);

	hr = m_pGB->QueryInterface(IID_IBasicVideo,(void **)&m_pBV);
	
	hr = m_pVW->put_WindowStyle(WS_CHILD | WS_CLIPSIBLINGS);
	
	hr = m_pVW->put_Owner(*((OAHWND*)pHWnd));
	
	hr = m_pVW->put_MessageDrain(*((OAHWND*)pHWnd));

	hr = m_pBV->GetVideoSize(&m_Width, &m_Height);

	hr = m_pMS->GetStopPosition(&m_Duration);
	
	m_Inited = true;

	return true;
}

bool DSVideo::OpenMovieHard(string csMovieName, void *pHWnd)
{ 	
	HRESULT hr;
	int bln;
	int i, res;
	
	IPin *pOutGB, *pInGB, *pInD, *pOutD, *pInVR, *pPin;
	PIN_INFO PinInfo;
	FILTER_INFO FilterInfo;
	IBaseFilter *pFilter = NULL;
	AM_MEDIA_TYPE mt;

	m_MovieName = csMovieName;

	m_log = "";

	if (m_Inited) 
	{
		hr = CleanUp();
	}

	hr = CoInitialize(NULL);

	hr = CoCreateInstance(CLSID_FilterGraph, NULL, CLSCTX_INPROC_SERVER, 
						IID_IGraphBuilder, (void **)&m_pGB);

	hr = m_pGB->RenderFile(StringToLPCWSTR(m_MovieName), NULL);
	if (hr == S_OK) 
	{
		m_log += "PASS: Видео файл " + m_MovieName + " был успешно отрендерен.\n";
	}
	else
	{
		if (hr != VFW_E_UNSUPPORTED_STREAM) 
		{
			m_log += "PASS: Видео файл " + m_MovieName + " был отрендерен c некоторыми ошибками.\n";
		}
		else
		{
			m_log += "FAIL: Формат файла " + m_MovieName + " не поддерживается.\n";
			CleanUp(); 
			return false; 
		}
	}

	hr = m_pGB->QueryInterface(IID_IMediaControl, (void **)&m_pMC);

	hr = m_pGB->FindFilterByName(L"Video Renderer", &m_pVideoRenderFilter);
	if (hr == S_OK) 
	{
		m_log += "PASS: Video Renderer был успешно найден.\n";
	}
	else
	{ 
		m_log += "FAIL: Video Renderer не был найден.\n";
		CleanUp(); 
		return false; 
	}

	hr = GetPin(m_pVideoRenderFilter, PINDIR_INPUT, &pInVR);
	if (hr == S_OK) 
	{
		m_log += "PASS: Входной пин Video Renderer'a был успешно найден.\n";
	}
	else
	{ 
		m_log += "FAIL: Входной пин Video Renderer'a не удалось найти.\n";
		CleanUp(); 
		return false; 
	}
	if (hr != S_OK) { CleanUp(); return false; }

	hr = pInVR->ConnectedTo(&pOutD);
	if (hr != S_OK) { CleanUp(); return false; }

	hr = pInVR->Disconnect();
	
	hr = pOutD->Disconnect();

	hr = CoCreateInstance(CLSID_SampleGrabber, NULL, CLSCTX_INPROC_SERVER,
							IID_IBaseFilter, (void**)&m_pSampleGrabberFilter);
	if (hr == S_OK) 
	{
		m_log += "PASS: CoCreateInstance(..SampleGrabberFilter..)\n";
	}
	else
	{
		m_log += "FAIL: CoCreateInstance(..SampleGrabberFilter..)\n";
		CleanUp(); 
		return false; 
	}

	hr = m_pGB->AddFilter(m_pSampleGrabberFilter, L"SampleGrabber");

	hr = m_pSampleGrabberFilter->QueryInterface(IID_ISampleGrabber, (void**)&m_pGrabber);
	if (hr == S_OK) 
	{
		m_log += "PASS: Sample Grabber Filter был успешно создан.\n";
	}
	else
	{ 
		m_log += "FAIL: Sample Grabber Filter не удалось создать.\n";
		CleanUp(); 
		return false; 
	}

	ZeroMemory(&mt, sizeof(AM_MEDIA_TYPE));
	mt.majortype = MEDIATYPE_Video;
	mt.subtype = MEDIASUBTYPE_RGB32;
	mt.bTemporalCompression = false;

	hr = m_pGrabber->SetMediaType(&mt);
	if (hr == S_OK) 
	{
		m_log += "PASS: В Sample Grabber Filter был успешно настроен AM_MEDIA_TYPE.\n";
	}
	else
	{ 
		m_log += "FAIL: В Sample Grabber Filter не удалось настроить AM_MEDIA_TYPE.\n";
		CleanUp(); 
		return false; 
	}

	bln = 1;
	hr = m_pGrabber->SetOneShot(FALSE);
	if (hr != S_OK) bln = 0; 
	hr = m_pGrabber->SetBufferSamples(TRUE);
	if (hr != S_OK) bln = 0; 
	hr = m_pGrabber->SetCallback(&m_SGCallback,1);
	if (hr != S_OK) bln = 0; 
	m_SGCallback.m_pVideo = this;
	m_SGCallback.m_ImageGeted = false;
	
	if (bln == 1)
	{
		m_log += "PASS: В Sample Grabber Filter были успешно настроены дополнительные настройки.\n";
	}
	else
	{
		m_log += "FAIL: В Sample Grabber Filter не удалось настроить дополнительные настройки.\n";
	}

	hr = GetPin(m_pSampleGrabberFilter, PINDIR_INPUT, &pInGB);
	if (hr == S_OK) 
	{
		m_log += "PASS: Входной пин Sample Grabber Filter'a был успешно найден.\n";
	}
	else
	{ 
		m_log += "FAIL: Входной пин Sample Grabber Filter'a не удалось найти.\n";
		CleanUp(); 
		return false; 
	}

	hr = GetPin(m_pSampleGrabberFilter, PINDIR_OUTPUT, &pOutGB);
	if (hr == S_OK) 
	{
		m_log += "PASS: Выходной пин Sample Grabber Filter'a был успешно найден.\n";
	}
	else
	{ 
		m_log += "FAIL: Выходной пин Sample Grabber Filter'a не удалось найти.\n";
		CleanUp(); 
		return false; 
	}

	hr = pOutD->QueryPinInfo(&PinInfo);

	pFilter = PinInfo.pFilter;

	hr = pFilter->QueryFilterInfo(&FilterInfo);

	bln = 0;
	if ( (string("XviD") != WCSToStr(FilterInfo.achName).substr(0, 4)) &&
		 (string("ffdshow") != WCSToStr(FilterInfo.achName).substr(0, 7)) )
	{
		if ((hr = m_pGB->ConnectDirect(pOutD, pInGB, NULL)) == S_OK)
		{
			bln = 1;
			m_log += string("PASS: ") + WCSToStr(FilterInfo.achName) + string(" был успешно соединен с Sample Grabber Filter.\n");
		}
		else
		{
			m_log += string("FAIL: ") + WCSToStr(FilterInfo.achName) + string(" не удалось соединить с Sample Grabber Filter.\n");
		}
	}
	if (bln == 0)
	{ 
		if ( (string("XviD") == WCSToStr(FilterInfo.achName).substr(0, 4)) ||
		 (string("ffdshow") == WCSToStr(FilterInfo.achName).substr(0, 7)) )
		{
			m_log += string("###: Обнаружена попытка соединения с не всегда корректно работающим фильтром ") + WCSToStr(FilterInfo.achName) + string("\n");
		}

		hr = GetPin(pFilter, PINDIR_INPUT, &pInD);

		hr = pInD->ConnectedTo(&pPin);

		hr = pInD->Disconnect();

		hr = pPin->Disconnect();

		hr = m_pGB->RemoveFilter(pFilter);
		res = pFilter->Release();

		if (hr != S_OK)
		{
			m_log += string("FAIL: Не удалось удалить ") + WCSToStr(FilterInfo.achName) + string("\n");
			CleanUp(); return false;
		}

		CLSID CLSID_DivX = {0x78766964, 0x0000, 0x0010, 0x80, 0x00, 0x00, 0xAA, 0x00, 0x38, 0x9B, 0x71};
		CLSID CLSID_ffdshow = {0x04FE9017, 0xF873, 0x410E, 0x87, 0x1E, 0xAB, 0x91 , 0x66, 0x1A, 0x4E, 0xF7};
		vector<CLSID> cls;
		vector<string> fnames;

		cls.push_back(CLSID_DivX);
		fnames.push_back("DivX Decoder Filter");

		cls.push_back(CLSID_AVIDec);
		fnames.push_back("AVI Decoder");

		cls.push_back(CLSID_ffdshow);
		fnames.push_back("ffdshow MPEG-4 Video Decoder");
		
		for (i=0; i < (int)cls.size(); i++)
		{
			hr = CoCreateInstance(cls[i], NULL, CLSCTX_INPROC_SERVER,
								IID_IBaseFilter, (void**)&m_pDecoder);

			if (hr == S_OK)	
			{
				m_log += string("PASS: Удалось создать фильтр ") + fnames[i] + "\n";
			}
			else
			{
				m_log += string("FAIL: Не удалось создать фильтр ") + fnames[i] + "\n";
				continue;
			}

			hr = m_pGB->AddFilter(m_pDecoder, L"Video Decoder");

			hr = GetPin(m_pDecoder, PINDIR_INPUT, &pInD);
		
			hr = GetPin(m_pDecoder, PINDIR_OUTPUT, &pOutD);
		
			hr = m_pGB->ConnectDirect(pPin, pInD, NULL);
			
			if (hr == S_OK)
			{
				m_log += "PASS: Старый фильтр был успешно заменен новым " + fnames[i] + "\n";
				break;
			}
			else
			{
				hr = m_pGB->RemoveFilter(m_pDecoder);
				res = m_pDecoder->Release();
				m_log += "FAIL: Не удалось заменить старый фильтр новым " + fnames[i] + "\n";
			}
		}

		if (hr != S_OK) 
		{
			m_log += "FAIL: Не удалось подобрать соответствующий декомпрессор\n";
			CleanUp(); 
			return false; 
		}

		hr = m_pGB->Connect(pOutD, pInGB);
		if (hr == S_OK) 
		{
			m_log += "PASS: " + fnames[i] + " был успешно соединен с Sample Grabber Filter.\n";
		}
		else
		{
			m_log += "FAIL: Не удалось соединить " + fnames[i] + " с Sample Grabber Filter.\n";
			CleanUp(); 
			return false; 
		}
	}

	if (hr == S_OK)	
	{
		hr = m_pGB->RemoveFilter(m_pVideoRenderFilter);
		res = m_pVideoRenderFilter->Release();
		
		m_pVideoRenderFilter = NULL;
		hr = CoCreateInstance(CLSID_VideoRenderer, NULL, CLSCTX_INPROC_SERVER,
								IID_IBaseFilter,(void**)&m_pVideoRenderFilter);

		hr = m_pGB->AddFilter(m_pVideoRenderFilter, L"Video Render");
		
		pInVR = NULL;
		hr = GetPin(m_pVideoRenderFilter, PINDIR_INPUT, &pInVR);
	}

	hr = m_pGB->ConnectDirect(pOutGB, pInVR, NULL);
	if (hr == S_OK) 
	{
		m_log += "PASS: Sample Grabber Filter был успешно соединен с дефолтовым Video Renderer.\n";
	}
	else
	{ 
		m_log += "FAIL: Sample Grabber Filter не удалось соединить с дефолтовым Video Renderer.\n";	
					
		hr = m_pGB->RemoveFilter(m_pVideoRenderFilter);
		res = m_pVideoRenderFilter->Release();
		m_pVideoRenderFilter = NULL;
		
		hr = m_pGB->Render(pOutGB);

		if (hr == S_OK) 
		{
			m_log += "PASS: Sample Grabber Filter был успешно отрендерен автоматически.\n";
		}
		else
		{ 
			m_log += "FAIL: Sample Grabber Filter не удалось отрендерить автоматически.\n";	
		}

		hr = m_pGB->FindFilterByName(L"Video Renderer", &m_pVideoRenderFilter);
		if (hr == S_OK) 
		{
			m_log += "PASS: Video Renderer был успешно найден.\n";
		}
		else
		{ 
			m_log += "FAIL: Video Renderer не был найден.\n";
		}
	}

	hr = m_pGB->QueryInterface(IID_IMediaEventEx, (void **)&m_pME);

	hr = m_pGB->QueryInterface(IID_IMediaSeeking,(void **)&m_pMS);

	hr = m_pGB->QueryInterface(IID_IMediaFilter, (void **)&m_pMF); 

	hr = m_pGB->QueryInterface(IID_IVideoWindow,(void **)&m_pVW);

	hr = m_pGB->QueryInterface(IID_IBasicVideo,(void **)&m_pBV);

	bln = 0;

	while(1)
	{
		//---------------------------------------------------------------
		// удаляем аудио рендерер и аудио декомпресор
		IBaseFilter *pSR, *pSD;

		hr = m_pGB->FindFilterByName(L"Default DirectSound Device", &pSR);
		if (hr != S_OK) break;

		pPin = NULL;
		hr = GetPin(pSR, PINDIR_INPUT, &pPin);
		if (hr != S_OK) break;

		hr = pPin->ConnectedTo(&pPin);
		if (hr != S_OK) break;

		hr = pPin->QueryPinInfo(&PinInfo);
		pSD = PinInfo.pFilter;

		hr = m_pGB->RemoveFilter(pSR);
		if (hr != S_OK) break;

		hr = m_pGB->RemoveFilter(pSD);
		if (hr != S_OK) break;
		//---------------------------------------------------------------

		bln = 1;
		break;
	}

	if (bln == 0)
	{
		hr = m_pGB->QueryInterface(IID_IBasicAudio,(void **)&m_pBA);

		hr = m_pBA->put_Volume(-10000);
	}

	hr = m_pMF->SetSyncSource(NULL);

	hr = m_pVW->put_Owner(*((OAHWND*)pHWnd));

	hr = m_pVW->put_WindowStyle(WS_CHILD | WS_CLIPSIBLINGS);
	
	hr = m_pVW->put_MessageDrain(*((OAHWND*)pHWnd));

	hr = m_pBV->GetVideoSize(&m_Width,&m_Height);

	hr = m_pMS->GetStopPosition(&m_Duration);
	
	m_Inited = true;

	return true;
}

bool DSVideo::SetVideoWindowPlacement(void *pHWnd)
{
	HRESULT hr;
	bool result = false;
	CLSID classID;
	IPin *pOut, *pIn;

	while(1)
	{
		hr = m_pVideoRenderFilter->GetClassID(&classID);
		if (hr != S_OK) break;

		pIn = NULL;
		hr = GetPin(m_pVideoRenderFilter, PINDIR_INPUT, &pIn);
		if (hr != S_OK) break;
	
		pOut = NULL;
		hr = pIn->ConnectedTo(&pOut);
		if (hr != S_OK) break;

		hr = m_pVW->put_Visible(false);

		hr = m_pVW->put_MessageDrain(NULL);
	
		hr = m_pVW->put_Owner(NULL);

		m_pVW->Release();	
		m_pVW = NULL;

		hr = pIn->Disconnect();
	
		hr = pOut->Disconnect();

		hr = m_pGB->RemoveFilter(m_pVideoRenderFilter);
		if (hr != S_OK) break;

		m_pVideoRenderFilter->Release();
		
		m_pVideoRenderFilter = NULL;
		hr = CoCreateInstance(classID, NULL, CLSCTX_INPROC_SERVER,
								IID_IBaseFilter,(void**)&m_pVideoRenderFilter);
		if (hr != S_OK) break;

		hr = m_pGB->AddFilter(m_pVideoRenderFilter, L"Video Render");
		if (hr != S_OK) break;
		
		pIn = NULL;
		hr = GetPin(m_pVideoRenderFilter, PINDIR_INPUT, &pIn);
		if (hr != S_OK) break;

		hr = m_pGB->ConnectDirect(pOut, pIn, NULL);
		if (hr != S_OK) break;

		break;
	}
	
	hr = m_pGB->QueryInterface(IID_IVideoWindow,(void **)&m_pVW);

	hr = m_pVW->put_Visible(true);

	hr = m_pVW->put_Owner(*((OAHWND*)pHWnd));

	hr = m_pVW->put_WindowStyle(WS_CHILD | WS_CLIPSIBLINGS);
	
	hr = m_pVW->put_MessageDrain(*((OAHWND*)pHWnd));

	if (hr == S_OK) result = true;

	return result;
}

IBaseFilter* DSVideo::GetDecoder()
{
	HRESULT hr;
	IPin *pOut, *pIn;
	PIN_INFO PinInfo;
	IBaseFilter* pFilter = NULL;

	while(1)
	{
		if (m_pSampleGrabberFilter == NULL) break;

		pIn = NULL;
		hr = GetPin(m_pSampleGrabberFilter, PINDIR_INPUT, &pIn);
		if (hr != S_OK) break;

		pOut = NULL;
		hr = pIn->ConnectedTo(&pOut);
		if (hr != S_OK) break;

		hr = pOut->QueryPinInfo(&PinInfo);
		if (hr != S_OK) break;

		pFilter = PinInfo.pFilter;

		break;
	}

	return pFilter;
}

IBaseFilter* DSVideo::GetSourceFilter()
{
	HRESULT hr;
	IPin *pOut, *pIn;
	PIN_INFO PinInfo;
	IBaseFilter* pFilter = NULL;

	while(1)
	{
		if (m_pSampleGrabberFilter == NULL) break;

		pIn = NULL;
		hr = GetPin(m_pSampleGrabberFilter, PINDIR_INPUT, &pIn);
		
		while (hr == S_OK)
		{
			pOut = NULL;
			hr = pIn->ConnectedTo(&pOut);
			if (hr != S_OK) { pFilter = NULL; break; }

			hr = pOut->QueryPinInfo(&PinInfo);
			if (hr != S_OK) { pFilter = NULL; break; }

			pFilter = PinInfo.pFilter;

			pIn = NULL;
			hr = GetPin(pFilter, PINDIR_INPUT, &pIn);
		}

		break;
	}

	return pFilter;
}

/////////////////////////////////////////////////////////////////////////////

bool DSVideo::CloseMovie()
{
	return true;
}

/////////////////////////////////////////////////////////////////////////////

bool DSVideo::SetNullRender()
{
	HRESULT hr;

	IPin *pOutGB, *pInVR;

	if (m_Inited == false) return false;
	
	//this->Stop();
	
	hr = m_pVW->put_Visible(false);

	hr = m_pVW->put_MessageDrain(NULL);
	
	hr = m_pVW->put_Owner(NULL);

	hr = m_pGB->RemoveFilter(m_pVideoRenderFilter);
	if (hr != S_OK) { CleanUp(); return false; }

	hr = GetPin(m_pSampleGrabberFilter, PINDIR_OUTPUT, &pOutGB);
	if (hr != S_OK) { CleanUp(); return false; }

	m_pVideoRenderFilter = NULL;
	hr = CoCreateInstance(CLSID_NullRenderer, NULL, CLSCTX_INPROC_SERVER,
							IID_IBaseFilter,(void**)&m_pVideoRenderFilter);

	hr = m_pGB->AddFilter(m_pVideoRenderFilter, L"Video Render");

	hr = GetPin(m_pVideoRenderFilter, PINDIR_INPUT, &pInVR);
	if (hr != S_OK) { CleanUp(); return false; }

	hr = pInVR->Disconnect();

	hr = pOutGB->Disconnect();

	hr = m_pGB->Connect(pOutGB, pInVR);
	if (hr != S_OK) { CleanUp(); return false; }

	hr = m_pMF->SetSyncSource(NULL);

	m_IsMSSuported = false;

	return true;
}

/////////////////////////////////////////////////////////////////////////////

void DSVideo::SetPos(s64 Pos)
{
	HRESULT hr;
	s64 endPos;
	long evCode;
	
	endPos = m_Duration;

	m_pMS->SetPositions(&Pos,AM_SEEKING_AbsolutePositioning,&endPos,AM_SEEKING_AbsolutePositioning);
	m_SGCallback.m_st = Pos;  

	m_SGCallback.m_ImageGeted = false;
	hr = m_pMC->Run();
	hr = m_pME->WaitForCompletion(300, &evCode);

	if (m_SGCallback.m_ImageGeted != true)
	{
		m_pMC->Pause();
	}
}

/////////////////////////////////////////////////////////////////////////////

void DSVideo::SetPos(double pos)
{
	HRESULT hr;
	s64 Pos, endPos;
	long evCode;

	endPos = m_Duration;
	Pos = (s64)(pos*10000000.0);

	m_pMS->SetPositions(&Pos,AM_SEEKING_AbsolutePositioning,&endPos,AM_SEEKING_AbsolutePositioning);
	m_SGCallback.m_st = Pos;  

	m_SGCallback.m_ImageGeted = false;
	hr = m_pMC->Run();
	hr = m_pME->WaitForCompletion(300, &evCode);

	if (m_SGCallback.m_ImageGeted != true)
	{
		m_pMC->Pause();
	}
}

/////////////////////////////////////////////////////////////////////////////

void DSVideo::SetPosFast(s64 Pos)
{
	HRESULT hr;
	s64 endPos;
	long evCode;
	
	endPos = m_Duration;
	hr = m_pMS->SetPositions(&Pos,AM_SEEKING_AbsolutePositioning,&endPos,AM_SEEKING_AbsolutePositioning);
	m_SGCallback.m_st = Pos;  

	hr = m_pME->WaitForCompletion(300, &evCode);
	
}

/////////////////////////////////////////////////////////////////////////////

void DSVideo::Stop()
{
	if(m_pMC != NULL)
	{
		HRESULT hr = m_pMC->Pause();
		this->SetPos((s64)0);
		m_pMC->Stop();
	}
}

/////////////////////////////////////////////////////////////////////////////

void DSVideo::StopFast()
{
	this->SetPos((s64)0);
	m_pMC->Stop();
}

/////////////////////////////////////////////////////////////////////////////

HRESULT DSVideo::CleanUp()
{
	HRESULT hr = S_OK;
	string log;
	int i, max_n = 100;
	IPin *pOut, *pIn;
	fstream fout;
	string fname;
	
	fname = m_Dir + string("/clean_video.log");
	fout.open(fname.c_str(), ios_base::out | ios_base::app);
	fout <<	"";
	fout.close();

	this->Stop();

	log += "PASS: m_pME->WaitForCompletion(...)\n";
	
	if (m_pVW != NULL)
	{
		hr = m_pVW->put_Visible(OAFALSE);
		hr = m_pVW->put_MessageDrain(NULL);
		hr = m_pVW->put_Owner(NULL);
	}

	m_pDecoder = GetDecoder();

	if (m_pDecoder != NULL)	log += "PASS: GetDecoder()\n";
	else log += "FAIL: GetDecoder()\n";

	if (m_pVideoRenderFilter != NULL)
	{
		pOut = NULL;
		hr = GetPin(m_pVideoRenderFilter, PINDIR_OUTPUT, &pOut);
		pIn = NULL;
		hr = GetPin(m_pVideoRenderFilter, PINDIR_INPUT, &pIn);

		if (pOut != NULL) hr = pOut->Disconnect();
		if (pIn != NULL) hr = pIn->Disconnect();

		hr = m_pGB->RemoveFilter(m_pVideoRenderFilter);

		if (hr == S_OK) log += "PASS: Delete VideoRenderFilter\n";
		else log += "FAIL: Delete VideoRenderFilter\n";
	}

	if (m_pSampleGrabberFilter != NULL)
	{
		hr = m_pGB->RemoveFilter(m_pSampleGrabberFilter);
		
		if (hr == S_OK) log += "PASS: Delete SampleGrabberFilter\n";
		else log += "FAIL: Delete SampleGrabberFilter\n";
	}

	if (m_pDecoder != NULL)
	{
		hr = m_pGB->RemoveFilter(m_pDecoder);

		if (hr == S_OK) log += "PASS: Delete Decoder\n";
		else log += "FAIL: Delete Decoder\n";
	}

	if(m_pMC != NULL) i = m_pMC->Release(); 
	m_pMC = NULL;

	if(m_pME != NULL) i = m_pME->Release();
	m_pME = NULL;

	if(m_pMS != NULL) i = m_pMS->Release();
	m_pMS = NULL;

	if(m_pMF != NULL) i = m_pMF->Release(); 
	m_pMF = NULL;

	if(m_pVW != NULL) i = m_pVW->Release();	
	m_pVW = NULL;

	if(m_pBV != NULL) i = m_pBV->Release();
	m_pBV = NULL;

	if(m_pBA != NULL) i = m_pBA->Release();
	m_pBA = NULL;

	if(m_pDecoder != NULL) i = m_pDecoder->Release();
	m_pDecoder = NULL;

	if(m_pGrabber != NULL) i = m_pGrabber->Release();
	m_pGrabber = NULL;

	if(m_pSampleGrabberFilter != NULL) i = m_pSampleGrabberFilter->Release(); 	
	m_pSampleGrabberFilter = NULL;

	if(m_pSourceFilter != NULL) i = m_pSourceFilter->Release();
	m_pSourceFilter = NULL;

	if(m_pVideoRenderFilter != NULL) i = m_pVideoRenderFilter->Release();
	m_pVideoRenderFilter = NULL;

	/*if(m_pGB != NULL)
	{
		n = 0;

		IEnumFilters *pEnum = NULL;
		hr = m_pGB->EnumFilters(&pEnum);
		if (SUCCEEDED(hr))
		{			
			IBaseFilter *pFilter = NULL;
			while (S_OK == pEnum->Next(1, &pFilter, NULL))
			{
				n++;
				hr = m_pGB->RemoveFilter(pFilter);
				hr = pEnum->Reset();
				hr = pFilter->Release();
				pFilter = NULL;

				if (n > max_n) 
				{
					break;
				}
			}
			hr = pEnum->Release();
		}
	}

	if (n <=max_n) log += "PASS: Delete All Filters n:=" + IntToCStr(n) + "\n";
	else log += "FAIL: Delete All Filters\n";*/

	if(m_pGB != NULL) i = m_pGB->Release();
	m_pGB = NULL;

	if(m_pBuilder != NULL) i = m_pBuilder->Release();
	m_pBuilder = NULL;

	CoUninitialize();
	
	if (m_pBuffer != NULL) delete[] m_pBuffer;
	m_pBuffer = NULL;

	m_Inited = false;

	m_IsMSSuported = true;
	m_SGCallback.m_st = 0;

	fname = m_Dir + string("\\clean_video.log");
	fout.open(fname.c_str(), ios::out);
	fout <<	log;
	fout.close();

	return hr;
}

/////////////////////////////////////////////////////////////////////////////

void DSVideo::OneStep()
{
	long evCode;

	m_SGCallback.m_ImageGeted = false;
	m_pMC->Run();
	m_pME->WaitForCompletion(300, &evCode);

	if (m_SGCallback.m_ImageGeted != true)
	{
		m_pMC->Pause();
	}
}

/////////////////////////////////////////////////////////////////////////////

s64 DSVideo::OneStepWithTimeout()
{
	s64 CurPos, PrevPos, tmpPos;
	clock_t start_t, dt = 10000;
	long evCode;
	long min_frame_rate = 15;
	long ddt = (s64)120*(s64)10000;

	PrevPos = this->GetPos();

	if (PrevPos == m_Duration)
	{
		CurPos = PrevPos;
	}
	else
	{
		m_SGCallback.m_ImageGeted = false;
		m_pMC->Run();
		m_pME->WaitForCompletion(500, &evCode);
		m_pMC->Pause();

		if ( (m_SGCallback.m_ImageGeted == false) && 
			  (PrevPos >= (m_Duration - ddt)) )
		{
			m_SGCallback.m_ImageGeted = true;
			CurPos = m_Duration;
		}
		else
		{
			if ( (m_SGCallback.m_ImageGeted == false) && 
				(PrevPos < (m_Duration - ddt*2)) )
			{
				start_t = clock();

				while ( ((clock() - start_t) < dt) && 
						(m_SGCallback.m_ImageGeted == false) )
				{
					this->SetPos(PrevPos);

					m_pMC->Run();
					m_pME->WaitForCompletion(1000, &evCode);
					m_pMC->Pause();
				}

				if (m_SGCallback.m_ImageGeted == false)
				{
					this->SetPos(PrevPos + ddt/3);
				}

				if (m_SGCallback.m_ImageGeted == false)
				{
					this->SetPos(PrevPos + ddt/2);
				}

				if (m_SGCallback.m_ImageGeted == false)
				{
					this->SetPos(PrevPos + (ddt*2)/3);
				}

				if (m_SGCallback.m_ImageGeted == false)
				{
					this->SetPos(PrevPos + ddt);
				}

				if (m_SGCallback.m_ImageGeted == false)
				{
					this->SetPos(PrevPos + (ddt*3)/2);
				}

				if (m_SGCallback.m_ImageGeted == false)
				{
					this->SetPos(PrevPos + ddt*2);
				}

				if (m_SGCallback.m_ImageGeted == false)
				{
					MessageBox(NULL, "Can'nt get new image.", "OneStepWithTimeout in DSVideo", MB_ICONERROR);
				}
			}
			
			CurPos = tmpPos = this->GetPos();

			if ( (tmpPos == PrevPos) || ((tmpPos - PrevPos) > ddt) )
			{
				this->SetPos(PrevPos + ddt/3);
				CurPos = this->GetPos();

				if (CurPos == PrevPos)
				{
					this->SetPos(PrevPos + ddt/2);
					CurPos = this->GetPos();
				}

				if (CurPos == PrevPos)
				{
					this->SetPos(PrevPos + (ddt*2)/3);
					CurPos = this->GetPos();
				}

				if (CurPos == PrevPos)
				{
					this->SetPos(PrevPos + ddt);
					CurPos = this->GetPos();
				}

				if (CurPos == PrevPos)
				{
					this->SetPos(PrevPos + (ddt*3)/2);
					CurPos = this->GetPos();
				}

				if (CurPos == PrevPos)
				{
					this->SetPos(PrevPos + ddt*2);
					CurPos = this->GetPos();
				}

				if ((CurPos == PrevPos) && (tmpPos == PrevPos))
				{
					MessageBox(NULL, "Can'nt get image with pos != prev_pos.", "OneStepWithTimeout in DSVideo", MB_ICONERROR);
				}
			}

			if (CurPos == PrevPos)
			{
				this->SetPos(tmpPos);
				CurPos = this->GetPos();

				if (CurPos == PrevPos)
				{
					CurPos = tmpPos;
				}
			}
		}
	}

	return CurPos;
}

/////////////////////////////////////////////////////////////////////////////

void DSVideo::ErrorMessage(string str)
{
	MessageBox(NULL, str.c_str(), "ERROR MESSAGE", MB_ICONERROR);
}

/////////////////////////////////////////////////////////////////////////////

s64 DSVideo::GetPos()
{
    s64 pos = -1;

	if ( m_IsMSSuported )
	{
		HRESULT hr = m_pMS->GetCurrentPosition(&pos);

		if (hr == E_NOTIMPL)
		{
			pos = m_SGCallback.m_st;
			m_IsMSSuported = false;
		}
	}
	else
	{
		pos = m_SGCallback.m_st;
	}

    return pos;
}

/////////////////////////////////////////////////////////////////////////////

void DSVideo::GetRGBImage(int *ImRGB, int xmin, int xmax, int ymin, int ymax)
{
	int w, h, x, y, i, j, wh, di, dj;
	long BufferSize;
	int *pBuffer;
	
    w = xmax-xmin+1;
	h = ymax-ymin+1;

	di = m_Width-w;
	dj = -2*w;
	wh = w*h;
	BufferSize = m_Width*m_Height*sizeof(int);

	if (m_pBuffer == NULL)
	{
		m_pBuffer = new int[m_Width*m_Height];
	}

	pBuffer = m_pBuffer;

	HRESULT hr = m_pGrabber->GetCurrentBuffer(&BufferSize,(long*)(pBuffer));
	
	i = (m_Height-1-ymax)*m_Width + xmin;
	j = (h-1)*w;

	for(y=0; y<h; y++)
	{
		for(x=0; x<w; x++)
		{
			ImRGB[j] = pBuffer[i];
			i++;
			j++;
		}
		i += di;
		j += dj; 
	}
}

/////////////////////////////////////////////////////////////////////////////

void DSVideo::SetImageGeted(bool ImageGeted)
{
	m_SGCallback.m_ImageGeted = ImageGeted;
}

/////////////////////////////////////////////////////////////////////////////

void DSVideo::RunWithTimeout(s64 timeout)
{
	long evCode;

	m_pMC->Run();
	m_pME->WaitForCompletion((long)timeout, &evCode);
}

/////////////////////////////////////////////////////////////////////////////

void DSVideo::Run()
{
	HRESULT hr;

	hr = m_pMC->Run();
}

/////////////////////////////////////////////////////////////////////////////

void DSVideo::Pause()
{
	m_pMC->Pause();
}

/////////////////////////////////////////////////////////////////////////////

void DSVideo::SetVideoWindowPosition(int left, int top, int width, int height)
{
	m_pVW->SetWindowPosition(left, top, width, height);
}

s64 DSVideo::PosToMilliSeconds(s64 pos)
{
	return (pos/(s64)10000);
}
