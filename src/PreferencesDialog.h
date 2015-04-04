#ifndef _PREFERENCES_DIALOG_H_
#define _PREFERENCES_DIALOG_H_

class PreferencesDialog : public wxDialog
{
public:
	PreferencesDialog(wxWindow * parent);

private:
	enum ControlIDs
	{
		ID_AUTO_BACKUP_CHECKBOX = 1,
		ID_VALIDATE_SIGNATURES_CHECKBOX
	};

	void OnChangeAutoBackupCheckbox(wxCommandEvent & event);
	void OnClickSaveButton(wxCommandEvent & event);
	wxDECLARE_EVENT_TABLE();

	wxCheckBox * autoBackupCheckbox = nullptr, * validateSignaturesCheckbox = nullptr;
	wxStaticText * backupIntervalLabel = nullptr, * backupIntervalMinutesLabel = nullptr;
	wxSpinCtrl * backupIntervalInput = nullptr;
};

#endif // _PREFERENCES_DIALOG_H_
