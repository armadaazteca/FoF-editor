#ifndef _DAT_SPR_OPEN_DIALOG_H_
#define _DAT_SPR_OPEN_DIALOG_H_

#include "Interfaces.h"

class DatSprOpenSaveDialog : public wxDialog, public ProgressUpdatable
{
public:
	const static unsigned int MODE_OPEN = 0;
	const static unsigned int MODE_SAVE = 1;
	const static unsigned int WIDTH = 500;
	const static unsigned int HEIGHT = 150;
	const static wxString DEFAULT_FILENAME;

	DatSprOpenSaveDialog(wxWindow * parent, unsigned int mode);
	void updateProgress(double value);
	inline virtual ~DatSprOpenSaveDialog() {};
private:
	enum ButtonIDs
	{
		ID_BROWSE_DAT_BUTTON = 1,
		ID_BROWSE_SPR_BUTTON = 2,
		ID_BROWSE_ALP_BUTTON = 3,
		ID_READ_SAVE_ALPHA_CHECKBOX = 4,
		ID_OPEN = 5,
		ID_SAVE = 6
	};
	void OnClickBrowseButton(wxCommandEvent & event);
	void OnClickOpenSaveButton(wxCommandEvent & event);
	void OnChangeReadSaveAlphaCheckbox(wxCommandEvent & event);
	wxDECLARE_EVENT_TABLE();

	unsigned int mode = MODE_OPEN;
	wxTextCtrl * datPath = nullptr, * sprPath = nullptr, * alpPath = nullptr;
	wxStaticText * alpLabel = nullptr;
	wxButton * alpButton = nullptr;
	wxCheckBox * readOrSaveAlphaCheckbox = nullptr;
	wxGauge * progress = nullptr;

	enum Loading { LOADING_DAT, LOADING_SPR, LOADING_ALP };
	Loading currentLoading = LOADING_DAT;
};

#endif // _DAT_SPR_OPEN_DIALOG_H_
