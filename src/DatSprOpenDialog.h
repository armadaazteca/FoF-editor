#ifndef _DAT_SPR_OPEN_DIALOG_H_
#define _DAT_SPR_OPEN_DIALOG_H_

#include "Interfaces.h"

class DatSprOpenDialog : public wxDialog, public ProgressUpdatable
{
public:
	const static unsigned int WIDTH = 400;
	const static unsigned int HEIGHT = 150;
	DatSprOpenDialog(wxWindow * parent);
	void updateProgress(double value);
	virtual ~DatSprOpenDialog();
private:
	enum MenuItemIds
	{
		ID_BROWSE_DAT_BUTTON = 1,
		ID_BROWSE_SPR_BUTTON = 2,
		ID_OPEN = 3
	};
	void OnClickBrowseButton(wxCommandEvent & event);
	void OnClickOpenButton(wxCommandEvent & event);
	wxDECLARE_EVENT_TABLE();

	wxTextCtrl * datPath = nullptr, * sprPath = nullptr;
	wxGauge * progress = nullptr;

	enum Loading { LOADING_DAT, LOADING_SPR };
	Loading currentLoading = LOADING_DAT;
};

#endif // _DAT_SPR_OPEN_DIALOG_H_
