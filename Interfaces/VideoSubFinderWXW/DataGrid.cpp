                              //DataGrid.cpp//                                
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

#include "DataGrid.h"

BEGIN_EVENT_TABLE(CDataGrid, wxGrid)
	EVT_SIZE(CDataGrid::OnSize)
END_EVENT_TABLE()

CDataGrid::CDataGrid( wxWindow* parent,
					  wxWindowID id,
					  const wxPoint& pos,
					  const wxSize& size )				
		:wxGrid( parent, id, pos, size )
{
	this->CreateGrid( 0, 2 );
	this->SetRowLabelSize(0);
}

CDataGrid::~CDataGrid()
{
}

void CDataGrid::OnSize(wxSizeEvent& event)
{
	int w, h, dw = 4;

    this->GetClientSize(&w, &h);

	this->SetColSize(0, (w-dw)/2);
	this->SetColSize(1, (w-dw)/2);
}
