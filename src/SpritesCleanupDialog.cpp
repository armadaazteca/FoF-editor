#include <wx/wxprec.h>
#ifndef WX_PRECOMP
#include <wx/wx.h>
#endif
#include "Events.h"
#include "DatSprReaderWriter.h"
#include "SpritesCleanupDialog.h"

wxBEGIN_EVENT_TABLE(SpritesCleanupDialog, wxDialog)
	EVT_BUTTON(ID_RUN_BUTTON, SpritesCleanupDialog::OnClickRunButton)
wxEND_EVENT_TABLE()

SpritesCleanupDialog::SpritesCleanupDialog(wxWindow * parent)
	: wxDialog(parent, -1, "Clean-up unused sprites", wxDefaultPosition, wxSize(400, 150))
{
	auto vbox = new wxBoxSizer(wxVERTICAL), vboxwrap = new wxBoxSizer(wxVERTICAL);

	const char * toolDesc = "This tool will remove all sprites that are not used by any object. "
	                        "Sprites will be removed in-memory, thus .spr file needs to be saved after this operation "
													"to persist changes.";
	auto toolDescLabel = new wxStaticText(this, -1, toolDesc, wxDefaultPosition, wxSize(-1, 50));
	vbox->Add(toolDescLabel, 0, wxALL | wxEXPAND, 5);

	auto hbox = new wxBoxSizer(wxHORIZONTAL);
	progress = new wxGauge(this, -1, 100);
	hbox->Add(progress, 9, wxALL | wxALIGN_CENTER | wxEXPAND, 5);
	auto runButton = new wxButton(this, ID_RUN_BUTTON, "Run");
	runButton->SetDefault();
	runButton->SetFocus();
	hbox->Add(runButton, 1, wxTOP | wxBOTTOM | wxALIGN_CENTER | wxEXPAND, 5);
	vbox->Add(hbox);

	vboxwrap->Add(vbox, 0, wxALL, 5);

	SetSizer(vboxwrap);
	Center();
}

void SpritesCleanupDialog::OnClickRunButton(wxCommandEvent & event)
{
	auto & dsrw = DatSprReaderWriter::getInstance();
	auto items = dsrw.getObjects(CategoryItem);
	auto creatures = dsrw.getObjects(CategoryCreature);
	auto effects = dsrw.getObjects(CategoryEffect);
	auto projectiles = dsrw.getObjects(CategoryProjectile);
	auto sprites = dsrw.getSprites();

	unsigned int operations = 0, allObjectsCount = items->size() + creatures->size() + effects->size() + projectiles->size();
	unsigned int totalCount = allObjectsCount + sprites->size() * 2; // x2 because we iterate spites twice
	shared_ptr <DatSprReaderWriter::DatObjectList> lists[] = { items, creatures, effects, projectiles };
	shared_ptr <DatSprReaderWriter::DatObjectList> currentList = nullptr;
	unsigned int sc = 0, sid = 0;

	// === STAGE 1 - REMOVING SPRITES ===
	// at first marking all sprites as unused
	for (auto kv : *sprites)
	{
		kv.second->isUsed = false;
		progress->SetValue(ceil(++operations / totalCount * 50));
		wxTheApp->Yield();
	}

	// then iterating across all objects and their sprites to mark used sprites
	for (int category = CategoryItem; category < LastCategory; category++)
	{
		currentList = lists[category];
		for (auto object : *currentList)
		{
			sc = object->spriteCount;
			for (unsigned int i = 0; i < sc; ++i)
			{
				if ((sid = object->spriteIDs[i]))
				{
					sprites->at(object->spriteIDs[i])->isUsed = true;
				}
			}
			progress->SetValue(ceil(++operations / totalCount * 50));
			wxTheApp->Yield();
		}
	}

	// then removing all sprites that are not used
	unsigned int removed = 0;
	auto it = sprites->begin();
	while (it != sprites->end())
	{
		if (!it->second->isUsed)
		{
			sprites->erase(it++);
			removed++;
		}
		else
		{
			++it;
		}
		progress->SetValue(ceil(++operations / totalCount * 50));
		wxTheApp->Yield();
	}
	// === END OF STAGE 1 ===
	
	if (removed)
	{
		// === STAGE 2 - RE-MAPPING ===
		// making new sprites map and map for oldID -> newID
		operations = 0;
		totalCount = allObjectsCount + sprites->size();
		auto newSprites = make_shared <DatSprReaderWriter::SpriteMap> ();
		map <unsigned int, unsigned int> spriteIDsMap;
		unsigned int id = 1;
		shared_ptr <Sprite> sprite = nullptr;
		for (auto & kv : *sprites)
		{
			sprite = kv.second;
			spriteIDsMap[sprite->id] = id;
			sprite->id = id;
			(*newSprites)[id++] = sprite;
			
			progress->SetValue(50 + ceil(++operations / totalCount * 50));
			wxTheApp->Yield();
		}
		dsrw.setSprites(newSprites);

		// replacing sprite IDs in objects
		for (int category = CategoryItem; category < LastCategory; category++)
		{
			currentList = lists[category];
			for (auto object : *currentList)
			{
				sc = object->spriteCount;
				for (unsigned int i = 0; i < sc; ++i)
				{
					if (object->spriteIDs[i])
					{
						object->spriteIDs[i] = spriteIDsMap[object->spriteIDs[i]];
					}
				}
				progress->SetValue(50 + ceil(++operations / totalCount * 50));
				wxTheApp->Yield();
			}
		}
		// === END OF STAGE 2 ===

		wxMessageBox(wxString::Format("Cleaned-up %i sprites. Don't forget to save.", removed), "Clean-up result");
		wxCommandEvent event(SPRITES_CLEANED_UP, 1);
		wxPostEvent(m_parent, event);
	}
	else
	{
		wxMessageBox("No unused sprites have been found", "Clean-up result");
	}
	Close();
}
