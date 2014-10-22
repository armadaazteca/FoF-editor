#ifndef _GENERATE_RME_DIALOG_H_
#define _GENERATE_RME_DIALOG_H_

#include "Interfaces.h"

/**
 * This class represents dialog from which RME-specific resources will be generated based on editor's data
 */
class GenerateRMEDialog : public wxDialog, public ProgressUpdatable
{
public:
	enum ButtonIDs { ID_BROWSE_BUTTON, ID_GENERATE_BUTTON };
	enum CurrentWriting { WRITING_ITEMS_OTB, WRITING_ITEMS_XML, WRITING_CREATURES_XML };

	GenerateRMEDialog(wxWindow * parent);
	void updateProgress(double value);
	inline virtual ~GenerateRMEDialog() {}

private:
	void OnClickBrowseButton(wxCommandEvent & event);
	void OnClickGenerateButton(wxCommandEvent & event);
	wxDECLARE_EVENT_TABLE();

	wxTextCtrl * dataPath = nullptr;
	wxCheckBox * overwriteCb = nullptr;
	wxGauge * progress = nullptr;
	CurrentWriting currentWriting = WRITING_ITEMS_OTB;
};

#endif // _GENERATE_RME_DIALOG_H_
