                              //SSAlgorithms.h//                                
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

#ifndef SSALGORITHMS_H
#define SSALGORITHMS_H

#include "IPAlgorithms.h"
#include "Video.h"
#include <string>
#include <QtCore/QtGlobal>

extern bool g_RunSubSearch;

extern int g_SubFrameLength;                    //sub frame length
extern double g_TextPercent;                    //text percent
extern double g_MinTextLengthPercent;           //min text len (in percent)
extern double g_SubSquareError;                 //sub square error
extern double g_VerticalEdgesPointsLineError;   //vedges points line error

extern bool g_fast_search;

void SetVideoWindowSettins(CVideo *pV, double dx_min, double dx_max, double dy_min, double dy_max);

qint64 SearchSubtitles(CVideo *pV, qint64 Begin, qint64 End);
qint64 FastSearchSubtitles(CVideo *pV, qint64 Begin, qint64 End);

int CompareTwoImages(int *Im1, int *ImNFF1, int *Im2, int *ImNFF2, int size);

int AnalyseImage(int *Im, int w, int h);

int CompareTwoSubs(int *Im1, int *ImVE1, int *Im2, int *ImVE2, int w, int h);

int PreCompareTwoSubs(int *Image1, int *Image2, int *ImageResult, int *lb, int *le, int w, int h);      // return ln

int FinalCompareTwoSubs1(int *ImRES, int *lb, int *le, int ln, int *ImVE1, int *ImVE2, int w, int h);   // return 0 or 1
int FinalCompareTwoSubs2(int *ImRES, int *lb, int *le, int ln, int *ImVE1, int *ImVE2, int w, int h);

int DifficultCompareTwoSubs(int *ImRGB1, int *ImF1, int *ImRGB2, int *ImF2, int w, int h);

int SimpleCombineTwoImages(int *Im1, int *Im2, int size);

void AddTwoImages(int *Image1, int *Image2, int *ImageResult, int size);

int ConvertImage(int *ImRGB, int *ImF, int *ImVE, int w, int h);

int GetAndConvertImage(int *ImRGB, int *ImFF, int *ImSF, int *ImTF, int *ImVE, int *ImNE, int *ImHE, CVideo *pVideo, int w, int h);

void ImageToNativeSize(int *Im, int w, int h);

std::string VideoTimeToString(qint64 pos);

#endif
