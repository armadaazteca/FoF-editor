#ifndef _GENERATE_RME_DIALOG_H_
#define _GENERATE_RME_DIALOG_H_

/**
 * This class represents dialog from which RME-specific resources will be generated based on editor's data
 */
class GenerateRMEDialog : public wxDialog, public ProgressUpdatable
{
public:
	enum ButtonIDs { ID_BROWSE_BUTTON, ID_GENERATE_BUTTON };

	GenerateRMEDialog(wxWindow * parent);
	void updateProgress(double value);
	inline virtual ~GenerateRMEDialog() {};

private:
	wxTextCtrl * dataPath = nullptr;
	wxGauge * progress = nullptr;
};

#endif // _GENERATE_RME_DIALOG_H_
