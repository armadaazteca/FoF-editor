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
	virtual ~DatSprOpenSaveDialog();
private:
	enum MenuItemIds
	{
		ID_BROWSE_DAT_BUTTON = 1,
		ID_BROWSE_SPR_BUTTON = 2,
		ID_OPEN = 3,
		ID_SAVE = 4
	};
	void OnClickBrowseButton(wxCommandEvent & event);
	void OnClickOpenSaveButton(wxCommandEvent & event);
	wxDECLARE_EVENT_TABLE();

	unsigned int mode = MODE_OPEN;
	wxTextCtrl * datPath = nullptr, * sprPath = nullptr;
	wxGauge * progress = nullptr;

	enum Loading { LOADING_DAT, LOADING_SPR };
	Loading currentLoading = LOADING_DAT;
};

#endif // _DAT_SPR_OPEN_DIALOG_H_
