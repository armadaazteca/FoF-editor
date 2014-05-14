#include <wx/wxprec.h>
#ifndef WX_PRECOMP
#include <wx/wx.h>
#include <wx/filename.h>
#endif
#include "Settings.h"
#include "DatSprOpenDialog.h"
#include "DatSprReaderWriter.h"

wxBEGIN_EVENT_TABLE(DatSprOpenDialog, wxDialog)
	EVT_BUTTON(ID_BROWSE_DAT_BUTTON, DatSprOpenDialog::OnClickBrowseButton)
	EVT_BUTTON(ID_BROWSE_SPR_BUTTON, DatSprOpenDialog::OnClickBrowseButton)
	EVT_BUTTON(ID_OPEN, DatSprOpenDialog::OnClickOpenButton)
wxEND_EVENT_TABLE()

DatSprOpenDialog::DatSprOpenDialog(wxWindow * parent) : wxDialog(parent, -1, wxT("Open .dat / .spr files"))
{
	SetMinSize(wxSize(WIDTH, -1));

	auto panel = new wxPanel(this, -1);
	auto vbox = new wxBoxSizer(wxVERTICAL);
	auto fgs = new wxFlexGridSizer(3, 3, 5, 5);

	auto datLabel = new wxStaticText(panel, -1, wxT(".dat file:"));
	Settings & settings = Settings::getInstance();
	const wxString & datPathStr = settings.get("datPath");
	datPath = new wxTextCtrl(panel, -1, datPathStr);
	auto datButton = new wxButton(panel, ID_BROWSE_DAT_BUTTON, wxT("Browse..."));
	auto sprLabel = new wxStaticText(panel, -1, wxT(".spr file:"));
	const wxString & sprPathStr = settings.get("sprPath");
	sprPath = new wxTextCtrl(panel, -1, sprPathStr);
	auto sprButton = new wxButton(panel, ID_BROWSE_SPR_BUTTON, wxT("Browse..."));
	auto openButton = new wxButton(panel, ID_OPEN, wxT("Open"));

	fgs->Add(datLabel, 0, wxALIGN_CENTER);
	fgs->Add(datPath, 1, wxEXPAND);
	fgs->Add(datButton);
	fgs->Add(sprLabel, 0, wxALIGN_CENTER);
	fgs->Add(sprPath, 1, wxEXPAND);
	fgs->Add(sprButton);
	fgs->AddGrowableCol(1, 1);
	fgs->AddSpacer(0);
	fgs->AddSpacer(0);
	fgs->Add(openButton);

	vbox->Add(fgs, 1, wxALL | wxEXPAND, 10);

	panel->SetSizer(vbox);
	vbox->Layout();
	vbox->Fit(this);
	Center();
}

void DatSprOpenDialog::OnClickBrowseButton(wxCommandEvent & event)
{
	Settings & settings = Settings::getInstance();
	const wxString & datPathStr = settings.get("datPath");
	const wxString & sprPathStr = settings.get("sprPath");
	wxString browseDir = wxFileName(event.GetId() == ID_BROWSE_DAT_BUTTON ? datPathStr : sprPathStr).GetPath();
	wxFileDialog * browseDialog = new wxFileDialog(this, wxFileSelectorPromptStr, browseDir);
	if (browseDialog->ShowModal() == wxID_OK)
	{
		if (event.GetId() == ID_BROWSE_DAT_BUTTON)
		{
			datPath->Replace(0, datPath->GetLastPosition(), browseDialog->GetPath());
		}
		else if (event.GetId() == ID_BROWSE_SPR_BUTTON)
		{
			sprPath->Replace(0, sprPath->GetLastPosition(), browseDialog->GetPath());
		}
	}
}

void DatSprOpenDialog::OnClickOpenButton(wxCommandEvent & event)
{
	const wxString & datPathStr = datPath->GetValue();
	const wxString & sprPathStr = sprPath->GetValue();
	Settings & settings = Settings::getInstance();
	settings.set("datPath", datPathStr);
	settings.set("sprPath", sprPathStr);
	settings.save();
	if (DatSprReaderWriter::getInstance().readDat(datPathStr))
	{
		if (DatSprReaderWriter::getInstance().readSpr(sprPathStr))
		{
			Close();
		}
	}
	else
	{
		wxMessageBox("The .dat file cannot be read", "Error", wxOK | wxICON_ERROR);
	}
}
