#include <wx/wxprec.h>
#ifndef WX_PRECOMP
#include <wx/wx.h>
#endif
#include <wx/hyperlink.h>
#include "AboutDialog.h"
#include "Config.h"

AboutDialog::AboutDialog(wxWindow * parent) : wxDialog(parent, wxID_ANY, "About", wxDefaultPosition, wxSize(640, 480))
{
	auto vbox = new wxBoxSizer(wxVERTICAL);


  const char * aboutText = "%s v%s\n\n"
	                         "The purpose of this software is to create and edit game objects "
	                         "(items, creatures, effects, projectiles) and their corresponding graphic "
	                         "sprites. Each sprite is 32x32 pixels.\n\n"
	                         "Game object data (mostly various boolean and numeric attributes) is saved "
	                         "into .dat file; sprites are saved into .spr file.\n\n"
	                         "The format of these files is based on Tibia / OpenTibia game engines.\n\n";
	auto st = new wxStaticText(this, wxID_ANY, wxString::Format(aboutText, Config::APP_NAME, Config::VERSION_STRING));
	vbox->Add(st, 0, wxALL, 10);
	auto hl = new wxHyperlinkCtrl(this, wxID_ANY, "", "http://tibia.com");
	vbox->Add(hl, 0, wxLEFT | wxRIGHT, 10);
	hl = new wxHyperlinkCtrl(this, wxID_ANY, "", "http://opentibia.net");
	vbox->Add(hl, 0, wxLEFT | wxRIGHT, 10);
	st = new wxStaticText(this, wxID_ANY, "Links to articles, describing formats:");
	vbox->Add(st, 0, wxALL, 10);
	hl = new wxHyperlinkCtrl(this, wxID_ANY, "", "http://tpforums.org/forum/threads/5030-Tibia-Data-File-Structure");
	vbox->Add(hl, 0, wxLEFT | wxRIGHT, 10);
	hl = new wxHyperlinkCtrl(this, wxID_ANY, "", "http://tpforums.org/forum/threads/899-Reading-the-Tibia-dat-file");
	vbox->Add(hl, 0, wxLEFT | wxRIGHT, 10);
	hl = new wxHyperlinkCtrl(this, wxID_ANY, "", "http://tpforums.org/forum/threads/5031-Tibia-Sprite-File-Structure");
	vbox->Add(hl, 0, wxLEFT | wxRIGHT, 10);
	hl = new wxHyperlinkCtrl(this, wxID_ANY, "", "http://otland.net/threads/tibia-dat-reader-dat-spr-structure-and-spr-reading-code-link.25117");
	vbox->Add(hl, 0, wxLEFT | wxRIGHT, 10);

	SetSizer(vbox);
}
