#ifndef _SPRITES_CLEANUP_DIALOG_H_
#define _SPRITES_CLEANUP_DIALOG_H_

class SpritesCleanupDialog : public wxDialog
{
public:
	enum ButtonIDs { ID_RUN_BUTTON };

	SpritesCleanupDialog(wxWindow * parent);

private:
	void OnClickRunButton(wxCommandEvent & event);
	wxDECLARE_EVENT_TABLE();

	wxGauge * progress = nullptr;
};

#endif // _SPRITES_CLEANUP_DIALOG_H_