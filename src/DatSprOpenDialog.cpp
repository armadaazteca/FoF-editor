#include <wx/wxprec.h>
#ifndef WX_PRECOMP
#include <wx/wx.h>
#endif
#include <wx/filename.h>
#include <wx/gbsizer.h>
#include "Settings.h"
#include "Events.h"
#include "DatSprOpenDialog.h"
#include "DatSprReaderWriter.h"

wxBEGIN_EVENT_TABLE(DatSprOpenDialog, wxDialog)
	EVT_BUTTON(ID_BROWSE_DAT_BUTTON, DatSprOpenDialog::OnClickBrowseButton)
	EVT_BUTTON(ID_BROWSE_SPR_BUTTON, DatSprOpenDialog::OnClickBrowseButton)
	EVT_BUTTON(ID_OPEN, DatSprOpenDialog::OnClickOpenButton)
	//EVT_COMMAND(wxID_ANY, DAT_SPR_LOADED, DatSprOpenDialog::OnDatSprLoaded)
wxEND_EVENT_TABLE()

DatSprOpenDialog::DatSprOpenDialog(wxWindow * parent) : wxDialog(parent, -1, wxT("Open .dat / .spr files")), currentLoading(LOADING_DAT)
{
	SetMinSize(wxSize(WIDTH, -1));

	auto panel = new wxPanel(this, -1);
	auto vbox = new wxBoxSizer(wxVERTICAL);
	auto gbs = new wxGridBagSizer(5, 5);

	auto datLabel = new wxStaticText(panel, -1, wxT(".dat file:"));
	Settings & settings = Settings::getInstance();
	const wxString & datPathStr = settings.get("datPath");
	datPath = new wxTextCtrl(panel, -1, datPathStr);
	auto datButton = new wxButton(panel, ID_BROWSE_DAT_BUTTON, wxT("Browse..."));
	auto sprLabel = new wxStaticText(panel, -1, wxT(".spr file:"));
	const wxString & sprPathStr = settings.get("sprPath");
	sprPath = new wxTextCtrl(panel, -1, sprPathStr);
	auto sprButton = new wxButton(panel, ID_BROWSE_SPR_BUTTON, wxT("Browse..."));
	progress = new wxGauge(panel, -1, 100);
	auto openButton = new wxButton(panel, ID_OPEN, wxT("Open"));

	gbs->Add(datLabel, wxGBPosition(0, 0), wxDefaultSpan, wxALIGN_CENTER);
	gbs->Add(datPath, wxGBPosition(0, 1), wxDefaultSpan, wxEXPAND);
	gbs->Add(datButton, wxGBPosition(0, 2), wxDefaultSpan);
	gbs->Add(sprLabel, wxGBPosition(1, 0), wxDefaultSpan, wxALIGN_CENTER);
	gbs->Add(sprPath, wxGBPosition(1, 1), wxDefaultSpan, wxEXPAND);
	gbs->Add(sprButton, wxGBPosition(1, 2), wxDefaultSpan);
	gbs->AddGrowableCol(1, 1);
	gbs->Add(progress, wxGBPosition(2, 0), wxGBSpan(1, 3), wxEXPAND);
	gbs->Add(openButton, wxGBPosition(3, 2));

	vbox->Add(gbs, 1, wxALL | wxEXPAND, 10);

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
	currentLoading = LOADING_DAT;
	if (DatSprReaderWriter::getInstance().readDat(datPathStr, this))
	{
		currentLoading = LOADING_SPR;
		if (DatSprReaderWriter::getInstance().readSpr(sprPathStr, this))
		{
			Close();
			// notifying main window of that files has been loaded
			wxCommandEvent event(DAT_SPR_LOADED, 1);
			wxPostEvent(this->m_parent, event);
		}
		else
		{
			wxMessageBox("The .spr file cannot be read", "Error", wxOK | wxICON_ERROR);
		}
	}
	else
	{
		wxMessageBox("The .dat file cannot be read", "Error", wxOK | wxICON_ERROR);
	}
}

void DatSprOpenDialog::updateProgress(double value)
{
	if (currentLoading == LOADING_DAT)
	{
		progress->SetValue(value * 50);
	}
	else if (currentLoading == LOADING_SPR)
	{
		progress->SetValue(50 + value * 50);
	}
	wxTheApp->Yield();
}

DatSprOpenDialog::~DatSprOpenDialog()
{

}

