#include <wx/wxprec.h>
#ifndef WX_PRECOMP
#include <wx/wx.h>
#endif
#include "NewEditor.h"
#include "MainWindow.h"
#include "Config.h"

wxIMPLEMENT_APP(NewEditor);

bool NewEditor::OnInit()
{
	wxImage::AddHandler(new wxPNGHandler());
	const wxString & appTitle = wxString::Format("%s %s", Config::APP_NAME, Config::VERSION_STRING);
	MainWindow * win = new MainWindow(appTitle, wxDefaultPosition, wxSize(Config::MAIN_WINDOW_WIDTH, Config::MAIN_WINDOW_HEIGHT));
	win->Show(true);
	win->Center();
	return true;
}
