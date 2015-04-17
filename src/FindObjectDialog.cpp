#include <wx/wxprec.h>
#ifndef WX_PRECOMP
#include <wx/wx.h>
#endif
#include <wx/valnum.h>
#include "Config.h"
#include "FindObjectDialog.h"

wxBEGIN_EVENT_TABLE(FindObjectDialog, wxDialog)
	EVT_BUTTON(wxID_FIND, FindObjectDialog::OnClickFindButton)
wxEND_EVENT_TABLE()

FindObjectDialog::FindObjectDialog(wxWindow * parent) : wxDialog(parent, wxID_ANY, "Find object")
{
	auto wrapVBox = new wxBoxSizer(wxVERTICAL), vbox = new wxBoxSizer(wxVERTICAL);

	auto findByIdSizer = new wxBoxSizer(wxHORIZONTAL);
	auto findByIdLabel = new wxStaticText(this, wxID_ANY, wxT("Find by ID: "));
	findByIdSizer->Add(findByIdLabel, 0, wxLEFT | wxTOP | wxBOTTOM | wxALIGN_CENTER_VERTICAL, 5);
	wxIntegerValidator <unsigned int> val;
	val.SetRange(0, 1000000);
	objectIDInput = new wxTextCtrl(this, wxID_ANY, "", wxDefaultPosition,
	                               wxSize(Config::COMMON_NUM_FIELD_WIDTH, -1), 0, val);
	objectIDInput->SetFocus();
	findByIdSizer->Add(objectIDInput, 0, wxALL | wxALIGN_CENTER_VERTICAL, 5);

	vbox->Add(findByIdSizer);

	auto findButton = new wxButton(this, wxID_FIND, "Find");
	findButton->SetDefault();
	
	vbox->Add(findButton, 0, wxALL | wxALIGN_RIGHT, 5);

	wrapVBox->Add(vbox, 0, wxALL, 5);

	SetSizer(wrapVBox);
	Fit();
	Center();
}

void FindObjectDialog::OnClickFindButton(wxCommandEvent & event)
{
	objectID = objectIDInput->GetValue();
	EndModal(wxOK);
}
