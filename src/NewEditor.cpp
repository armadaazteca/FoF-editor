#include <wx/wxprec.h>
#ifndef WX_PRECOMP
#include <wx/wx.h>
#endif
#include "NewEditor.h"
#include "MainWindow.h"

wxIMPLEMENT_APP(NewEditor);

bool NewEditor::OnInit()
{
	wxImage::AddHandler(new wxPNGHandler());
	MainWindow * win = new MainWindow("NewEditor", wxDefaultPosition, wxSize(DEFAULT_WIDTH, DEFAULT_HEIGHT));
	win->Show(true);
	win->Center();
	return true;
}
