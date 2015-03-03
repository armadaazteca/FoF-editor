#ifndef _DAT_SPR_OPEN_DIALOG_H_
#define _DAT_SPR_OPEN_DIALOG_H_

#include "Interfaces.h"

class DatSprOpenSaveDialog : public wxDialog, public ProgressUpdatable
{
public:
	const static unsigned int MODE_OPEN = 0;
	const static unsigned int MODE_SAVE = 1;
	const static unsigned int WIDTH = 500;
	const static wxString DEFAULT_FILENAME;

	DatSprOpenSaveDialog(wxWindow * parent, unsigned int mode);
	void updateProgress(double value);
	inline virtual ~DatSprOpenSaveDialog() {};
private:
	enum ControlIDs
	{
		ID_BROWSE_DAT_BUTTON = 1,
		ID_BROWSE_SPR_BUTTON,
		ID_BROWSE_BLK_BUTTON,
		ID_BROWSE_ALP_BUTTON,
		ID_BROWSE_AOA_BUTTON,
		ID_READ_SAVE_BLOCKING_STATES_CHECKBOX,
		ID_READ_SAVE_ATTRS_CHECKBOX,
		ID_READ_SAVE_ALPHA_CHECKBOX
	};
	void OnClickBrowseButton(wxCommandEvent & event);
	void OnClickOpenSaveButton(wxCommandEvent & event);
	void OnChangeReadSaveBlockingStatesCheckbox(wxCommandEvent & event);
	void OnChangeReadSaveAlphaCheckbox(wxCommandEvent & event);
	void OnChangeReadSaveAttrsCheckbox(wxCommandEvent & event);
	wxDECLARE_EVENT_TABLE();

	unsigned int mode = MODE_OPEN;
	wxTextCtrl * datPath = nullptr, * sprPath = nullptr, * blkPath = nullptr;
	wxTextCtrl * alpPath = nullptr, * aoaPath = nullptr;
	wxStaticText * blkLabel = nullptr, * alpLabel = nullptr, * aoaLabel = nullptr;
	wxButton * blkButton = nullptr, * alpButton = nullptr, * aoaButton = nullptr;
	wxCheckBox * readOrSaveBlockingStatesCheckbox = nullptr, * readOrSaveAlphaCheckbox = nullptr;
	wxCheckBox * readOrSaveAttrsCheckbox = nullptr;
	wxGauge * progress = nullptr;

	int progressStages = 0, currentProgressStage = 0;
	float percentsPerStage = 0;
};

#endif // _DAT_SPR_OPEN_DIALOG_H_
