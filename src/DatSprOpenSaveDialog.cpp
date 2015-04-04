#include <wx/wxprec.h>
#ifndef WX_PRECOMP
#include <wx/wx.h>
#endif
#include <wx/filename.h>
#include <wx/gbsizer.h>
#include "Config.h"
#include "Events.h"
#include "Settings.h"
#include "DatSprReaderWriter.h"
#include "AdvancedAttributesManager.h"
#include "DatSprOpenSaveDialog.h"

wxBEGIN_EVENT_TABLE(DatSprOpenSaveDialog, wxDialog)
	EVT_BUTTON(ID_BROWSE_DAT_BUTTON, DatSprOpenSaveDialog::OnClickBrowseButton)
	EVT_BUTTON(ID_BROWSE_SPR_BUTTON, DatSprOpenSaveDialog::OnClickBrowseButton)
	EVT_BUTTON(ID_BROWSE_ALP_BUTTON, DatSprOpenSaveDialog::OnClickBrowseButton)
	EVT_BUTTON(ID_BROWSE_AOA_BUTTON, DatSprOpenSaveDialog::OnClickBrowseButton)
	EVT_BUTTON(wxID_OPEN, DatSprOpenSaveDialog::OnClickOpenSaveButton)
	EVT_BUTTON(wxID_SAVE, DatSprOpenSaveDialog::OnClickOpenSaveButton)
	EVT_CHECKBOX(ID_READ_SAVE_BLOCKING_STATES_CHECKBOX, DatSprOpenSaveDialog::OnChangeReadSaveBlockingStatesCheckbox)
	EVT_CHECKBOX(ID_READ_SAVE_ALPHA_CHECKBOX, DatSprOpenSaveDialog::OnChangeReadSaveAlphaCheckbox)
	EVT_CHECKBOX(ID_READ_SAVE_ATTRS_CHECKBOX, DatSprOpenSaveDialog::OnChangeReadSaveAttrsCheckbox)
wxEND_EVENT_TABLE()

const wxString DatSprOpenSaveDialog::DEFAULT_FILENAME = "Tibia";

DatSprOpenSaveDialog::DatSprOpenSaveDialog(wxWindow * parent, unsigned int mode)
	: wxDialog(parent, -1, wxString::Format("%s .dat / .spr / .alp / .aoa files", mode == MODE_OPEN ? "Open" : "Save"))
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
	datPath = new wxTextCtrl(panel, -1, datPathStr, wxDefaultPosition, wxSize(300, -1));
	auto datButton = new wxButton(panel, ID_BROWSE_DAT_BUTTON, browse);

	auto sprLabel = new wxStaticText(panel, -1, wxT(".spr file:"));
	const wxString & sprPathStr = (mode == MODE_OPEN ? settings.get("sprOpenPath") : settings.get("sprSavePath"));
	sprPath = new wxTextCtrl(panel, -1, sprPathStr);
	auto sprButton = new wxButton(panel, ID_BROWSE_SPR_BUTTON, browse);

	bool blockingStatesCheckboxValue = (mode == MODE_OPEN ? settings.get("readBlockingStates")
	                                                      : settings.get("saveBlockingStates")).IsSameAs("yes");
	readOrSaveBlockingStatesCheckbox = new wxCheckBox(panel, ID_READ_SAVE_BLOCKING_STATES_CHECKBOX,
	    wxString::Format("%s sprites 'blocking' states", (mode == MODE_OPEN ? "Read" : "Save")));
	readOrSaveBlockingStatesCheckbox->SetValue(blockingStatesCheckboxValue);

	blkLabel = new wxStaticText(panel, -1, wxT(".blk file:"));
	const wxString & blkPathStr = (mode == MODE_OPEN ? settings.get("blkOpenPath") : settings.get("blkSavePath"));
	blkPath = new wxTextCtrl(panel, -1, blkPathStr);
	blkButton = new wxButton(panel, ID_BROWSE_BLK_BUTTON, browse);
	blkLabel->Enable(blockingStatesCheckboxValue);
	blkPath->Enable(blockingStatesCheckboxValue);
	blkButton->Enable(blockingStatesCheckboxValue);

	bool alphaCheckboxValue = (mode == MODE_OPEN ? settings.get("readAlpha")
	                                             : settings.get("saveAlpha")).IsSameAs("yes");
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

	bool attsCheckboxValue = (mode == MODE_OPEN ? settings.get("readAdvancedAttrs")
	                                            : settings.get("saveAdvancedAttrs")).IsSameAs("yes");
	readOrSaveAttrsCheckbox = new wxCheckBox(panel, ID_READ_SAVE_ATTRS_CHECKBOX,
			wxString::Format("%s advanced object attributes", (mode == MODE_OPEN ? "Read" : "Save")));
	readOrSaveAttrsCheckbox->SetValue(attsCheckboxValue);

	aoaLabel = new wxStaticText(panel, -1, wxT(".aoa file:"));
	const wxString & aoaPathStr = (mode == MODE_OPEN ? settings.get("aoaOpenPath") : settings.get("aoaSavePath"));
	aoaPath = new wxTextCtrl(panel, -1, aoaPathStr);
	aoaButton = new wxButton(panel, ID_BROWSE_AOA_BUTTON, browse);
	aoaLabel->Enable(attsCheckboxValue);
	aoaPath->Enable(attsCheckboxValue);
	aoaButton->Enable(attsCheckboxValue);

	progress = new wxGauge(panel, -1, 100);
	auto openSaveButton = (mode == MODE_OPEN ? new wxButton(panel, wxID_OPEN, "Open") : new wxButton(panel, wxID_SAVE, "Save"));
	openSaveButton->SetFocus();

	int currentRow = 0;
	gbs->Add(datLabel, wxGBPosition(currentRow, 0), wxDefaultSpan, wxALIGN_CENTER);
	gbs->Add(datPath, wxGBPosition(currentRow, 1), wxDefaultSpan, wxEXPAND);
	gbs->Add(datButton, wxGBPosition(currentRow, 2), wxDefaultSpan);
	currentRow++;

	gbs->Add(sprLabel, wxGBPosition(currentRow, 0), wxDefaultSpan, wxALIGN_CENTER);
	gbs->Add(sprPath, wxGBPosition(currentRow, 1), wxDefaultSpan, wxEXPAND);
	gbs->Add(sprButton, wxGBPosition(currentRow, 2), wxDefaultSpan);
	currentRow++;

	gbs->Add(readOrSaveBlockingStatesCheckbox, wxGBPosition(currentRow++, 0), wxGBSpan(1, 3), wxEXPAND);

	gbs->Add(blkLabel, wxGBPosition(currentRow, 0), wxDefaultSpan, wxALIGN_CENTER);
	gbs->Add(blkPath, wxGBPosition(currentRow, 1), wxDefaultSpan, wxEXPAND);
	gbs->Add(blkButton, wxGBPosition(currentRow, 2), wxDefaultSpan);
	currentRow++;

	gbs->Add(readOrSaveAlphaCheckbox, wxGBPosition(currentRow++, 0), wxGBSpan(1, 3), wxEXPAND);

	gbs->Add(alpLabel, wxGBPosition(currentRow, 0), wxDefaultSpan, wxALIGN_CENTER);
	gbs->Add(alpPath, wxGBPosition(currentRow, 1), wxDefaultSpan, wxEXPAND);
	gbs->Add(alpButton, wxGBPosition(currentRow, 2), wxDefaultSpan);
	currentRow++;

	gbs->Add(readOrSaveAttrsCheckbox, wxGBPosition(currentRow++, 0), wxGBSpan(1, 3), wxEXPAND);

	gbs->Add(aoaLabel, wxGBPosition(currentRow, 0), wxDefaultSpan, wxALIGN_CENTER);
	gbs->Add(aoaPath, wxGBPosition(currentRow, 1), wxDefaultSpan, wxEXPAND);
	gbs->Add(aoaButton, wxGBPosition(currentRow, 2), wxDefaultSpan);
	currentRow++;

	gbs->AddGrowableCol(1, 1);
	gbs->Add(progress, wxGBPosition(currentRow++, 0), wxGBSpan(1, 3), wxEXPAND);
	gbs->Add(openSaveButton, wxGBPosition(currentRow, 2));

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
	const wxString & blkPathStr = (mode == MODE_OPEN ? settings.get("blkOpenPath") : settings.get("blkSavePath"));
	const wxString & alpPathStr = (mode == MODE_OPEN ? settings.get("alpOpenPath") : settings.get("alpSavePath"));
	const wxString & aoaPathStr = (mode == MODE_OPEN ? settings.get("aoaOpenPath") : settings.get("aoaSavePath"));
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
	else if (event.GetId() == ID_BROWSE_BLK_BUTTON)
	{
		browseDir = wxFileName(blkPathStr).GetPath();
		caption = "Select .blk file";
		defaultFilename = (mode == MODE_OPEN ? "" : DEFAULT_FILENAME + ".blk");
		filter = "*.blk|*.blk|All files|*";
	}
	else if (event.GetId() == ID_BROWSE_ALP_BUTTON)
	{
		browseDir = wxFileName(alpPathStr).GetPath();
		caption = "Select .alp file";
		defaultFilename = (mode == MODE_OPEN ? "" : DEFAULT_FILENAME + ".alp");
		filter = "*.alp|*.alp|All files|*";
	}
	else if (event.GetId() == ID_BROWSE_AOA_BUTTON)
	{
		browseDir = wxFileName(aoaPathStr).GetPath();
		caption = "Select .aoa file";
		defaultFilename = (mode == MODE_OPEN ? "" : DEFAULT_FILENAME + ".aoa");
		filter = "*.aoa|*.aoa|All files|*";
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
		else if (event.GetId() == ID_BROWSE_BLK_BUTTON)
		{
			blkPath->Replace(0, blkPath->GetLastPosition(), browseDialog.GetPath());
		}
		else if (event.GetId() == ID_BROWSE_ALP_BUTTON)
		{
			alpPath->Replace(0, alpPath->GetLastPosition(), browseDialog.GetPath());
		}
		else if (event.GetId() == ID_BROWSE_AOA_BUTTON)
		{
			aoaPath->Replace(0, aoaPath->GetLastPosition(), browseDialog.GetPath());
		}
	}
}

void DatSprOpenSaveDialog::OnClickOpenSaveButton(wxCommandEvent & event)
{
	const wxString & datPathStr = datPath->GetValue();
	const wxString & sprPathStr = sprPath->GetValue();
	const wxString & blkPathStr = blkPath->GetValue();
	const wxString & alpPathStr = alpPath->GetValue();
	const wxString & aoaPathStr = aoaPath->GetValue();
	bool readOrSaveBlockingStates = readOrSaveBlockingStatesCheckbox->GetValue();
	bool readOrSaveAlpha = readOrSaveAlphaCheckbox->GetValue();
	bool readOrSaveAttrs = readOrSaveAttrsCheckbox->GetValue();
	Settings & settings = Settings::getInstance();
	const wxString & emptyErrorMsg = "%s path cannot be empty";
	if (datPathStr.Length() == 0 || sprPathStr.Length() == 0)
	{
		wxMessageBox(".dat / .spr paths cannot be empty", Config::ERROR_TITLE, wxOK | wxICON_ERROR);
		return;
	}
	if (readOrSaveBlockingStates && blkPathStr.Length() == 0)
	{
		wxMessageBox(wxString::Format(emptyErrorMsg, ".blk"), Config::ERROR_TITLE, wxOK | wxICON_ERROR);
		return;
	}
	if (readOrSaveAlpha && alpPathStr.Length() == 0)
	{
		wxMessageBox(wxString::Format(emptyErrorMsg, ".alp"), Config::ERROR_TITLE, wxOK | wxICON_ERROR);
		return;
	}
	if (readOrSaveAttrs && aoaPathStr.Length() == 0)
	{
		wxMessageBox(wxString::Format(emptyErrorMsg, ".aoa"), Config::ERROR_TITLE, wxOK | wxICON_ERROR);
		return;
	}

	progressStages = 2;
	if (readOrSaveBlockingStates) progressStages++;
	if (readOrSaveAlpha) progressStages++;
	if (readOrSaveAttrs) progressStages++;
	currentProgressStage = 0;
	percentsPerStage = 100 / (float) progressStages;

	auto closeAndNotify = [&]()
	{
		Close();
		// notifying main window that files have been loaded
		wxCommandEvent event(mode == MODE_OPEN ? DAT_SPR_LOADED : DAT_SPR_SAVED, 1);
		wxPostEvent(m_parent, event);
	};

	auto & dsrw = DatSprReaderWriter::getInstance();
	if (mode == MODE_OPEN)
	{
		settings.set("datOpenPath", datPathStr);
		settings.set("sprOpenPath", sprPathStr);
		settings.set("readBlockingStates", readOrSaveBlockingStates ? "yes" : "no");
		settings.set("readAdvancedAttrs", readOrSaveAttrs ? "yes" : "no");
		settings.set("readAlpha", readOrSaveAlpha ? "yes" : "no");
		if (readOrSaveBlockingStates)
		{
			settings.set("blkOpenPath", blkPathStr);
		}
		if (readOrSaveAttrs)
		{
			settings.set("aoaOpenPath", aoaPathStr);
		}
		if (readOrSaveAlpha)
		{
			settings.set("alpOpenPath", alpPathStr);
		}
		settings.save();
		if (dsrw.readDat(datPathStr, this))
		{
			currentProgressStage++;
			if (dsrw.readSpr(sprPathStr, this))
			{
				if (readOrSaveBlockingStates)
				{
					currentProgressStage++;
					if (!dsrw.readBlockingStates(blkPathStr, this))
					{
						switch (dsrw.getLastError())
						{
							case DatSprReaderWriter::ERROR_UNKNOWN:
								wxMessageBox(wxString::Format(Config::COMMON_READ_ERROR, ".blk"), Config::ERROR_TITLE,
								             wxOK | wxICON_ERROR);
							break;
							case DatSprReaderWriter::ERROR_INVALID_SIGNATURE:
								wxMessageBox(wxString::Format(Config::INVALID_SIGNATURE_ERROR, ".blk",
								             DatSprReaderWriter::DEFAULT_SPR_SIGNATURE), Config::ERROR_TITLE, wxOK | wxICON_ERROR);
							break;
						}
						progress->SetValue(0);
					}
				}

				if (readOrSaveAlpha)
				{
					currentProgressStage++;
					if (!dsrw.readAlpha(alpPathStr, this))
					{
						switch (dsrw.getLastError())
						{
							case DatSprReaderWriter::ERROR_UNKNOWN:
								wxMessageBox(wxString::Format(Config::COMMON_READ_ERROR, ".alp"), Config::ERROR_TITLE,
								             wxOK | wxICON_ERROR);
							break;
							case DatSprReaderWriter::ERROR_INVALID_SIGNATURE:
								wxMessageBox(wxString::Format(Config::INVALID_SIGNATURE_ERROR, ".alp",
								             DatSprReaderWriter::DEFAULT_SPR_SIGNATURE), Config::ERROR_TITLE, wxOK | wxICON_ERROR);
							break;
						}
						progress->SetValue(0);
					}
				}

				if (readOrSaveAttrs)
				{
					currentProgressStage++;
					if (!AdvancedAttributesManager::getInstance().read(aoaPathStr, this))
					{
						wxMessageBox(wxString::Format(Config::COMMON_READ_ERROR, ".aoa"), Config::ERROR_TITLE,
						             wxOK | wxICON_ERROR);
						progress->SetValue(0);
					}
				}

				closeAndNotify();
			}
			else
			{
				switch (dsrw.getLastError())
				{
					case DatSprReaderWriter::ERROR_UNKNOWN:
						wxMessageBox(wxString::Format(Config::COMMON_READ_ERROR, ".spr"), Config::ERROR_TITLE,
						             wxOK | wxICON_ERROR);
					break;
					case DatSprReaderWriter::ERROR_INVALID_SIGNATURE:
						wxMessageBox(wxString::Format(Config::INVALID_SIGNATURE_ERROR, ".spr",
						             DatSprReaderWriter::DEFAULT_SPR_SIGNATURE), Config::ERROR_TITLE, wxOK | wxICON_ERROR);
					break;
				}
				progress->SetValue(0);
			}
		}
		else
		{
			switch (dsrw.getLastError())
			{
				case DatSprReaderWriter::ERROR_UNKNOWN:
					wxMessageBox(wxString::Format(Config::COMMON_READ_ERROR, ".dat"), Config::ERROR_TITLE,
					             wxOK | wxICON_ERROR);
				break;
				case DatSprReaderWriter::ERROR_INVALID_SIGNATURE:
					wxMessageBox(wxString::Format(Config::INVALID_SIGNATURE_ERROR, ".dat",
					             DatSprReaderWriter::DEFAULT_DAT_SIGNATURE), Config::ERROR_TITLE, wxOK | wxICON_ERROR);
				break;
			}
			progress->SetValue(0);
		}
	}
	else if (mode == MODE_SAVE)
	{
		settings.set("datSavePath", datPathStr);
		settings.set("sprSavePath", sprPathStr);
		settings.set("saveBlockingStates", readOrSaveBlockingStates ? "yes" : "no");
		settings.set("saveAdvancedAttrs", readOrSaveAttrs ? "yes" : "no");
		settings.set("saveAlpha", readOrSaveAlpha ? "yes" : "no");
		if (readOrSaveBlockingStates)
		{
			settings.set("blkSavePath", blkPathStr);
		}
		if (readOrSaveAttrs)
		{
			settings.set("aoaSavePath", aoaPathStr);
		}
		if (readOrSaveAlpha)
		{
			settings.set("alpSavePath", alpPathStr);
		}
		settings.save();
		if (dsrw.writeDat(datPathStr, this))
		{
			currentProgressStage++;
			if (dsrw.writeSpr(sprPathStr, this))
			{
				if (readOrSaveBlockingStates)
				{
					currentProgressStage++;
					if (!dsrw.writeBlockingStates(blkPathStr, this))
					{
						wxMessageBox(wxString::Format(Config::COMMON_WRITE_ERROR, ".blk"), Config::ERROR_TITLE,
												 wxOK | wxICON_ERROR);
						progress->SetValue(0);
					}
				}

				if (readOrSaveAlpha)
				{
					currentProgressStage++;
					if (!dsrw.writeAlpha(alpPathStr, this))
					{
						wxMessageBox(wxString::Format(Config::COMMON_WRITE_ERROR, ".alp"), Config::ERROR_TITLE,
						             wxOK | wxICON_ERROR);
						progress->SetValue(0);
					}
				}

				if (readOrSaveAttrs)
				{
					currentProgressStage++;
					if (!AdvancedAttributesManager::getInstance().save(aoaPathStr, this))
					{
						wxMessageBox(wxString::Format(Config::COMMON_WRITE_ERROR, ".aoa"), Config::ERROR_TITLE,
						             wxOK | wxICON_ERROR);
						progress->SetValue(0);
					}
				}

				closeAndNotify();
			}
			else
			{
				wxMessageBox(wxString::Format(Config::COMMON_WRITE_ERROR, ".spr"), Config::ERROR_TITLE,
				             wxOK | wxICON_ERROR);
				progress->SetValue(0);
			}
		}
		else
		{
			wxMessageBox(wxString::Format(Config::COMMON_WRITE_ERROR, ".dat"), Config::ERROR_TITLE,
			             wxOK | wxICON_ERROR);
			progress->SetValue(0);
		}
	}
}

void DatSprOpenSaveDialog::OnChangeReadSaveBlockingStatesCheckbox(wxCommandEvent & event)
{
	bool blockingStatesCheckBoxValue = event.GetInt() > 0;
	blkLabel->Enable(blockingStatesCheckBoxValue);
	blkPath->Enable(blockingStatesCheckBoxValue);
	blkButton->Enable(blockingStatesCheckBoxValue);
}

void DatSprOpenSaveDialog::OnChangeReadSaveAttrsCheckbox(wxCommandEvent & event)
{
	bool attrsCheckBoxValue = event.GetInt() > 0;
	aoaLabel->Enable(attrsCheckBoxValue);
	aoaPath->Enable(attrsCheckBoxValue);
	aoaButton->Enable(attrsCheckBoxValue);
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
	progress->SetValue(ceil(percentsPerStage * currentProgressStage + percentsPerStage * value));
	wxTheApp->Yield();
}
