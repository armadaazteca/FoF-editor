#include <wx/wxprec.h>
#ifndef WX_PRECOMP
#include <wx/wx.h>
#endif
#include <wx/filename.h>
#include "jsoncpp/json.h"
#include "Settings.h"
#include "ItemsOtbWriter.h"
#include "GenerateRMEDialog.h"

GenerateRMEDialog::GenerateRMEDialog(wxWindow * parent)
	: wxDialog(parent, -1, "Generate RME resources")
{
	auto vbox = new wxBoxSizer(wxVERTICAL);
	Settings & settings = Settings::getInstance();

	const wchar_t * toolDesc = wxT(
		"This tool will generate specific resources for Remere's Map Editor, such as:\n"
		"items.otb, items.xml, creatures.xml, materials.xml.\n"
		"Generation is based on data from .dat-file and on advanced object attributes, stored in .aoa-file. "
		"Generation is based on v9.60 formats, so path to files should be like \"<RME_dir>/data/960\"."
	);
	auto toolDescLabel = new wxStaticText(this, -1, toolDesc, wxDefaultPosition, wxSize(450, 105));
	vbox->Add(toolDescLabel, 0, wxALL, 5);

	auto hbox = new wxBoxSizer(wxHORIZONTAL);

	auto pathLabel = new wxStaticText(this, -1, wxT("Path:"));
	hbox->Add(pathLabel, 0, wxALL | wxALIGN_CENTER, 5);
	dataPath = new wxTextCtrl(this, -1, settings.get("generateRMEPath"), wxDefaultPosition, wxSize(300, -1));
	hbox->Add(dataPath, 1, wxALL | wxALIGN_CENTER, 5);
	auto browseButton = new wxButton(this, ID_BROWSE_BUTTON, wxT("Browse..."));
	hbox->Add(browseButton, 0, wxALL | wxALIGN_CENTER, 5);

	vbox->Add(hbox);

	progress = new wxGauge(this, -1, 100);
	vbox->Add(progress, 1, wxALL | wxEXPAND, 5);

	auto generateButton = new wxButton(this, ID_GENERATE_BUTTON, wxT("Generate"));
	generateButton->SetFocus();
	vbox->Add(generateButton, 0, wxALL | wxALIGN_RIGHT, 5);

	SetSizer(vbox);
	vbox->Layout();
	vbox->Fit(this);
	Center();
}

void GenerateRMEDialog::updateProgress(double value)
{
	/*float percentageFactor = readOrSaveAlphaCheckbox->GetValue() ? 33.333 : 50;
	if (currentLoading == LOADING_DAT)
	{
		progress->SetValue(ceil(value * percentageFactor));
	}
	else if (currentLoading == LOADING_SPR)
	{
		progress->SetValue(ceil(percentageFactor + value * percentageFactor));
	}
	else if (currentLoading == LOADING_ALP)
	{
		progress->SetValue(ceil(percentageFactor * 2 + value * percentageFactor));
	}*/
	wxTheApp->Yield();
}
