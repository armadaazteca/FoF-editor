#include <wx/wxprec.h>
#ifndef WX_PRECOMP
#include <wx/wx.h>
#endif
#include <wx/filename.h>
#include "Config.h"
#include "Events.h"
#include "DatSprReaderWriter.h"
#include "AdvancedAttributesManager.h"
#include "AutoBackupDialog.h"

wxBEGIN_EVENT_TABLE(AutoBackupDialog, wxDialog)
	EVT_ACTIVATE(AutoBackupDialog::run)
wxEND_EVENT_TABLE()

AutoBackupDialog::AutoBackupDialog(wxWindow * parent)
	: wxDialog(parent, wxID_ANY, "Performing auto-backup")
{
	auto vbox = new wxBoxSizer(wxVERTICAL);
	auto hbox = new wxBoxSizer(wxHORIZONTAL);

	progress = new wxGauge(this, wxID_ANY, 100, wxDefaultPosition, wxSize(200, -1));
	auto cancelButton = new wxButton(this, wxID_CANCEL);
	hbox->Add(progress, 0, wxRIGHT | wxALIGN_CENTER, 5);
	hbox->Add(cancelButton);

	vbox->Add(hbox, 0, wxALL, 10);

	SetSizer(vbox);
	Fit();
	Center();
}

void AutoBackupDialog::run(wxActivateEvent & event)
{
	if (isRunning) return;

	auto & dsrw = DatSprReaderWriter::getInstance();
	bool hasObjects = false;
	for (int cat = CategoryItem; cat < InvalidCategory; ++cat)
	{
		auto objects = dsrw.getObjects((DatObjectCategory) cat);
		if (objects && objects->size() > 0)
		{
			hasObjects = true;
			break;
		}
	}
	if (!hasObjects) return;

	progressStages = 5;
	currentProgressStage = 0;
	percentsPerStage = 100 / (float) progressStages;

	auto closeAndNotify = [&]()
	{
		Close();
		// notifying main window that files have been loaded
		wxCommandEvent event(AUTOBACKUP_PROCESSED, 1);
		wxPostEvent(m_parent, event);
	};

	wxFileName savePath;
	savePath.AssignCwd();
	wxString fullPath = savePath.GetFullPath();
	const wxString & defaultName = "autosave";
	savePath.AppendDir(defaultName);
	fullPath = savePath.GetFullPath();
	if (!savePath.DirExists())
	{
		wxMkdir(savePath.GetFullPath());
	}
	savePath.SetName(defaultName);

	savePath.SetExt("dat");
	if (dsrw.writeDat(savePath.GetFullPath(), this))
	{
		currentProgressStage++;
		savePath.SetExt("spr");
		if (dsrw.writeSpr(savePath.GetFullPath(), this))
		{
			currentProgressStage++;
			savePath.SetExt("blk");
			if (!dsrw.writeBlockingStates(savePath.GetFullPath(), this))
			{
				wxMessageBox(wxString::Format(Config::COMMON_WRITE_ERROR, ".blk"), Config::ERROR_TITLE,
										 wxOK | wxICON_ERROR);
			}

			currentProgressStage++;
			savePath.SetExt("alp");
			if (!dsrw.writeAlpha(savePath.GetFullPath(), this))
			{
				wxMessageBox(wxString::Format(Config::COMMON_WRITE_ERROR, ".alp"), Config::ERROR_TITLE,
										 wxOK | wxICON_ERROR);
			}

			currentProgressStage++;
			savePath.SetExt("aoa");
			if (!AdvancedAttributesManager::getInstance().save(savePath.GetFullPath(), this))
			{
				wxMessageBox(wxString::Format(Config::COMMON_WRITE_ERROR, ".aoa"), Config::ERROR_TITLE,
										 wxOK | wxICON_ERROR);
			}

			closeAndNotify();
		}
		else
		{
			wxMessageBox(wxString::Format(Config::COMMON_WRITE_ERROR, ".spr"), Config::ERROR_TITLE,
			             wxOK | wxICON_ERROR);
			closeAndNotify();
		}
	}
	else
	{
		wxMessageBox(wxString::Format(Config::COMMON_WRITE_ERROR, ".dat"), Config::ERROR_TITLE,
		             wxOK | wxICON_ERROR);
		closeAndNotify();
	}
}

void AutoBackupDialog::updateProgress(double value)
{
	progress->SetValue(ceil(percentsPerStage * currentProgressStage + percentsPerStage * value));
	wxTheApp->Yield();
}
