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
	EVT_BUTTON(ID_BROWSE_ALP_BUTTON, DatSprOpenSaveDialog::OnClickBrowseButton)
	EVT_BUTTON(ID_OPEN, DatSprOpenSaveDialog::OnClickOpenSaveButton)
	EVT_BUTTON(ID_SAVE, DatSprOpenSaveDialog::OnClickOpenSaveButton)
	EVT_CHECKBOX(ID_READ_SAVE_ALPHA_CHECKBOX, DatSprOpenSaveDialog::OnChangeReadSaveAlphaCheckbox)
wxEND_EVENT_TABLE()

const wxString DatSprOpenSaveDialog::DEFAULT_FILENAME = "Tibia";

DatSprOpenSaveDialog::DatSprOpenSaveDialog(wxWindow * parent, unsigned int mode) : wxDialog(parent, -1, wxT("Open .dat / .spr files")), currentLoading(LOADING_DAT)
{
	this->mode = mode;

	SetMinSize(wxSize(WIDTH, -1));

	auto panel = new wxPanel(this, -1);
	auto vbox = new wxBoxSizer(wxVERTICAL);
	auto gbs = new wxGridBagSizer(5, 5);
	Settings & settings = Settings::getInstance();

	const wchar_t * browse = wxT("Browse...");

	auto datLabel = new wxStaticText(panel, -1, wxT(".dat file:"));
	const wxString & datPathStr = (mode == MODE_OPEN ? settings.get("datOpenPath") : settings.get("datSavePath"));
	datPath = new wxTextCtrl(panel, -1, datPathStr);
	auto datButton = new wxButton(panel, ID_BROWSE_DAT_BUTTON, browse);

	auto sprLabel = new wxStaticText(panel, -1, wxT(".spr file:"));
	const wxString & sprPathStr = (mode == MODE_OPEN ? settings.get("sprOpenPath") : settings.get("sprSavePath"));
	sprPath = new wxTextCtrl(panel, -1, sprPathStr);
	auto sprButton = new wxButton(panel, ID_BROWSE_SPR_BUTTON, browse);

	bool alphaCheckboxValue = (mode == MODE_OPEN ? settings.get("readAlphaValue")
			: settings.get("saveAlphaValue")).IsSameAs("1");
	readOrSaveAlphaCheckbox = new wxCheckBox(panel, ID_READ_SAVE_ALPHA_CHECKBOX,
	    wxString::Format("%s alpha-transparency", (mode == MODE_OPEN ? "Read" : "Save")));
	readOrSaveAlphaCheckbox->SetValue(alphaCheckboxValue);

	alpLabel = new wxStaticText(panel, -1, wxT(".alp file:"));
	const wxString & alpPathStr = (mode == MODE_OPEN ? settings.get("alpOpenPath") : settings.get("alpSavePath"));
	alpPath = new wxTextCtrl(panel, -1, alpPathStr);
	alpButton = new wxButton(panel, ID_BROWSE_ALP_BUTTON, browse);
	alpLabel->Enable(alphaCheckboxValue);
	alpPath->Enable(alphaCheckboxValue);
	alpButton->Enable(alphaCheckboxValue);

	progress = new wxGauge(panel, -1, 100);
	auto openSaveButton = (mode == MODE_OPEN ? new wxButton(panel, ID_OPEN, wxT("Open")) : new wxButton(panel, ID_SAVE, wxT("Save")));
	openSaveButton->SetFocus();

	gbs->Add(datLabel, wxGBPosition(0, 0), wxDefaultSpan, wxALIGN_CENTER);
	gbs->Add(datPath, wxGBPosition(0, 1), wxDefaultSpan, wxEXPAND);
	gbs->Add(datButton, wxGBPosition(0, 2), wxDefaultSpan);

	gbs->Add(sprLabel, wxGBPosition(1, 0), wxDefaultSpan, wxALIGN_CENTER);
	gbs->Add(sprPath, wxGBPosition(1, 1), wxDefaultSpan, wxEXPAND);
	gbs->Add(sprButton, wxGBPosition(1, 2), wxDefaultSpan);

	gbs->Add(readOrSaveAlphaCheckbox, wxGBPosition(2, 0), wxGBSpan(1, 3), wxEXPAND);

	gbs->Add(alpLabel, wxGBPosition(3, 0), wxDefaultSpan, wxALIGN_CENTER);
	gbs->Add(alpPath, wxGBPosition(3, 1), wxDefaultSpan, wxEXPAND);
	gbs->Add(alpButton, wxGBPosition(3, 2), wxDefaultSpan);

	gbs->AddGrowableCol(1, 1);
	gbs->Add(progress, wxGBPosition(4, 0), wxGBSpan(1, 3), wxEXPAND);
	gbs->Add(openSaveButton, wxGBPosition(5, 2));

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
	const wxString & alpPathStr = (mode == MODE_OPEN ? settings.get("alpOpenPath") : settings.get("alpSavePath"));
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
	else if (event.GetId() == ID_BROWSE_ALP_BUTTON)
	{
		browseDir = wxFileName(alpPathStr).GetPath();
		caption = "Select .alp file";
		defaultFilename = (mode == MODE_OPEN ? "" : DEFAULT_FILENAME + ".alp");
		filter = "*.alp|*.alp|All files|*";
	}
	int flags = (mode == MODE_OPEN ? wxFD_OPEN | wxFD_FILE_MUST_EXIST : wxFD_SAVE | wxFD_OVERWRITE_PROMPT);
	wxFileDialog browseDialog(this, caption, browseDir, defaultFilename, filter, flags);
	if (browseDialog.ShowModal() == wxID_OK)
	{
		if (event.GetId() == ID_BROWSE_DAT_BUTTON)
		{
			datPath->Replace(0, datPath->GetLastPosition(), browseDialog.GetPath());
		}
		else if (event.GetId() == ID_BROWSE_SPR_BUTTON)
		{
			sprPath->Replace(0, sprPath->GetLastPosition(), browseDialog.GetPath());
		}
		else if (event.GetId() == ID_BROWSE_ALP_BUTTON)
		{
			alpPath->Replace(0, alpPath->GetLastPosition(), browseDialog.GetPath());
		}
	}
}

void DatSprOpenSaveDialog::OnClickOpenSaveButton(wxCommandEvent & event)
{
	const wxString & datPathStr = datPath->GetValue();
	const wxString & sprPathStr = sprPath->GetValue();
	const wxString & alpPathStr = alpPath->GetValue();
	bool readOrSaveAlpha = readOrSaveAlphaCheckbox->GetValue();
	Settings & settings = Settings::getInstance();
	if (datPathStr.Length() == 0 || sprPathStr.Length() == 0)
	{
		wxMessageDialog error(this, ".dat / .spr paths cannot be empty", "Error", wxOK | wxICON_ERROR);
		error.ShowModal();
		return;
	}
	if (readOrSaveAlpha && alpPathStr.Length() == 0)
	{
		wxMessageDialog error(this, ".alp path cannot be empty", "Error", wxOK | wxICON_ERROR);
		error.ShowModal();
		return;
	}

	if (mode == MODE_OPEN)
	{
		settings.set("datOpenPath", datPathStr);
		settings.set("sprOpenPath", sprPathStr);
		settings.set("readAlphaValue", readOrSaveAlpha ? "1" : "0");
		if (readOrSaveAlpha)
		{
			settings.set("alpOpenPath", alpPathStr);
		}
		settings.save();
		currentLoading = LOADING_DAT;
		if (DatSprReaderWriter::getInstance().readDat(datPathStr, this))
		{
			currentLoading = LOADING_SPR;
			if (DatSprReaderWriter::getInstance().readSpr(sprPathStr, this))
			{
				if (readOrSaveAlpha)
				{
					currentLoading = LOADING_ALP;
					if (DatSprReaderWriter::getInstance().readAlpha(alpPathStr, this))
					{
						Close();
						// notifying main window of that files has been loaded
						wxCommandEvent event(DAT_SPR_LOADED, 1);
						wxPostEvent(this->m_parent, event);
					}
				}
				else
				{
					Close();
					// notifying main window of that files has been loaded
					wxCommandEvent event(DAT_SPR_LOADED, 1);
					wxPostEvent(this->m_parent, event);
				}
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
		settings.set("saveAlphaValue", readOrSaveAlpha ? "1" : "0");
		if (readOrSaveAlpha)
		{
			settings.set("alpSavePath", alpPathStr);
		}
		settings.save();
		currentLoading = LOADING_DAT;
		if (DatSprReaderWriter::getInstance().writeDat(datPathStr, this))
		{
			currentLoading = LOADING_SPR;
			if (DatSprReaderWriter::getInstance().writeSpr(sprPathStr, this))
			{
				if (readOrSaveAlpha)
				{
					currentLoading = LOADING_ALP;
					if (DatSprReaderWriter::getInstance().writeAlpha(alpPathStr, this))
					{
						Close();
					}
					else
					{
						wxMessageBox("The .alp file cannot be written", "Error", wxOK | wxICON_ERROR);
					}
				}
				else
				{
					Close();
				}
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

void DatSprOpenSaveDialog::OnChangeReadSaveAlphaCheckbox(wxCommandEvent & event)
{
	bool alphaCheckboxValue = event.GetInt() > 0;
	alpLabel->Enable(alphaCheckboxValue);
	alpPath->Enable(alphaCheckboxValue);
	alpButton->Enable(alphaCheckboxValue);
}

void DatSprOpenSaveDialog::updateProgress(double value)
{
	float percentageFactor = readOrSaveAlphaCheckbox->GetValue() ? 33.333 : 50;
	if (currentLoading == LOADING_DAT)
	{
		progress->SetValue(ceil(value * percentageFactor));
	}
	else if (currentLoading == LOADING_SPR)
	{
		progress->SetValue(ceil(percentageFactor + value * percentageFactor));
	}
	else if (currentLoading == LOADING_ALP)
	{
		progress->SetValue(ceil(percentageFactor * 2 + value * percentageFactor));
	}
	wxTheApp->Yield();
}
