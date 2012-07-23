                              //IPAlgorithms.h//                                
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

#ifndef IPALGORITHMS_H
#define IPALGORITHMS_H

#include "SSAlgorithms.h"
#include "MyClosedFigure.h"
#include <string>
#include <fstream>

#include <QtCore/QtGlobal>

#include <stdio.h>
#include <wx/image.h>

extern void (*g_pViewRGBImage)(int *Image, int width, int height);
extern void (*g_pViewImage[2])(int *Image, int width, int height);

extern int g_Width;
extern int g_Height;
extern int g_width;
extern int g_height;
extern int g_xmin;
extern int g_xmax;
extern int g_ymin;
extern int g_ymax;

extern std::string g_Directory;

extern double g_ModerateThreshold;                          //moderate threshold
extern double g_ModerateVerticalEdgesThreshold;             //moderate threshold for VEdges
extern double g_ModerateHorizontalEdgesThreshold;           //moderate threshold for HEdges
extern double g_ModerateNEdgesThreshold;                    //moderate threshold for NEdges
extern int g_HorizontalVerticalEdgesThreshold;              //horizontal-vertical edges threshold
extern int g_SegmentWidth;                                  //segment width
extern int g_SegmentHeight;                                 //segment height
extern int g_MinSegmentsCount;                              //minimum segments count
extern int g_MinSumColorDifference;                         //min sum color diff
extern int g_MinSumMultipleColorDifference;                 //min sum multiple color diff
extern double g_DistanceBetweenText;                        //between text distace
extern double g_CenterTextOffset;                           //text centre offset
extern double g_CenterTextOffsetPercent;                    //text centre percent offset

extern int g_MinPointsNumber;                               //min points number
extern double g_MinPointsDensity;                           //min points density
extern double g_MinVerticalEdgesPointsDensityPerLine;       //min VEdges points density (per full line)
extern double g_MinHorizontalEdgesPointsDensityPerLine;     //min HEdges points density (per full line)
extern double g_MinNEdgesPointsDensityPerLine;              //min NEdges points density (per full line)
extern double g_MinVerticalEdgesPointsDensity;              //min VEdges points density
extern double g_MinNEdgesPointsDensity;                     //min NEdges points density

extern int g_scale;

extern int *g_ImRES1;
extern int *g_ImRES2;
extern int *g_ImRES3;

extern int *g_RGBImage;
extern int *g_ImF[6];

extern int *g_pLB;
extern int *g_pLE;
extern int g_LN;

extern int *g_pLB6;
extern int *g_pLE6;
extern int *g_pLB7;
extern int *g_pLE7;
extern int *g_pLB8;
extern int *g_pLE8;
extern int *g_pLB9;
extern int *g_pLE9;

extern int *g_pImFF1;
extern int *g_pImVE1;
extern int *g_pImNE1;
extern int *g_pImFF2;
extern int *g_pImVE2;
extern int *g_pImNE2;
extern int *g_TempImage1;
extern int *g_TempImage2;
extern int *g_TempImage3;
extern int *g_ImRES10;
extern int *g_ImRES11;
extern int *g_ImRES12;

extern bool g_blnVNE;
extern bool g_blnHE;

extern bool g_debug;

extern bool g_MMX_SSE;

extern bool g_hard_sub_mining;
extern bool g_show_results;

extern int g_dmaxy;

void InitIPData(int w, int h, int scale);
void ReleaseIPData();

void YIQ_to_RGB(int Y, int I, int Q, int &R, int &G, int &B, int max_val);

void RGB_to_YUV(int *inImage, int *ImageY, int *ImageU, int *ImageV, int width, int height);
void RGB_to_YIQ(int *inImage, int *ImageY, int *ImageI, int *ImageQ, int width, int height);
void RGB_to_YIQ(int *RGBImage, qint64 *YIQImage, int width, int height);

void ImprovedSobelMEdge(int *ImIn, int *ImMOE, int w, int h);
void ImprovedSobelAllEdge_MMX_SSE(qint64 *ImYIQ, int *ImMOE1, int *ImMOE2, int *ImVOE, int *ImNOE, int *ImHOE, double vthr, double nthr, double hthr, int w, int h);
void FastImprovedSobelHEdge(int *ImIn, int *ImHOE, int w, int h);
void FastImprovedSobelVEdge(int *ImIn, int *ImVOE, int w, int h);
void FastImprovedSobelNEdge(int *ImIn, int *ImNOE, int w, int h);

void FindAndApplyGlobalThreshold(int *Im, int w, int h);
void FindAndApplyLocalThresholding(int *Im, int dw, int dh, int w, int h);
void ApplyModerateThreshold(int *Im, double mthr, int w, int h);
void ApplyModerateThreshold_MMX_SSE(int *Im, double mthr, int w, int h);

void ApplyEdgeStrengthSmoothing(int *ImIn, int* ImOut, int w, int h);
void ApplyEdgeClusteringPower(int *ImIn, int* ImOut, int w, int h);

void ColorFiltration(int *Im, int *LB, int *LE, int &N, int w, int h);

void BorderClear(int *Im, int dd, int w, int h);
void EasyBorderClear(int *Im, int w, int h);

void UnpackImage(int *ImIn, int* ImRES, int *LB, int *LE, int LN, int w, int h);

int GetTransformedImage(int *ImRGB, int *ImFF, int *ImSF, int *ImTF, int *ImVE, int *ImNE, int *ImHE, int W, int H);
int GetFastTransformedImage(int *ImRGB, int *ImF, int *ImVE, int W, int H);
int GetVeryFastTransformedImage(int *ImRGB, int *ImF, int *ImVE, int W, int H);

int SecondFiltration(int* Im, int* ImRGB, int* ImVE, int* ImNE, int *LB, int *LE, int N, int w, int h);
int ThirdFiltration(int* Im, int* ImVE, int* ImNE, int *ImHE, int *LB, int *LE, int LN, int w, int h);
int ThirdFiltrationForGFTI(int* Im, int* ImVE, int* ImNE, int *ImHE, int *LB, int *LE, int LN, int w, int h);

int FindTextLines(int *ImRGB, int *ImF, int *ImNF, std::vector<std::string> &SavedFiles, int W, int H);

void StrAnalyseImage(int *Im, int *ImGR, int *GRStr, int w, int h, int xb, int xe, int yb, int ye, int offset);
void FindMaxStrDistribution(int *GRStr, int delta, int *smax, int *smaxi, int &N, int offset);
void FindMaxStr(int *smax, int *smaxi, int &max_i, int &max_val, int N);

int AnalizeAndClearImage(int *Im, int *ImGR, int w, int h, int j1_min, int j1_max, int r, int g, int yb, int ye, int xb, int xe, int &cnt1, int &cnt2);

void StrAnalyseImage(int *Im, int *ImGR, int *GRStr, int w, int h, int xb, int xe, int yb, int ye, int offset);
void StrAnalyseImage(MyClosedFigure *pFigure, int *ImGR, int *GRStr, int offset);

void ClearImage4x4(int *Im, int w, int h, int white);
void ClearImageSpecific1(int *Im, int w, int h, int yb, int ye, int xb, int xe, int white);
void ClearImageSpecific2(int *Im, int w, int h, int LMAXY, int LH, int white);
void ClearImageSpecific(int *Im, int w, int h, int white);
int ClearImage(int *Im, int w, int h, int yb, int ye, int white);
int ClearImageDetailed(int *Im, int w, int h, int yb, int ye, int white);
int ClearImageOptimal(int *Im, int w, int h, int yb, int ye, int white);

int ClearImageOpt2(int *Im, int w, int h, int white, int &LH, int &LMAXY, 
                    int &jY_min, int &jY_max, int &jI_min, int &jI_max, int &jQ_min, int &jQ_max,
                    int &mY, int &dY, int &mI, int &dI, int &mQ, int &dQ,
                    int &mmY, int &ddY1, int &ddY2, int &mmI, int &ddI, int &mmQ, int &ddQ);

int ClearImageLogical(int *Im, int w, int h, int &LH, int &LMAXY, int xb, int xe, int white);

void SaveTextLineParameters(std::string ImageName, int YB, int LH, int LY, int LXB, int LXE, int LYB, int LYE, int mY, int mI, int mQ);
void GetSymbolAvgColor(MyClosedFigure *pFigure);
void GetTextLineParameters(int *Im, int w, int h, int &LH, int &LMAXY, int &XB, int &XE, int &YB, int &YE, int &mY, int &mI, int &mQ, int white);

int ClearImageOpt5(int *Im, int w, int h, int LH, int LMAXY, 
                    int jY_min, int jY_max, int jI_min, int jI_max, int jQ_min, int jQ_max,
                    int mY, int dY, int mI, int dI, int mQ, int dQ, 
                    int mmY, int ddY1, int ddY2, int mmI, int ddI, int mmQ, int ddQ, int white);

void ResizeImage4x(int *Im, int *ImRES, int w, int h);
void SimpleResizeImage4x(int *Im, int *ImRES, int w, int h);
void ResizeGrayscaleImage4x(int *Im, int *ImRES, int w, int h);

int CompareTXTImages(int *Im1, int *Im2, int w1, int h1, int w2, int h2, int YB1, int YB2);

void GetImageSize(std::string name, int &w, int &h);
void SaveRGBImage(int *Im, std::string name, int w, int h);
void LoadRGBImage(int *Im, std::string name, int &w, int &h);
void SaveImage(int *Im, std::string name, int w, int h, int dpi = -1);
void LoadImage(int *Im, std::string name, int &w, int &h);

#endif
