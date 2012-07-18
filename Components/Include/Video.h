                              //Video.h//                                
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

#ifndef VIDEO_H
#define VIDEO_H

#include <QtCore/QtGlobal>
#include <Windows.h>
#include <fstream>

/////////////////////////////////////////////////////////////////////////////

class CVideo
{
public:
    CVideo()
    {
        m_MovieName = "";
        m_Inited = false;
        m_Width = 0;
        m_Height = 0;
        m_Duration = 0;
        m_log = "";
        m_Dir = "";
    }

    virtual ~CVideo()
    {
    }
    
public:    
    std::string        m_MovieName;
    bool        m_Inited;

    long        m_Width;
    long        m_Height;

    qint64            m_Duration;

    std::string        m_log;
    std::string        m_Dir;

public:

    virtual bool OpenMovieNormally(std::string csMovieName, void *pHWnd)
    {
        return false;
    }

    virtual bool OpenMovieAllDefault(std::string csMovieName, void *pHWnd)
    {
        return false;
    }

    virtual bool OpenMovieHard(std::string csMovieName, void *pHWnd)
    {
        return false;
    }

    virtual bool SetVideoWindowPlacement(void *pHWnd)
    {
        return false;
    }

    virtual bool Set0Render()
    {
        return false;
    }

    virtual bool CloseMovie()
    {
        return false;
    }

    virtual void SetPos(qint64 Pos)
    {
    }

    virtual void SetPos(double pos)
    {
    }

    virtual void SetPosFast(qint64 Pos)
    {
    }

    virtual void SetImageGeted(bool ImageGeted)
    {
    }

    virtual void Run()
    {
    }

    virtual void Pause()
    {
    }

    virtual void WaitForCompletion(qint64 timeout)
    {        
    }

    virtual void StopFast()
    {
    }

    virtual void RunWithTimeout(qint64 timeout)
    {
    }

    virtual void Stop()
    {
    }

    virtual void OneStep()
    {
    }

    virtual qint64 OneStepWithTimeout()
    {
        return 0;
    }

    virtual qint64  GetPos()
    {
        return 0;
    }

    virtual void GetRGBImage(int *ImRGB, int xmin, int xmax, int ymin, int ymax)
    {
    }

    virtual void SetVideoWindowPosition(int left, int top, int width, int height)
    {
    }
};

#endif
