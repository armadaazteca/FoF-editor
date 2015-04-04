#include <wx/wxprec.h>
#ifndef WX_PRECOMP
#include <wx/wx.h>
#endif
#include <wx/gbsizer.h>
#include <wx/spinctrl.h>
#include "Config.h"
#include "Events.h"
#include "Settings.h"
#include "PreferencesDialog.h"

wxBEGIN_EVENT_TABLE(PreferencesDialog, wxDialog)
	EVT_CHECKBOX(ID_AUTO_BACKUP_CHECKBOX, PreferencesDialog::OnChangeAutoBackupCheckbox)
	EVT_BUTTON(wxID_SAVE, PreferencesDialog::OnClickSaveButton)
wxEND_EVENT_TABLE()

PreferencesDialog::PreferencesDialog(wxWindow * parent)
	: wxDialog(parent, wxID_ANY, "Preferences")
{
	auto vbox = new wxBoxSizer(wxVERTICAL);
	auto gbs = new wxGridBagSizer(5, 5);
	Settings & settings = Settings::getInstance();

	bool validateSignaturesCheckboxValue = settings.get("validateSignatures").IsSameAs("yes");
	validateSignaturesCheckbox = new wxCheckBox(this, ID_VALIDATE_SIGNATURES_CHECKBOX, wxT("Validate signatures"));
	validateSignaturesCheckbox->SetValue(validateSignaturesCheckboxValue);
	const wchar_t * hint = wxT("When signatures validation is turned on, all loaded files are validated to have signature "
	                    "corresponding to Tibia 9.60 format");
	auto validateSignaturesHint = new wxStaticText(this, wxID_ANY, hint, wxDefaultPosition, wxSize(300, 50));
	validateSignaturesHint->SetFont(validateSignaturesHint->GetFont().Italic());

	bool autoBackupCheckboxValue = settings.get("autoBackupEnabled").IsSameAs("yes");
	autoBackupCheckbox = new wxCheckBox(this, ID_AUTO_BACKUP_CHECKBOX, wxT("Enable auto-backup"));
	autoBackupCheckbox->SetValue(autoBackupCheckboxValue);
	backupIntervalLabel = new wxStaticText(this, wxID_ANY, wxT("Interval:"));
	const wxString & autoBackupIntervalValue = settings.get("autoBackupInterval");
	backupIntervalInput = new wxSpinCtrl(this, wxID_ANY, autoBackupIntervalValue, wxDefaultPosition,
	                                 wxSize(Config::COMMON_NUM_FIELD_WIDTH, -1), wxTE_RIGHT);
	backupIntervalInput->SetRange(1, Config::MAX_AUTOBACKUP_INTERVAL);
	backupIntervalMinutesLabel = new wxStaticText(this, wxID_ANY, wxT("minutes"));
	if (!autoBackupCheckboxValue)
	{
		backupIntervalLabel->Disable();
		backupIntervalInput->Disable();
		backupIntervalMinutesLabel->Disable();
	}

	auto cancelButton = new wxButton(this, wxID_CANCEL);
	auto saveButton = new wxButton(this, wxID_SAVE);
	saveButton->SetFocus();

	int currentRow = 0;
	gbs->Add(validateSignaturesCheckbox, wxGBPosition(currentRow++, 0), wxGBSpan(1, 3));
	gbs->Add(validateSignaturesHint, wxGBPosition(currentRow++, 0), wxGBSpan(1, 3));

	gbs->Add(autoBackupCheckbox, wxGBPosition(currentRow++, 0), wxGBSpan(1, 3));

	gbs->Add(backupIntervalLabel, wxGBPosition(currentRow, 0), wxDefaultSpan, wxALIGN_CENTER);
	gbs->Add(backupIntervalInput, wxGBPosition(currentRow, 1), wxDefaultSpan, wxEXPAND);
	gbs->Add(backupIntervalMinutesLabel, wxGBPosition(currentRow, 2), wxDefaultSpan, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL);
	currentRow++;

	auto buttonsHbox = new wxBoxSizer(wxHORIZONTAL);
	buttonsHbox->Add(cancelButton, 0, wxRIGHT, 5);
	buttonsHbox->Add(saveButton);
	gbs->Add(buttonsHbox, wxGBPosition(currentRow++, 0), wxGBSpan(1, 3), wxALIGN_RIGHT);

	vbox->Add(gbs, 1, wxALL | wxEXPAND, 10);
	SetSizer(vbox);
	Fit();
	Center();
}

void PreferencesDialog::OnChangeAutoBackupCheckbox(wxCommandEvent & event)
{
	bool autoBackupCheckboxValue = event.GetInt() > 0;
	backupIntervalLabel->Enable(autoBackupCheckboxValue);
	backupIntervalInput->Enable(autoBackupCheckboxValue);
	backupIntervalMinutesLabel->Enable(autoBackupCheckboxValue);
}

void PreferencesDialog::OnClickSaveButton(wxCommandEvent & event)
{
	Settings & settings = Settings::getInstance();
	settings.set("validateSignatures", validateSignaturesCheckbox->GetValue() ? "yes" : "no");
	settings.set("autoBackupEnabled", autoBackupCheckbox->GetValue() ? "yes" : "no");
	settings.set("autoBackupInterval", wxString::Format("%i", backupIntervalInput->GetValue()));
	settings.save();
	Close();
	wxCommandEvent evt(PREFERENCES_SAVED, 1);
	wxPostEvent(m_parent, evt);
}
