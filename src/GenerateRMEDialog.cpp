#include <wx/wxprec.h>
#ifndef WX_PRECOMP
#include <wx/wx.h>
#endif
#include <wx/filename.h>
#include "Events.h"
#include "Settings.h"
#include "DatSprReaderWriter.h"
#include "ItemsOtbWriter.h"
#include "GenerateRMEDialog.h"

wxBEGIN_EVENT_TABLE(GenerateRMEDialog, wxDialog)
	EVT_BUTTON(ID_BROWSE_BUTTON, GenerateRMEDialog::OnClickBrowseButton)
	EVT_BUTTON(ID_GENERATE_BUTTON, GenerateRMEDialog::OnClickGenerateButton)
wxEND_EVENT_TABLE()

GenerateRMEDialog::GenerateRMEDialog(wxWindow * parent) : wxDialog(parent, -1, "Generate RME resources")
{
	auto vbox = new wxBoxSizer(wxVERTICAL), vboxwrap = new wxBoxSizer(wxVERTICAL);
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

	overwriteCb = new wxCheckBox(this, wxID_ANY, "Overwrite existing files");
	overwriteCb->SetValue(settings.get("generateRMEOverwrite").IsSameAs("yes"));
	vbox->Add(overwriteCb, 0, wxALL, 5);

	progress = new wxGauge(this, -1, 100);
	vbox->Add(progress, 1, wxALL | wxEXPAND, 5);

	auto generateButton = new wxButton(this, ID_GENERATE_BUTTON, wxT("Generate"));
	generateButton->SetFocus();
	vbox->Add(generateButton, 0, wxALL | wxALIGN_RIGHT, 5);

	vboxwrap->Add(vbox, 0, wxALL, 5);

	SetSizer(vboxwrap);
	vboxwrap->Layout();
	vboxwrap->Fit(this);
	Center();
}

void GenerateRMEDialog::OnClickBrowseButton(wxCommandEvent & event)
{
	Settings & settings = Settings::getInstance();
	const wxString & pathStr = settings.get("generateRMEPath");
	wxDirDialog browseDialog(this, "Choose path for generated files", pathStr);
	if (browseDialog.ShowModal() == wxID_OK)
	{
		dataPath->Replace(0, dataPath->GetLastPosition(), browseDialog.GetPath());
	}
}

void GenerateRMEDialog::OnClickGenerateButton(wxCommandEvent & event)
{
	const wxString & dataPathStr = dataPath->GetValue();
	bool allowOverwrite = overwriteCb->GetValue();
	Settings & settings = Settings::getInstance();
	if (dataPathStr.Length() == 0)
	{
		wxMessageDialog error(this, "Path cannot be empty", "Error", wxOK | wxICON_ERROR);
		error.ShowModal();
		return;
	}

	settings.set("generateRMEPath", dataPathStr);
	settings.set("generateRMEOverwrite", allowOverwrite ? "yes" : "no");
	settings.save();

	wxFileName filename(dataPathStr);
	if (!filename.DirExists())
	{
		wxMessageDialog error(this, "Directory does not exist", "Error", wxOK | wxICON_ERROR);
		error.ShowModal();
		return;
	}

	auto items = DatSprReaderWriter::getInstance().getObjects(CategoryItem);
	if (items->size() == 0)
	{
		wxMessageDialog error(this, "Items list is empty, nothing to write.", "Error", wxOK | wxICON_ERROR);
		error.ShowModal();
		return;
	}


	currentWriting = WRITING_ITEMS_OTB;
	filename.AppendDir(filename.GetFullName());
	filename.SetName("items");
	filename.SetExt("otb");
	if (!allowOverwrite && filename.FileExists())
	{
		wxMessageBox("items.otb already exists, aborting", "Error", wxOK | wxICON_ERROR);
		return;
	}

	if (ItemsOtbWriter::getInstance().writeItemsOtb(items, filename.GetFullPath(), this))
	{
		// notifying main window that files have been saved
		wxCommandEvent event(RME_RES_GENERATED, 1);
		wxPostEvent(m_parent, event);
		Close();
	}
	else
	{
		wxMessageBox("The items.otb file cannot be written", "Error", wxOK | wxICON_ERROR);
	}
}

void GenerateRMEDialog::updateProgress(double value)
{
	float percentageFactor = 33.333;
	if (currentWriting == WRITING_ITEMS_OTB)
	{
		progress->SetValue(ceil(value * percentageFactor));
	}
	else if (currentWriting == WRITING_ITEMS_XML)
	{
		progress->SetValue(ceil(percentageFactor + value * percentageFactor));
	}
	else if (currentWriting == WRITING_CREATURES_XML)
	{
		progress->SetValue(ceil(percentageFactor * 2 + value * percentageFactor));
	}
	wxTheApp->Yield();
}
