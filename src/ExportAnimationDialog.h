#ifndef _EXPORT_ANIMATION_DIALOG_H_
#define _EXPORT_ANIMATION_DIALOG_H_

#include <wx/clrpicker.h>
#include <wx/spinctrl.h>

class ExportAnimationDialog : public wxDialog
{
public:
	enum ControlIDs
	{
		ID_FRAME_FROM = 1,
		ID_FRAME_TO
	};

	ExportAnimationDialog(wxWindow * parent, unsigned int startFrame, unsigned int endFrame);
	inline const wxColour getAnimBgColor() { return animBgColorValue; }
	inline int getFrameFrom() { return frameFromValue; }
	inline int getFrameTo() { return frameToValue; }

private:
	void OnClickExportButton(wxCommandEvent & event);
	void OnFrameFromOrFrameToChanged(wxSpinEvent & event);
	wxDECLARE_EVENT_TABLE();

	unsigned int startFrame = 0, endFrame = 0;
	wxColourPickerCtrl * animBgColor = nullptr;
	wxSpinCtrl * frameFrom = nullptr, * frameTo = nullptr;
	unsigned int frameFromValue = 0, frameToValue = 0;
	wxColour animBgColorValue;
};

#endif // _EXPORT_ANIMATION_DIALOG_H_
