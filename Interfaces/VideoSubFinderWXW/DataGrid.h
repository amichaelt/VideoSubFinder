                              //DataGrid.h//                                
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
#include <wx/grid.h>

class CDataGrid : public wxGrid
{
public:
	CDataGrid ( wxWindow* parent,
				wxWindowID id = wxID_ANY,
				const wxPoint& pos = wxDefaultPosition,
				const wxSize& size = wxDefaultSize );		
	~CDataGrid();

public:
	//bool SetFont(const wxFont& font);
	//void SetLabel(const wxString& label);
	//bool SetBackgroundColour(const wxColour& colour);
	//void SetTextColour(const wxColour& colour);
	void OnSize(wxSizeEvent& event);

private:
   DECLARE_EVENT_TABLE()
};