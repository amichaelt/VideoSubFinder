
//////////////////////////////////////////////////////////////////////////////////
//							VideoSubFinder.h  Version 1.75						//
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

#include "MainFrm.h"
#include "resource.h"

class CVideoSubFinderApp : public CWinApp
{
public:
	CVideoSubFinderApp();

	virtual BOOL InitInstance();

	afx_msg void OnAppAbout();

	DECLARE_MESSAGE_MAP()
};