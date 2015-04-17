#include <wx/wxprec.h>
#ifndef WX_PRECOMP
#include <wx/wx.h>
#endif
#include "Config.h"
#include "Settings.h"
#include "ExportAnimationDialog.h"

wxBEGIN_EVENT_TABLE(ExportAnimationDialog, wxDialog)
	EVT_SPINCTRL(ID_FRAME_FROM, ExportAnimationDialog::OnFrameFromOrFrameToChanged)
	EVT_SPINCTRL(ID_FRAME_TO, ExportAnimationDialog::OnFrameFromOrFrameToChanged)
	EVT_BUTTON(wxID_OK, ExportAnimationDialog::OnClickExportButton)
wxEND_EVENT_TABLE()

ExportAnimationDialog::ExportAnimationDialog(wxWindow * parent, unsigned int startFrame, unsigned int endFrame)
	: wxDialog(parent, wxID_ANY, "Export animation settings"), startFrame(startFrame), endFrame(endFrame)
{
	auto wrapVBox = new wxBoxSizer(wxVERTICAL), vbox = new wxBoxSizer(wxVERTICAL);

	auto colorPickerSizer = new wxBoxSizer(wxHORIZONTAL);
	auto animBgColorLabel = new wxStaticText(this, wxID_ANY, wxT("Animation background color:"));
	colorPickerSizer->Add(animBgColorLabel, 0, wxALL | wxALIGN_CENTER_VERTICAL, 5);
	wxColour color(*wxBLACK);
	const wxString & savedColor = Settings::getInstance().get("exportAnimationBgColor");
	if (savedColor.Length() > 0)
	{
		color.Set(savedColor);
	}
	animBgColor = new wxColourPickerCtrl(this, wxID_ANY, color);
	colorPickerSizer->Add(animBgColor, 0, wxALL, 5);
	vbox->Add(colorPickerSizer);

	auto framesIntervalLabel = new wxStaticText(this, wxID_ANY, wxT("Frames interval:"));
	vbox->Add(framesIntervalLabel, 0, wxALL, 5);

	auto framesFromToSizer = new wxBoxSizer(wxHORIZONTAL);
	auto fromLabel = new wxStaticText(this, wxID_ANY, wxT("From"));
	framesFromToSizer->Add(fromLabel, 0, wxALL | wxALIGN_CENTER_VERTICAL, 5);

	frameFrom = new wxSpinCtrl(this, ID_FRAME_FROM, wxString::Format("%i", startFrame), wxDefaultPosition,
	                           wxSize(Config::COMMON_NUM_FIELD_WIDTH, -1), wxTE_RIGHT);
	frameFrom->SetRange(startFrame, endFrame - 1);
	framesFromToSizer->Add(frameFrom, 0, wxALL, 5);

	auto toLabel = new wxStaticText(this, wxID_ANY, wxT("to"));
	framesFromToSizer->Add(toLabel, 0, wxALL | wxALIGN_CENTER_VERTICAL, 5);

	frameTo = new wxSpinCtrl(this, ID_FRAME_TO, wxString::Format("%i", endFrame), wxDefaultPosition,
	                         wxSize(Config::COMMON_NUM_FIELD_WIDTH, -1), wxTE_RIGHT);
	frameTo->SetRange(startFrame + 1, endFrame);
	framesFromToSizer->Add(frameTo, 0, wxALL, 5);

	vbox->Add(framesFromToSizer);

	auto buttonsSizer = new wxBoxSizer(wxHORIZONTAL);
	auto cancelButton = new wxButton(this, wxID_CANCEL, "Cancel");
	buttonsSizer->Add(cancelButton, 0, wxALL, 5);
	auto exportButton = new wxButton(this, wxID_OK, "Export");
	exportButton->SetDefault();
	exportButton->SetFocus();
	buttonsSizer->Add(exportButton, 0, wxALL, 5);

	vbox->Add(buttonsSizer, 0, wxALIGN_RIGHT);

	wrapVBox->Add(vbox, 0, wxALL, 5);

	SetSizer(wrapVBox);
	Fit();
	Center();
}

void ExportAnimationDialog::OnFrameFromOrFrameToChanged(wxSpinEvent & event)
{
	// adjusting ranges in a way that 'from' is always <= 'to' and 'to' is always >= 'from'
	frameFrom->SetRange(startFrame, frameTo->GetValue());
	frameTo->SetRange(frameFrom->GetValue(), endFrame);
}

void ExportAnimationDialog::OnClickExportButton(wxCommandEvent & event)
{
	animBgColorValue = animBgColor->GetColour();
	frameFromValue = frameFrom->GetValue();
	frameToValue = frameTo->GetValue();

	Settings & settings = Settings::getInstance();
	settings.set("exportAnimationBgColor", animBgColorValue.GetAsString(wxC2S_CSS_SYNTAX));
	settings.save();

	EndModal(wxID_OK);
}
