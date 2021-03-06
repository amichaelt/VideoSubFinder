                              //MyVideo.cpp//                                
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

#include "DSVideo.h"
#include <Dvdmedia.h>
#include <QtCore/QtGlobal>
#include <time.h>

static CLSID CLSID_Trans032 = {0x1916c5c7, 0x2aa, 0x415f, 0x89, 0xf, 0x76, 0xd9, 0x4c, 0x85, 0xaa, 0xf1};

static CLSID CLSID_DivX = {0x78766964, 0x0000, 0x0010, 0x80, 0x00, 0x00, 0xAA, 0x00, 0x38, 0x9B, 0x71};
static CLSID CLSID_ffdshow = {0x04FE9017, 0xF873, 0x410E, 0x87, 0x1E, 0xAB, 0x91 , 0x66, 0x1A, 0x4E, 0xF7};

/////////////////////////////////////////////////////////////////////////////

std::string WCSToStr(WCHAR *wstr)
{
    static char str[100];

    wcstombs( str, wstr, 100 );

    return std::string(str);
}

/////////////////////////////////////////////////////////////////////////////

LPCWSTR StringToLPCWSTR(std::string csStr)
{
    static WCHAR wName[500]; 

    int len = MultiByteToWideChar( CP_ACP, 0, csStr.c_str(), -1, 0, 0);

    MultiByteToWideChar( CP_ACP, 0, csStr.c_str(), -1, wName, len);

    wName[len] = L'\0';

    return wName;
}

/////////////////////////////////////////////////////////////////////////////

std::string IntToCStr(int n)
{
    char str[100];

    itoa(n, str, 10);

    return std::string(str);
}

/////////////////////////////////////////////////////////////////////////////

CTrans032::CTrans032( int **inBuffer, qint64 *inStartTime,
                            bool *inGotImage, IMediaControl *inMediaControl,
                            bool *inIs0RenderSet, LPUNKNOWN punk, HRESULT *phr )
    : CTransInPlaceFilter(TEXT("Trans032"), punk, CLSID_Trans032, phr)
{
    videoBuffer = inBuffer;
    startTime = inStartTime;
    gotImage = inGotImage;
    mediaControl = inMediaControl;
    is0RenderSet = inIs0RenderSet;

    *gotImage = true;
    tryingToGetImage = false;
    *startTime = 0;

    videoWidth = 0;
    videoHeight = 0;
 
    videoFormatType = 1; //format type

    reInitialize = true;
}

/////////////////////////////////////////////////////////////////////////////

STDMETHODIMP CTrans032::NonDelegatingQueryInterface(REFIID riid, void **ppv)
{
    return CBaseFilter::NonDelegatingQueryInterface(riid,ppv);
}

/////////////////////////////////////////////////////////////////////////////

HRESULT CTrans032::CheckInputType(const CMediaType *mtIn)
{
    CheckPointer(mtIn,E_POINTER);

    if( *mtIn->Type( ) != MEDIATYPE_Video )
    {
        return E_INVALIDARG;
    }

    if ( (*mtIn->FormatType() != FORMAT_VideoInfo) &&
         (*mtIn->FormatType() != FORMAT_VideoInfo2) )
    {
        return E_INVALIDARG;
    }

    if( *mtIn->Subtype( ) != MEDIASUBTYPE_RGB32 )
    {
        return E_INVALIDARG;
    }

    return NOERROR;
}

/////////////////////////////////////////////////////////////////////////////

HRESULT CTrans032::Transform(IMediaSample *pSample)
{
    HRESULT hr;
    VIDEOINFOHEADER  *pVi1;
    VIDEOINFOHEADER2 *pVi2;
    qint64 StartTime, StopTime;
    BYTE *pBuffer;
    int *pIntBuffer;
    long BufferLen;
    int w, h, x, y, i, j, dj;
    AM_MEDIA_TYPE mt, *pmt;
    IPin *pIn = 0;

    if (!(*is0RenderSet) || !(*gotImage))
    {    
        hr = pSample->GetPointer(&pBuffer);
        BufferLen = pSample->GetSize();
        pIntBuffer = (int*)pBuffer;

        if (reInitialize == true)
        {
            hr = m_pInput->ConnectedTo(&pIn);
            hr = pIn->ConnectionMediaType(&mt);
            pmt = &mt;
            
            if (hr == S_OK)
            {
                if (pmt->formattype == FORMAT_VideoInfo2)
                {
                    videoFormatType = 2;
                    pVi2 = (VIDEOINFOHEADER2*)pmt->pbFormat;
                    videoWidth = pVi2->bmiHeader.biWidth;
                    videoHeight = pVi2->bmiHeader.biHeight;
                }
                else
                {
                    videoFormatType = 1;
                    pVi1 = (VIDEOINFOHEADER*)pmt->pbFormat;
                    videoWidth = pVi1->bmiHeader.biWidth;
                    videoHeight = pVi1->bmiHeader.biHeight;
                }
            }            

            if (*videoBuffer == 0)
            {
                *videoBuffer = new int[BufferLen/sizeof(int)];
            }

            reInitialize = false;
        }
                

        if (videoHeight < 0)
        {
            memcpy(*videoBuffer, pBuffer, BufferLen);
        }
        else
        {
            w = videoWidth;
            h = videoHeight;

            dj = -2*w;

            i = 0;
            j = (h-1)*w;

            for(y=0; y<h; y++)
            {
                for(x=0; x<w; x++)
                {
                    (*videoBuffer)[j] = pIntBuffer[i];
                    i++;
                    j++;
                }                
                j += dj; 
            }
        }

        hr = pSample->GetTime(&StartTime, &StopTime);
        StartTime += m_pInput->CurrentStartTime();        
        *startTime = StartTime;

        if (!(*gotImage))
        {
            *gotImage = true;
            mediaControl->Pause();
        }
    }

    return NOERROR;
}

/////////////////////////////////////////////////////////////////////////////

MySampleGrabberCallback::MySampleGrabberCallback( int **ppBuffer, qint64 *pST, 
                                                  bool *pImageGeted, DSVideo *pVideo,
                                                  bool *pIsSet0Render )
{
    m_ppBuffer = ppBuffer;
    m_pST = pST;
    m_pImageGeted = pImageGeted;
    m_pVideo = pVideo;
    m_pIsSet0Render = pIsSet0Render;

    *m_pImageGeted = true;
    *m_pST = 0;
}

/////////////////////////////////////////////////////////////////////////////

HRESULT STDMETHODCALLTYPE MySampleGrabberCallback::BufferCB(double SampleTime, BYTE *pBuffer, long BufferLen)
{
    int w, h, x, y, i, j, dj;
    int *pIntBuffer = (int*)pBuffer;

    if ( (*m_pIsSet0Render == false) || (*m_pImageGeted == false) )
    {
        if (*m_ppBuffer == 0)
        {
            *m_ppBuffer = new int[BufferLen/sizeof(int)];
        }

        w = m_pVideo->m_Width;
        h = m_pVideo->m_Height;

        dj = -2*w;

        i = 0;
        j = (h-1)*w;

        for(y=0; y<h; y++)
        {
            for(x=0; x<w; x++)
            {
                (*m_ppBuffer)[j] = pIntBuffer[i];
                i++;
                j++;
            }                
            j += dj; 
        }

        *m_pST = (qint64)(SampleTime*10000000.0);

        if (*m_pImageGeted == false)
        {
            *m_pImageGeted = true;

            m_pVideo->m_pMC->Pause();
        }
    }

    return S_OK;
}

/////////////////////////////////////////////////////////////////////////////

HRESULT DSVideo::GetPin(IBaseFilter *pFilter, PIN_DIRECTION PinDir, IPin **ppPin)
{
    if (!pFilter || !ppPin) return E_POINTER;
    *ppPin = 0;

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

    IPin *pOut = 0, *pIn = 0;
    
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
    m_pGB=0;
    m_pMC=0;
    m_pME=0;
    m_pMS=0;
    m_pVW=0;
    m_pBV=0;
    m_pBA=0;
    m_pMF=0;
    m_pDecoder=0;
    m_pGrabber=0;
    m_pSourceFilter=0;
    m_pSampleGrabberFilter=0;
    m_pTrans032Filter=0;
    m_pVideoRenderFilter=0;
    m_pBuilder=0;
    m_pSGCallback=0;
    m_IsMSSuported = true;
    m_IsSet0Render = false;
    
    m_pBuffer=0;
    m_st = 0;

    m_type = 0;
}

/////////////////////////////////////////////////////////////////////////////

DSVideo::~DSVideo()
{
    if (m_Inited) CleanUp();
}

bool DSVideo::OpenMovieAllDefault(std::string csMovieName, void *pHWnd)
{     
    HRESULT hr;
    std::string Str;
    std::vector<CLSID> cls;
    std::vector<std::string> fnames;
    ULONG res;
    int i;
    
    m_type = 1;

    m_MovieName = csMovieName;

    if (m_Inited) 
    {
        hr = CleanUp();
    }

    hr = CoInitialize(0);

    hr = CoCreateInstance(CLSID_FilterGraph, 0, CLSCTX_INPROC_SERVER, 
                            IID_IGraphBuilder, (void **)&m_pGB);

    hr = CoCreateInstance(CLSID_CaptureGraphBuilder2, 0,
                            CLSCTX_INPROC_SERVER, IID_ICaptureGraphBuilder2, 
                            (void **)&m_pBuilder);
    
    hr = m_pBuilder->SetFiltergraph(m_pGB);

    hr = CoCreateInstance(CLSID_SampleGrabber, 0, CLSCTX_INPROC_SERVER,
                            IID_IBaseFilter, (void**)&m_pSampleGrabberFilter);

    hr = CoCreateInstance(CLSID_VideoMixingRenderer, 0, CLSCTX_INPROC_SERVER,
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

    try    
    {
        hr = m_pBuilder->RenderStream(0, 0, m_pSourceFilter, 0, m_pSampleGrabberFilter);
    }
    catch(...) 
    {
        hr = E_FAIL;
    }
    if (SUCCEEDED(hr))
    {
        hr = m_pBuilder->RenderStream(0, 0, m_pSampleGrabberFilter, 0, m_pVideoRenderFilter);
    }

    if (FAILED(hr))
    {        
        cls.push_back(CLSID_AVIDec);
        fnames.push_back("AVI Decoder");

        cls.push_back(CLSID_ffdshow);
        fnames.push_back("ffdshow MPEG-4 Video Decoder");

        cls.push_back(CLSID_DivX);
        fnames.push_back("DivX Decoder Filter");
        
        for (i=0; i < (int)cls.size(); i++)
        {
            hr = CoCreateInstance(cls[i], 0, CLSCTX_INPROC_SERVER,
                                IID_IBaseFilter, (void**)&m_pDecoder);

            hr = m_pGB->AddFilter(m_pDecoder, L"Video Decoder");

            try    
            {
                hr = ConnectFilters(m_pGB, m_pSourceFilter, m_pDecoder);
            }
            catch(...) 
            {
                hr = E_FAIL;
            }            

            if (SUCCEEDED(hr))
            {
                hr = ConnectFilters(m_pGB, m_pDecoder, m_pSampleGrabberFilter);
            }
            
            if (FAILED(hr))
            {
                hr = m_pGB->RemoveFilter(m_pDecoder);
                res = m_pDecoder->Release();
                m_pDecoder = 0;
                hr = E_FAIL;
            }
            else
            {
                break;
            }
        }

        if (FAILED(hr)) 
        { 
            MessageBox(0, "[DSHOW] Can't render stream SourceFilter to SampleGrabberFilter.", "ERROR MESSAGE", MB_ICONERROR); 
            CleanUp(); 
            return false; 
        }

        if (SUCCEEDED(hr))
        {
            hr = m_pBuilder->RenderStream(0, 0, m_pSampleGrabberFilter, 0, m_pVideoRenderFilter);
        }

        if (FAILED(hr)) 
        { 
            MessageBox(0, "[DSHOW] Can't render stream SampleGrabberFilter to VideoRenderFilter.", "ERROR MESSAGE", MB_ICONERROR); 
            CleanUp(); 
            return false; 
        }
    }

    hr = m_pGrabber->SetOneShot(FALSE);
    hr = m_pGrabber->SetBufferSamples(TRUE);
    m_pSGCallback = new MySampleGrabberCallback(&m_pBuffer, &m_st, 
                                                &m_ImageGeted, this,
                                                &m_IsSet0Render); 
    hr = m_pGrabber->SetCallback(m_pSGCallback,1);
    
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

/////////////////////////////////////////////////////////////////////////////

bool DSVideo::OpenMovieNormally(std::string csMovieName, void *pHWnd)
{     
    HRESULT hr;
    std::string Str;
    std::vector<CLSID> cls;
    std::vector<std::string> fnames;
    AM_MEDIA_TYPE mt;    
    IPin *pIn;
    ULONG res;
    int i;
    
    if (m_type == -3)
    {
        m_type = 3;
    }
    else
    {
        m_type = 2;
    }

    m_MovieName = csMovieName;

    if (m_Inited) 
    {
        hr = CleanUp();
    }

    hr = CoInitialize(0);

    hr = CoCreateInstance(CLSID_FilterGraph, 0, CLSCTX_INPROC_SERVER, 
                            IID_IGraphBuilder, (void **)&m_pGB);
    if (FAILED(hr)) { CleanUp(); return false; }

    hr = CoCreateInstance(CLSID_CaptureGraphBuilder2, 0,
                            CLSCTX_INPROC_SERVER, IID_ICaptureGraphBuilder2, 
                            (void **)&m_pBuilder);
    if (FAILED(hr)) { CleanUp(); return false; }
    
    hr = m_pBuilder->SetFiltergraph(m_pGB);
    if (FAILED(hr)) { CleanUp(); return false; }

    hr = m_pGB->QueryInterface(IID_IMediaControl, (void **)&m_pMC);
    if (FAILED(hr)) { CleanUp(); return false; }

    hr = m_pGB->QueryInterface(IID_IMediaEventEx, (void **)&m_pME);
    if (FAILED(hr)) { CleanUp(); return false; }

    hr = m_pGB->QueryInterface(IID_IMediaSeeking,(void **)&m_pMS);
    if (FAILED(hr)) { CleanUp(); return false; }
    
    hr = CoCreateInstance(CLSID_VideoMixingRenderer, 0, CLSCTX_INPROC_SERVER,
                            IID_IBaseFilter,(void**)&m_pVideoRenderFilter);
    if (FAILED(hr)) { CleanUp(); return false; }

    m_pTrans032 = new CTrans032(&m_pBuffer, &m_st, &m_ImageGeted, m_pMC, &m_IsSet0Render, 0, &hr); 
    if (FAILED(hr)) { CleanUp(); return false; }
       
    hr = m_pTrans032->QueryInterface(IID_IBaseFilter, 
                            reinterpret_cast<void**>(&m_pTrans032Filter));
    if (FAILED(hr)) { CleanUp(); return false; }
   
    hr = m_pGB->AddSourceFilter(StringToLPCWSTR(m_MovieName), L"Source1", &m_pSourceFilter);
    if (FAILED(hr)) { CleanUp(); return false; }

    hr = m_pGB->AddFilter(m_pTrans032Filter, L"MyColorSpaceConverter");
    if (FAILED(hr)) { CleanUp(); return false; }

    hr = m_pGB->AddFilter(m_pVideoRenderFilter, L"Video Renderer");
    if (FAILED(hr)) { CleanUp(); return false; }    

    try    
    {
        hr = m_pBuilder->RenderStream(0, 0, m_pSourceFilter, m_pTrans032Filter, m_pVideoRenderFilter);
    }
    catch(...) 
    {
        hr = E_FAIL;
    }

    if (FAILED(hr))
    {
        try    
        {
            hr = m_pBuilder->RenderStream(0, 0, m_pSourceFilter, 0, m_pTrans032Filter);
        }
        catch(...) 
        {
            hr = E_FAIL;
        }

        if (SUCCEEDED(hr))
        {
            try    
            {
                hr = m_pBuilder->RenderStream(0, 0, m_pTrans032Filter, 0, m_pVideoRenderFilter);
            }
            catch(...) 
            {
                hr = E_FAIL;
            }            
        }
    }    

    if (FAILED(hr))
    {        
        cls.push_back(CLSID_AVIDec);
        fnames.push_back("AVI Decoder");

        cls.push_back(CLSID_ffdshow);
        fnames.push_back("ffdshow MPEG-4 Video Decoder");

        cls.push_back(CLSID_DivX);
        fnames.push_back("DivX Decoder Filter");
        
        for (i=0; i < (int)cls.size(); i++)
        {
            hr = CoCreateInstance(cls[i], 0, CLSCTX_INPROC_SERVER,
                                IID_IBaseFilter, (void**)&m_pDecoder);

            hr = m_pGB->AddFilter(m_pDecoder, L"Video Decoder");

            try    
            {
                hr = ConnectFilters(m_pGB, m_pSourceFilter, m_pDecoder);
            }
            catch(...) 
            {
                hr = E_FAIL;
            }            

            if (SUCCEEDED(hr))
            {
                hr = ConnectFilters(m_pGB, m_pDecoder, m_pTrans032Filter);
            }
            
            if (FAILED(hr))
            {
                hr = m_pGB->RemoveFilter(m_pDecoder);
                res = m_pDecoder->Release();
                m_pDecoder = 0;
                hr = E_FAIL;
            }
            else
            {
                break;
            }
        }

        if (FAILED(hr)) 
        { 
            MessageBox(0, "[DSHOW] Can't render stream SourceFilter to Trans032Filter.", "ERROR MESSAGE", MB_ICONERROR); 
            CleanUp(); 
            return false; 
        }

        if (SUCCEEDED(hr))
        {
            hr = m_pBuilder->RenderStream(0, 0, m_pTrans032Filter, 0, m_pVideoRenderFilter);
        }

        if (FAILED(hr)) 
        { 
            MessageBox(0, "[DSHOW] Can't render stream Trans032Filter to VideoRenderFilter.", "ERROR MESSAGE", MB_ICONERROR); 
            CleanUp(); 
            return false; 
        }
    }
    
    hr = m_pGB->QueryInterface(IID_IMediaFilter, (void **)&m_pMF); 

    hr = m_pGB->QueryInterface(IID_IVideoWindow,(void **)&m_pVW);
    
    hr = m_pVW->put_WindowStyle(WS_CHILD | WS_CLIPSIBLINGS);
    
    hr = m_pVW->put_Owner(*((OAHWND*)pHWnd));
    
    hr = m_pVW->put_MessageDrain(*((OAHWND*)pHWnd));

    hr = m_pMS->GetStopPosition(&m_Duration);

    hr = GetPin(m_pTrans032Filter, PINDIR_INPUT, &pIn);
    if (FAILED(hr)) { CleanUp(); return false; }

    hr = pIn->ConnectionMediaType(&mt);
    if (FAILED(hr)) { CleanUp(); return false; }

    if (mt.formattype == FORMAT_VideoInfo)
    {        
        VIDEOINFOHEADER  *pVi = (VIDEOINFOHEADER*)mt.pbFormat;
        
        m_Width = abs(pVi->bmiHeader.biWidth);
        m_Height = abs(pVi->bmiHeader.biHeight);
    }
    else if (mt.formattype == FORMAT_VideoInfo2)
    {
        VIDEOINFOHEADER2 *pVi = (VIDEOINFOHEADER2*)mt.pbFormat;

        m_Width = abs(pVi->bmiHeader.biWidth);
        m_Height = abs(pVi->bmiHeader.biHeight);
    }
    else
    {
        CleanUp(); 
        return false;
    }
    
    m_pMS->SetTimeFormat(&TIME_FORMAT_MEDIA_TIME);

    m_pTrans032->reInitialize = true;

    m_Inited = true;

    return true;
}

/////////////////////////////////////////////////////////////////////////////

bool DSVideo::OpenMovieHard(std::string csMovieName, void *pHWnd)
{     
    m_type = -3;
    return OpenMovieNormally(csMovieName, pHWnd);
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

        pIn = 0;
        hr = GetPin(m_pVideoRenderFilter, PINDIR_INPUT, &pIn);
        if (hr != S_OK) break;
    
        pOut = 0;
        hr = pIn->ConnectedTo(&pOut);
        if (hr != S_OK) break;

        hr = m_pVW->put_Visible(false);

        hr = m_pVW->put_MessageDrain(0);
    
        hr = m_pVW->put_Owner(0);

        m_pVW->Release();    
        m_pVW = 0;

        hr = pIn->Disconnect();
    
        hr = pOut->Disconnect();

        hr = m_pGB->RemoveFilter(m_pVideoRenderFilter);
        if (hr != S_OK) break;

        m_pVideoRenderFilter->Release();
        
        m_pVideoRenderFilter = 0;
        hr = CoCreateInstance(classID, 0, CLSCTX_INPROC_SERVER,
                                IID_IBaseFilter,(void**)&m_pVideoRenderFilter);
        if (hr != S_OK) break;

        hr = m_pGB->AddFilter(m_pVideoRenderFilter, L"Video Render");
        if (hr != S_OK) break;
        
        pIn = 0;
        hr = GetPin(m_pVideoRenderFilter, PINDIR_INPUT, &pIn);
        if (hr != S_OK) break;

        hr = m_pGB->ConnectDirect(pOut, pIn, 0);
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
    IBaseFilter* pFilter = 0;

    while(1)
    {
        if (m_pSampleGrabberFilter == 0) break;

        pIn = 0;
        hr = GetPin(m_pSampleGrabberFilter, PINDIR_INPUT, &pIn);
        if (hr != S_OK) break;

        pOut = 0;
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
    IBaseFilter* pFilter = 0;

    while(1)
    {
        if (m_pSampleGrabberFilter == 0) break;

        pIn = 0;
        hr = GetPin(m_pSampleGrabberFilter, PINDIR_INPUT, &pIn);
        
        while (hr == S_OK)
        {
            pOut = 0;
            hr = pIn->ConnectedTo(&pOut);
            if (hr != S_OK) { pFilter = 0; break; }

            hr = pOut->QueryPinInfo(&PinInfo);
            if (hr != S_OK) { pFilter = 0; break; }

            pFilter = PinInfo.pFilter;

            pIn = 0;
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

bool DSVideo::Set0Render()
{
    HRESULT hr;

    IPin *pOutGB, *pInVR;

    if (m_Inited == false) return false;
    
    hr = m_pVW->put_Visible(false);    

    if (m_type != 3)
    {
        hr = m_pVW->put_MessageDrain(0);
    
        hr = m_pVW->put_Owner(0);

        hr = m_pGB->RemoveFilter(m_pVideoRenderFilter);
        if (hr != S_OK) { CleanUp(); return false; }

        if (m_pSampleGrabberFilter != 0)
        {
            hr = GetPin(m_pSampleGrabberFilter, PINDIR_OUTPUT, &pOutGB);
            if (hr != S_OK) { CleanUp(); return false; }
        }
        else
        {
            hr = GetPin(m_pTrans032Filter, PINDIR_OUTPUT, &pOutGB);
            if (hr != S_OK) { CleanUp(); return false; }
        }

        m_pVideoRenderFilter = 0;
        hr = CoCreateInstance(CLSID_NullRenderer, 0, CLSCTX_INPROC_SERVER,
                                IID_IBaseFilter,(void**)&m_pVideoRenderFilter);

        hr = m_pGB->AddFilter(m_pVideoRenderFilter, L"Video Render");

        hr = GetPin(m_pVideoRenderFilter, PINDIR_INPUT, &pInVR);
        if (hr != S_OK) { CleanUp(); return false; }

        hr = pInVR->Disconnect();

        hr = pOutGB->Disconnect();

        hr = m_pGB->Connect(pOutGB, pInVR);
        if (hr != S_OK) { CleanUp(); return false; }

        if (m_pTrans032Filter != 0)
        {
            m_pTrans032->reInitialize = true;
        }
    }
    else
    {
        hr = m_pVW->SetWindowPosition(-(100 + m_Width), -(100 + m_Height), m_Width, m_Height);
    }

    hr = m_pMF->SetSyncSource(0);

    m_IsMSSuported = false;

    m_IsSet0Render = true;

    return true;
}

/////////////////////////////////////////////////////////////////////////////

void DSVideo::SetPos(qint64 Pos)
{
    HRESULT hr;
    qint64 endPos;
    long evCode;
    
    endPos = m_Duration;

    m_ImageGeted = true;

    m_pMS->SetPositions(&Pos,AM_SEEKING_AbsolutePositioning,&endPos,AM_SEEKING_AbsolutePositioning);
    m_st = Pos;  

    m_ImageGeted = false;
    hr = m_pMC->Run();
    hr = m_pME->WaitForCompletion(300, &evCode);

    if (m_ImageGeted != true)
    {
        m_pMC->Pause();
    }
}

/////////////////////////////////////////////////////////////////////////////

void DSVideo::SetPos(double pos)
{
    HRESULT hr;
    qint64 Pos, endPos;
    long evCode;

    endPos = m_Duration;
    Pos = (qint64)(pos*10000000.0);

    m_ImageGeted = true;

    m_pMS->SetPositions(&Pos,AM_SEEKING_AbsolutePositioning,&endPos,AM_SEEKING_AbsolutePositioning);
    m_st = Pos;  

    m_ImageGeted = false;
    hr = m_pMC->Run();
    hr = m_pME->WaitForCompletion(300, &evCode);

    if (m_ImageGeted != true)
    {
        m_pMC->Pause();
    }
}

/////////////////////////////////////////////////////////////////////////////

void DSVideo::SetPosFast(qint64 Pos)
{
    HRESULT hr;
    qint64 endPos;
    long evCode;
    
    endPos = m_Duration;

    m_ImageGeted = true;

    hr = m_pMS->SetPositions(&Pos,AM_SEEKING_AbsolutePositioning,&endPos,AM_SEEKING_AbsolutePositioning);
    m_st = Pos;  

    hr = m_pME->WaitForCompletion(300, &evCode);
    
}

/////////////////////////////////////////////////////////////////////////////

void DSVideo::Stop()
{
    if(m_pMC != 0)
    {
        HRESULT hr = m_pMC->Pause();
        this->SetPos((qint64)0);
        m_pMC->Stop();
    }
}

/////////////////////////////////////////////////////////////////////////////

void DSVideo::StopFast()
{
    this->SetPos((qint64)0);
    m_pMC->Stop();
}

/////////////////////////////////////////////////////////////////////////////

HRESULT DSVideo::CleanUp()
{
    HRESULT hr = S_OK;
    std::string log;
    int i, max_n = 100;
    IPin *pOut, *pIn;
    std::fstream fout;
    std::string fname;

    fname = m_Dir + std::string("/clean_video.log");
    fout.open(fname.c_str(), std::ios_base::out | std::ios_base::app);
    fout <<    "";
    fout.close();

    this->Stop();

    log += "PASS: m_pME->WaitForCompletion(...)\n";
    
    if (m_pVW != 0)
    {
        hr = m_pVW->put_Visible(OAFALSE);
        hr = m_pVW->put_MessageDrain(0);
        hr = m_pVW->put_Owner(0);
    }

    if (m_pDecoder == 0)
    {
        m_pDecoder = GetDecoder();
    }

    if (m_pDecoder != 0)    log += "PASS: GetDecoder()\n";
    else log += "FAIL: GetDecoder()\n";

    if (m_pVideoRenderFilter != 0)
    {
        pOut = 0;
        hr = GetPin(m_pVideoRenderFilter, PINDIR_OUTPUT, &pOut);
        pIn = 0;
        hr = GetPin(m_pVideoRenderFilter, PINDIR_INPUT, &pIn);

        if (pOut != 0) hr = pOut->Disconnect();
        if (pIn != 0) hr = pIn->Disconnect();

        hr = m_pGB->RemoveFilter(m_pVideoRenderFilter);

        if (hr == S_OK) log += "PASS: Delete VideoRenderFilter\n";
        else log += "FAIL: Delete VideoRenderFilter\n";
    }

    if (m_pSampleGrabberFilter != 0)
    {
        hr = m_pGB->RemoveFilter(m_pSampleGrabberFilter);
        
        if (hr == S_OK) log += "PASS: Delete SampleGrabberFilter\n";
        else log += "FAIL: Delete SampleGrabberFilter\n";
    }

    if (m_pDecoder != 0)
    {
        hr = m_pGB->RemoveFilter(m_pDecoder);

        if (hr == S_OK) log += "PASS: Delete Decoder\n";
        else log += "FAIL: Delete Decoder\n";
    }

    if(m_pMC != 0) i = m_pMC->Release(); 
    m_pMC = 0;

    if(m_pME != 0) i = m_pME->Release();
    m_pME = 0;

    if(m_pMS != 0) i = m_pMS->Release();
    m_pMS = 0;

    if(m_pMF != 0) i = m_pMF->Release(); 
    m_pMF = 0;

    if(m_pVW != 0) i = m_pVW->Release();    
    m_pVW = 0;

    if(m_pBV != 0) i = m_pBV->Release();
    m_pBV = 0;

    if(m_pBA != 0) i = m_pBA->Release();
    m_pBA = 0;

    if(m_pDecoder != 0) i = m_pDecoder->Release();
    m_pDecoder = 0;
    
    if(m_pGrabber != 0) i = m_pGrabber->Release();
    m_pGrabber = 0;

    if(m_pSampleGrabberFilter != 0) i = m_pSampleGrabberFilter->Release();     
    m_pSampleGrabberFilter = 0;

    if(m_pTrans032Filter != 0) i = m_pTrans032Filter->Release();     
    m_pTrans032Filter = 0;
    
    if(m_pSourceFilter != 0) i = m_pSourceFilter->Release();
    m_pSourceFilter = 0;

    if(m_pVideoRenderFilter != 0) i = m_pVideoRenderFilter->Release();
    m_pVideoRenderFilter = 0;

    /*if(m_pGB != 0)
    {
        n = 0;

        IEnumFilters *pEnum = 0;
        hr = m_pGB->EnumFilters(&pEnum);
        if (SUCCEEDED(hr))
        {            
            IBaseFilter *pFilter = 0;
            while (S_OK == pEnum->Next(1, &pFilter, 0))
            {
                n++;
                hr = m_pGB->RemoveFilter(pFilter);
                hr = pEnum->Reset();
                hr = pFilter->Release();
                pFilter = 0;

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

    if(m_pGB != 0) i = m_pGB->Release();
    m_pGB = 0;

    if(m_pBuilder != 0) i = m_pBuilder->Release();
    m_pBuilder = 0;

    if (m_pSGCallback) delete m_pSGCallback;
    m_pSGCallback = 0;
    
    CoUninitialize();
    
    if (m_pBuffer) delete[] m_pBuffer;
    m_pBuffer = 0;

    m_Inited = false;

    m_IsMSSuported = true;
    m_st = 0;

    m_IsSet0Render = false;

    fname = m_Dir + std::string("\\clean_video.log");
    fout.open(fname.c_str(), std::ios::out);
    fout <<    log;
    fout.close();

    return hr;
}

/////////////////////////////////////////////////////////////////////////////

void DSVideo::OneStep()
{
    long evCode;

    m_ImageGeted = false;
    m_pMC->Run();
    m_pME->WaitForCompletion(300, &evCode);

    if (m_ImageGeted != true)
    {
        m_pMC->Pause();
    }
}

/////////////////////////////////////////////////////////////////////////////

qint64 DSVideo::OneStepWithTimeout()
{
    qint64 CurPos, PrevPos, tmpPos;
    clock_t start_t, dt = 10000;
    long evCode;
    long min_frame_rate = 15;
    long ddt = (qint64)120*(qint64)10000;
    int bln = 0;

    /*std::string fname = m_Dir + std::string("\\OneStepWithTimeout.log");
    std::ofstream fout;
    fout.open(fname.c_str(), std::ios::out );
    fout <<    "start\n";
    fout.close();*/

    PrevPos = this->GetPos();

    if (PrevPos == m_Duration)
    {
        CurPos = PrevPos;
    }
    else
    {
        m_ImageGeted = false;
        m_pMC->Run();
        m_pME->WaitForCompletion(500, &evCode);
        m_pMC->Pause();

        if ( (m_ImageGeted == false) && 
              (PrevPos >= (m_Duration - ddt)) )
        {
            m_ImageGeted = true;
            CurPos = m_Duration;
        }
        else
        {
            if ( (m_ImageGeted == false) && 
                (PrevPos < (m_Duration - ddt*2)) )
            {
                start_t = clock();

                while ( ((clock() - start_t) < dt) && 
                        (m_ImageGeted == false) )
                {
                    this->SetPos(PrevPos);

                    m_pMC->Run();
                    m_pME->WaitForCompletion(1000, &evCode);
                    m_pMC->Pause();
                }

                if (m_ImageGeted == false)
                {
                    this->SetPos(PrevPos + ddt/3);
                }

                if (m_ImageGeted == false)
                {
                    this->SetPos(PrevPos + ddt/2);
                }

                if (m_ImageGeted == false)
                {
                    this->SetPos(PrevPos + (ddt*2)/3);
                }

                if (m_ImageGeted == false)
                {
                    this->SetPos(PrevPos + ddt);
                }

                if (m_ImageGeted == false)
                {
                    this->SetPos(PrevPos + (ddt*3)/2);
                }

                if (m_ImageGeted == false)
                {
                    this->SetPos(PrevPos + ddt*2);
                }

                if (m_ImageGeted == false)
                {
                    CurPos = PrevPos;
                    bln = 1;
                    //MessageBox(0, "Can'nt get new image.", "OneStepWithTimeout in DSVideo", MB_ICONERROR);
                }
            }
            
            if (bln == 0)
            {
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
                        //MessageBox(0, "Can'nt get image with pos != prev_pos.", "OneStepWithTimeout in DSVideo", MB_ICONERROR);
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
    }

    /*fout.open(fname.c_str(), std::ios::app );
    fout <<    "end\n";
    fout.close();*/

    return CurPos;
}

/////////////////////////////////////////////////////////////////////////////

void DSVideo::ErrorMessage(std::string str)
{
    MessageBox(0, str.c_str(), "ERROR MESSAGE", MB_ICONERROR);
}

/////////////////////////////////////////////////////////////////////////////

qint64 DSVideo::GetPos()
{
    qint64 pos = -1;

    if ( m_IsMSSuported )
    {
        HRESULT hr = m_pMS->GetCurrentPosition(&pos);

        if (hr == E_NOTIMPL)
        {
            pos = m_st;
            m_IsMSSuported = false;
        }
    }
    else
    {
        pos = m_st;
    }

    return pos;
}

/////////////////////////////////////////////////////////////////////////////

void DSVideo::GetRGBImage(int *ImRGB, int xmin, int xmax, int ymin, int ymax)
{
    long evCode;
    int w, h, x, y, i, j, di;
    int *pBuffer = m_pBuffer;

    if (!pBuffer)
    {
        m_pBuffer = new int[m_Width*m_Height];
        pBuffer = m_pBuffer;

        m_ImageGeted = false;
        m_pMC->Run();
        m_pME->WaitForCompletion(300, &evCode);        
    }

    w = (xmax - xmin) + 1;
    h = (ymax - ymin) + 1;

    if ((w == m_Width) && (h == m_Height))
    {
        memcpy(ImRGB, pBuffer, m_Width * m_Height * sizeof(int));
    }
    else
    {    
        di = m_Width - w;

        i = (ymin * m_Width) + xmin;
        j = 0;

        for(y = 0; y < h; ++y)
        {
            for(x = 0; x < w; ++x)
            {
                ImRGB[j] = pBuffer[i];
                i++;
                j++;
            }
            i += di;
        }
    }
}

/////////////////////////////////////////////////////////////////////////////

void DSVideo::SetImageGeted(bool ImageGeted)
{
    m_ImageGeted = ImageGeted;
}

/////////////////////////////////////////////////////////////////////////////

void DSVideo::RunWithTimeout(qint64 timeout)
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

void DSVideo::WaitForCompletion(qint64 timeout)
{
    long evCode;

    m_pME->WaitForCompletion((long)timeout, &evCode);
}

/////////////////////////////////////////////////////////////////////////////

void DSVideo::SetVideoWindowPosition(int left, int top, int width, int height)
{
    m_pVW->SetWindowPosition(left, top, width, height);
}

qint64 DSVideo::PosToMilliSeconds(qint64 pos)
{
    return (pos/(qint64)10000);
}
