#include <wx/wxprec.h>
#ifndef WX_PRECOMP
#include <wx/wx.h>
#endif
#include <wx/filename.h>
#include <wx/gbsizer.h>
#include "Settings.h"
#include "Events.h"
#include "DatSprOpenSaveDialog.h"
#include "DatSprReaderWriter.h"

wxBEGIN_EVENT_TABLE(DatSprOpenSaveDialog, wxDialog)
	EVT_BUTTON(ID_BROWSE_DAT_BUTTON, DatSprOpenSaveDialog::OnClickBrowseButton)
	EVT_BUTTON(ID_BROWSE_SPR_BUTTON, DatSprOpenSaveDialog::OnClickBrowseButton)
	EVT_BUTTON(ID_OPEN, DatSprOpenSaveDialog::OnClickOpenSaveButton)
	EVT_BUTTON(ID_SAVE, DatSprOpenSaveDialog::OnClickOpenSaveButton)
wxEND_EVENT_TABLE()

const wxString DatSprOpenSaveDialog::DEFAULT_FILENAME = "Tibia";

DatSprOpenSaveDialog::DatSprOpenSaveDialog(wxWindow * parent, unsigned int mode) : wxDialog(parent, -1, wxT("Open .dat / .spr files")), currentLoading(LOADING_DAT)
{
	this->mode = mode;

	SetMinSize(wxSize(WIDTH, -1));

	auto panel = new wxPanel(this, -1);
	auto vbox = new wxBoxSizer(wxVERTICAL);
	auto gbs = new wxGridBagSizer(5, 5);

	auto datLabel = new wxStaticText(panel, -1, wxT(".dat file:"));
	Settings & settings = Settings::getInstance();
	const wxString & datPathStr = (mode == MODE_OPEN ? settings.get("datOpenPath") : settings.get("datSavePath"));
	datPath = new wxTextCtrl(panel, -1, datPathStr);
	auto datButton = new wxButton(panel, ID_BROWSE_DAT_BUTTON, wxT("Browse..."));
	auto sprLabel = new wxStaticText(panel, -1, wxT(".spr file:"));
	const wxString & sprPathStr = (mode == MODE_OPEN ? settings.get("sprOpenPath") : settings.get("sprSavePath"));
	sprPath = new wxTextCtrl(panel, -1, sprPathStr);
	auto sprButton = new wxButton(panel, ID_BROWSE_SPR_BUTTON, wxT("Browse..."));
	progress = new wxGauge(panel, -1, 100);
	auto openSaveButton = (mode == MODE_OPEN ? new wxButton(panel, ID_OPEN, wxT("Open")) : new wxButton(panel, ID_SAVE, wxT("Save")));
	openSaveButton->SetFocus();

	gbs->Add(datLabel, wxGBPosition(0, 0), wxDefaultSpan, wxALIGN_CENTER);
	gbs->Add(datPath, wxGBPosition(0, 1), wxDefaultSpan, wxEXPAND);
	gbs->Add(datButton, wxGBPosition(0, 2), wxDefaultSpan);
	gbs->Add(sprLabel, wxGBPosition(1, 0), wxDefaultSpan, wxALIGN_CENTER);
	gbs->Add(sprPath, wxGBPosition(1, 1), wxDefaultSpan, wxEXPAND);
	gbs->Add(sprButton, wxGBPosition(1, 2), wxDefaultSpan);
	gbs->AddGrowableCol(1, 1);
	gbs->Add(progress, wxGBPosition(2, 0), wxGBSpan(1, 3), wxEXPAND);
	gbs->Add(openSaveButton, wxGBPosition(3, 2));

	vbox->Add(gbs, 1, wxALL | wxEXPAND, 10);

	panel->SetSizer(vbox);
	vbox->Layout();
	vbox->Fit(this);
	Center();
}

void DatSprOpenSaveDialog::OnClickBrowseButton(wxCommandEvent & event)
{
	Settings & settings = Settings::getInstance();
	const wxString & datPathStr = (mode == MODE_OPEN ? settings.get("datOpenPath") : settings.get("datSavePath"));
	const wxString & sprPathStr = (mode == MODE_OPEN ? settings.get("sprOpenPath") : settings.get("sprSavePath"));
	wxString browseDir, caption, defaultFilename, filter;
	if (event.GetId() == ID_BROWSE_DAT_BUTTON)
	{
		browseDir = wxFileName(datPathStr).GetPath();
		caption = "Select .dat file";
		defaultFilename = (mode == MODE_OPEN ? "" : DEFAULT_FILENAME + ".dat");
		filter = "*.dat|*.dat|All files|*";
	}
	else if (event.GetId() == ID_BROWSE_SPR_BUTTON)
	{
		browseDir = wxFileName(sprPathStr).GetPath();
		caption = "Select .spr file";
		defaultFilename = (mode == MODE_OPEN ? "" : DEFAULT_FILENAME + ".spr");
		filter = "*.spr|*.spr|All files|*";
	}
	int flags = (mode == MODE_OPEN ? wxFD_OPEN | wxFD_FILE_MUST_EXIST : wxFD_SAVE | wxFD_OVERWRITE_PROMPT);
	wxFileDialog * browseDialog = new wxFileDialog(this, caption, browseDir, defaultFilename, filter, flags);
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

void DatSprOpenSaveDialog::OnClickOpenSaveButton(wxCommandEvent & event)
{
	const wxString & datPathStr = datPath->GetValue();
	const wxString & sprPathStr = sprPath->GetValue();
	Settings & settings = Settings::getInstance();
	if (datPathStr.Length() == 0 || sprPathStr.Length() == 0)
	{
		wxMessageDialog error(this, ".dat / .spr paths cannot be empty", "Error", wxOK | wxICON_ERROR);
		error.ShowModal();
		return;
	}

	if (mode == MODE_OPEN)
	{
		settings.set("datOpenPath", datPathStr);
		settings.set("sprOpenPath", sprPathStr);
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
	else if (mode == MODE_SAVE)
	{
		settings.set("datSavePath", datPathStr);
		settings.set("sprSavePath", sprPathStr);
		currentLoading = LOADING_DAT;
		if (DatSprReaderWriter::getInstance().writeDat(datPathStr, this))
		{
			currentLoading = LOADING_SPR;
			if (DatSprReaderWriter::getInstance().writeSpr(sprPathStr, this))
			{
				Close();
				// notifying main window of that files has been loaded
				//wxCommandEvent event(DAT_SPR_LOADED, 1);
				//wxPostEvent(this->m_parent, event);
			}
			else
			{
				wxMessageBox("The .spr file cannot be written", "Error", wxOK | wxICON_ERROR);
			}
		}
		else
		{
			wxMessageBox("The .dat file cannot be written", "Error", wxOK | wxICON_ERROR);
		}
	}
}

void DatSprOpenSaveDialog::updateProgress(double value)
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

DatSprOpenSaveDialog::~DatSprOpenSaveDialog()
{

}

