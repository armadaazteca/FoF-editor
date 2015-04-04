#include <wx/wxprec.h>
#ifndef WX_PRECOMP
#include <wx/wx.h>
#endif
#include <wx/gbsizer.h>
#include "Events.h"
#include "DatSprReaderWriter.h"
#include "AdvancedAttributesManager.h"
#include "AdvancedAttributesDialog.h"

const wxString AdvancedAttributesDialog::GROUPS[] = {
	"Other", "Ground", "Container", "Weapon", "Ammunition", "Armor", "Rune", "Teleport",
	"Magic Field", "Writeable", "Key", "Splash", "Fluid", "Door", "Deprecated"
};

wxBEGIN_EVENT_TABLE(AdvancedAttributesDialog, wxDialog)
	EVT_BUTTON(wxID_SAVE, AdvancedAttributesDialog::OnClickSaveButton)
	EVT_BUTTON(ID_BUTTON_ERASE, AdvancedAttributesDialog::OnClickEraseButton)
wxEND_EVENT_TABLE()

AdvancedAttributesDialog::AdvancedAttributesDialog(wxWindow * parent, DatObjectCategory objectCategory, unsigned int objectID)
	: wxDialog(parent, -1, wxString::Format("Edit advanced attributes of object <%i>", objectID)),
	  objCat(objectCategory), objID(objectID)
{
	auto vbox = new wxBoxSizer(wxVERTICAL);
	auto gbs = new wxGridBagSizer(5, 5);

	auto objects = DatSprReaderWriter::getInstance().getObjects(objCat);
	auto object = objects->at(objID - (objCat == CategoryItem ? 100 : 1));
	auto attributes = AdvancedAttributesManager::getInstance().getAttributes(objectCategory, objID);

	int currentRow = 0;
	if (objectCategory == CategoryItem)
	{
		auto groupLabel = new wxStaticText(this, wxID_ANY, "Group:");
		gbs->Add(groupLabel, wxGBPosition(currentRow, 0), wxDefaultSpan, wxALIGN_CENTER_VERTICAL);
		groupCombo = new wxComboBox(this, wxID_ANY, GROUPS[0],
		                      			wxDefaultPosition, wxDefaultSize, ITEM_GROUP_LAST, GROUPS, wxCB_READONLY);
		unsigned int group = object->isFullGround ? ITEM_GROUP_GROUND : ITEM_GROUP_NONE;
		if (attributes)
		{
			group = attributes->group < ITEM_GROUP_LAST ? attributes->group : ITEM_GROUP_NONE;
		}
		groupCombo->SetSelection(group);
		gbs->Add(groupCombo, wxGBPosition(currentRow, 1), wxDefaultSpan, wxALIGN_CENTER_VERTICAL);
		currentRow++;
	}

	auto nameLabel = new wxStaticText(this, wxID_ANY, "Name:");
	gbs->Add(nameLabel, wxGBPosition(currentRow, 0), wxDefaultSpan, wxALIGN_CENTER_VERTICAL);
	nameText = new wxTextCtrl(this, wxID_ANY, "", wxDefaultPosition, wxSize(300, -1));
	if (attributes && attributes->name)
	{
		nameText->ChangeValue(attributes->name.get());
	}
	gbs->Add(nameText, wxGBPosition(currentRow, 1), wxDefaultSpan, wxALIGN_CENTER_VERTICAL);
	currentRow++;

	if (objectCategory == CategoryItem)
	{
		auto descLabel = new wxStaticText(this, wxID_ANY, "Description:");
		gbs->Add(descLabel, wxGBPosition(currentRow, 0), wxDefaultSpan, wxALIGN_CENTER_VERTICAL);
		descriptionText = new wxTextCtrl(this, wxID_ANY, "", wxDefaultPosition, wxSize(300, -1));
		if (attributes && attributes->description)
		{
			descriptionText->ChangeValue(attributes->description.get());
		}
		gbs->Add(descriptionText, wxGBPosition(currentRow, 1), wxDefaultSpan, wxALIGN_CENTER_VERTICAL);
		currentRow++;

		auto articleLabel = new wxStaticText(this, wxID_ANY, "Article:");
		gbs->Add(articleLabel, wxGBPosition(currentRow, 0), wxDefaultSpan, wxALIGN_CENTER_VERTICAL);
		auto hbox = new wxBoxSizer(wxHORIZONTAL);
		articleARb = new wxRadioButton(this, wxID_ANY, "a", wxDefaultPosition, wxDefaultSize, wxRB_GROUP);
		articleARb->SetValue(attributes == nullptr || (attributes != nullptr && attributes->article == ARTICLE_A));
		hbox->Add(articleARb);
		articleAnRb = new wxRadioButton(this, wxID_ANY, "an");
		articleAnRb->SetValue(attributes != nullptr && attributes->article == ARTICLE_AN);
		hbox->Add(articleAnRb, 0, wxLEFT | wxALIGN_CENTER_VERTICAL, 5);
		gbs->Add(hbox, wxGBPosition(currentRow, 1), wxDefaultSpan, wxALIGN_CENTER_VERTICAL);
		currentRow++;

		auto floorChangeLabel = new wxStaticText(this, wxID_ANY, "Floor change:");
		gbs->Add(floorChangeLabel, wxGBPosition(currentRow, 0), wxDefaultSpan, wxALIGN_CENTER_VERTICAL);
		hbox = new wxBoxSizer(wxHORIZONTAL);
		floorChangeNoneRb = new wxRadioButton(this, wxID_ANY, "none", wxDefaultPosition, wxDefaultSize, wxRB_GROUP);
		floorChangeNoneRb->SetValue(attributes == nullptr || (attributes != nullptr && attributes->floorChange == FLOOR_CHANGE_NONE));
		hbox->Add(floorChangeNoneRb);
		floorChangeDownRb = new wxRadioButton(this, wxID_ANY, "down");
		floorChangeDownRb->SetValue(attributes != nullptr && attributes->floorChange == FLOOR_CHANGE_DOWN);
		hbox->Add(floorChangeDownRb, 0, wxLEFT | wxRIGHT | wxALIGN_CENTER_VERTICAL, 5);
		floorChangeUpRb = new wxRadioButton(this, wxID_ANY, "up");
		floorChangeUpRb->SetValue(attributes != nullptr && attributes->floorChange == FLOOR_CHANGE_UP);
		hbox->Add(floorChangeUpRb);
		gbs->Add(hbox, wxGBPosition(currentRow, 1), wxDefaultSpan, wxALIGN_CENTER_VERTICAL);
		currentRow++;
	}

	if (objectCategory == CategoryCreature)
	{
		auto typeLabel = new wxStaticText(this, wxID_ANY, "Creature type:");
		gbs->Add(typeLabel, wxGBPosition(currentRow, 0), wxDefaultSpan, wxALIGN_CENTER_VERTICAL);
		auto hbox = new wxBoxSizer(wxHORIZONTAL);
		typeMonsterRb = new wxRadioButton(this, wxID_ANY, "monster", wxDefaultPosition, wxDefaultSize, wxRB_GROUP);
		typeMonsterRb->SetValue(attributes == nullptr || (attributes != nullptr && attributes->creatureType == CREATURE_MONSTER));
		hbox->Add(typeMonsterRb);
		typeNPCRb = new wxRadioButton(this, wxID_ANY, "NPC");
		typeNPCRb->SetValue(attributes != nullptr && attributes->creatureType == CREATURE_NPC);
		hbox->Add(typeNPCRb, 0, wxLEFT | wxALIGN_CENTER_VERTICAL, 5);
		gbs->Add(hbox, wxGBPosition(currentRow, 1), wxDefaultSpan, wxALIGN_CENTER_VERTICAL);
		currentRow++;
	}

	auto hbox = new wxBoxSizer(wxHORIZONTAL);
	auto okButton = new wxButton(this, wxID_SAVE);
	okButton->SetFocus();
	hbox->Add(okButton);
	if (attributes)
	{
		auto eraseButton = new wxButton(this, ID_BUTTON_ERASE, "Erase...");
		hbox->Add(eraseButton, 0, wxLEFT, 5);
	}
	auto cancelButton = new wxButton(this, wxID_CANCEL);
	hbox->Add(cancelButton, 0, wxLEFT, 5);

	gbs->Add(hbox, wxGBPosition(currentRow, 0), wxGBSpan(1, 2), wxALIGN_CENTER_VERTICAL | wxALIGN_RIGHT);

	vbox->Add(gbs, 0, wxALL, 10);

	SetSizer(vbox);
	vbox->Layout();
	vbox->Fit(this);
	Center();
}

void AdvancedAttributesDialog::OnClickSaveButton(wxCommandEvent & event)
{
	auto & aam = AdvancedAttributesManager::getInstance();
	auto attrs = aam.getAttributes(objCat, objID);
	if (!attrs)
	{
		attrs = make_shared <AdvancedObjectAttributes> ();
	}
	const wxString & nameStr = nameText->GetValue();
	char * name = new char[nameStr.Length() + 1];
	strcpy(name, nameStr.c_str());
	attrs->name.reset(name);

	if (objCat == CategoryItem)
	{
		attrs->group = groupCombo->GetSelection();

		const wxString & descStr = descriptionText->GetValue();
		char * description = new char[descStr.Length() + 1];
		strcpy(description, descStr.c_str());
		attrs->description.reset(description);

		if (articleARb->GetValue()) attrs->article = ARTICLE_A;
		if (articleAnRb->GetValue()) attrs->article = ARTICLE_AN;

		if (floorChangeNoneRb->GetValue()) attrs->floorChange = FLOOR_CHANGE_NONE;
		if (floorChangeDownRb->GetValue()) attrs->floorChange = FLOOR_CHANGE_DOWN;
		if (floorChangeUpRb->GetValue()) attrs->floorChange = FLOOR_CHANGE_UP;
	}
	else if (objCat == CategoryCreature)
	{
		if (typeMonsterRb->GetValue()) attrs->creatureType = CREATURE_MONSTER;
		if (typeNPCRb->GetValue()) attrs->creatureType = CREATURE_NPC;
	}

	AdvancedAttributesManager::getInstance().setAttributes(objCat, objID, attrs);

	Close();

	// notifying main window that attributes have been changed
	wxCommandEvent evt(ADV_ATTRS_CHANGED, 1);
	wxPostEvent(m_parent, evt);
}

void AdvancedAttributesDialog::OnClickEraseButton(wxCommandEvent & event)
{
	const wchar_t * confirmation = wxT("This action will completely erase advanced attributes record, so it won't be saved into .aoa file");
	if (wxMessageBox(confirmation, "Confirm erase", wxOK | wxCANCEL) == wxOK)
	{
		AdvancedAttributesManager::getInstance().removeAttributes(objCat, objID);

		Close();

		// notifying main window that attributes have been changed
		wxCommandEvent evt(ADV_ATTRS_CHANGED, 1);
		wxPostEvent(m_parent, evt);
	}
}
