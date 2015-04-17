#include <wx/wxprec.h>
#ifndef WX_PRECOMP
#include <wx/wx.h>
#endif
#include <wx/hyperlink.h>
#include "AboutDialog.h"
#include "Config.h"

AboutDialog::AboutDialog(wxWindow * parent) : wxDialog(parent, wxID_ANY, wxT("About"), wxDefaultPosition, wxSize(640, 480))
{
	auto vbox = new wxBoxSizer(wxVERTICAL);


  const wchar_t * aboutText = wxT("%s v%s\n\n")
	                            wxT("The purpose of this software is to create and edit game objects ")
	                            wxT("(items, creatures, effects, projectiles) and their corresponding graphic ")
	                            wxT("sprites. Each sprite is 32x32 pixels, but sprites may be composed into larger picture.\n\n")
	                            wxT("Game object data (mostly various boolean and numeric attributes) is saved ")
	                            wxT("into .dat file.\nSprites are saved into .spr file. ")
	                            wxT("These formats are binary and has specific structure.\n");
  const wxString & at = wxString::Format(aboutText, Config::APP_NAME.wc_str(), Config::VERSION_STRING.wc_str());
	auto st = new wxStaticText(this, wxID_ANY, at);
	vbox->Add(st, 0, wxALL, 10);
	st = new wxStaticText(this, wxID_ANY, wxT("Here are some links to the articles, describing these formats:"));
	vbox->Add(st, 0, wxLEFT | wxRIGHT | wxBOTTOM, 10);
	auto hl = new wxHyperlinkCtrl(this, wxID_ANY, wxT(""), wxT("http://tpforums.org/forum/threads/5030-Tibia-Data-File-Structure"));
	vbox->Add(hl, 0, wxLEFT | wxRIGHT, 10);
	hl = new wxHyperlinkCtrl(this, wxID_ANY, wxT(""), wxT("http://tpforums.org/forum/threads/899-Reading-the-Tibia-dat-file"));
	vbox->Add(hl, 0, wxLEFT | wxRIGHT, 10);
	hl = new wxHyperlinkCtrl(this, wxID_ANY, wxT(""), wxT("http://tpforums.org/forum/threads/5031-Tibia-Sprite-File-Structure"));
	vbox->Add(hl, 0, wxLEFT | wxRIGHT, 10);
	hl = new wxHyperlinkCtrl(this, wxID_ANY, wxT(""), wxT("http://otland.net/threads/tibia-dat-reader-dat-spr-structure-and-spr-reading-code-link.25117"));
	vbox->Add(hl, 0, wxLEFT | wxRIGHT, 10);

	SetSizer(vbox);
	Center();
}
