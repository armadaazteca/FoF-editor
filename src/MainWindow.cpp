#include <wx/wxprec.h>
#ifndef WX_PRECOMP
#include <wx/wx.h>
#include <wx/gbsizer.h>
#include <wx/generic/statbmpg.h>
#endif
#include "Events.h"
#include "MainWindow.h"
#include "DatSprOpenDialog.h"
#include "DatSprReaderWriter.h"

const wxString MainWindow::CATEGORIES[] = { "Items", "Creatures", "Effects", "Projectiles" };

wxBEGIN_EVENT_TABLE(MainWindow, wxFrame)
	EVT_MENU(wxID_OPEN,  MainWindow::OnOpenDatSprDialog)
	EVT_MENU(wxID_EXIT,  MainWindow::OnExit)
	EVT_MENU(wxID_ABOUT, MainWindow::OnAbout)
	EVT_COMMAND(wxID_ANY, DAT_SPR_LOADED, MainWindow::OnDatSprLoaded)
	EVT_COMBOBOX(ID_CATEGORIES_COMBOBOX, MainWindow::OnObjectCategoryChanged)
	EVT_LISTBOX(ID_OBJECTS_LISTBOX, MainWindow::OnObjectSelected)
wxEND_EVENT_TABLE()

MainWindow::MainWindow(const wxString & title, const wxPoint & pos, const wxSize & size) : wxFrame(NULL, wxID_ANY, title, pos, size)
{
	currentCategory = CategoryItem;

	CreateStatusBar();

	// constructing main menu
	wxMenu * menuFile = new wxMenu();
	menuFile->Append(wxID_OPEN);
	menuFile->AppendSeparator();
	menuFile->Append(wxID_EXIT);
	wxMenu * menuHelp = new wxMenu();
	menuHelp->Append(wxID_ABOUT);
	wxMenuBar * menuBar = new wxMenuBar();
	menuBar->Append(menuFile, "&File");
	menuBar->Append(menuHelp, "&Help");
	SetMenuBar(menuBar);

	// constructing main window interface
	auto panel = new wxPanel(this, -1);
	auto vbox = new wxBoxSizer(wxVERTICAL);
	auto fgs = new wxFlexGridSizer(1, 3, 10, 10);

	auto leftColumnGrid = new wxFlexGridSizer(2, 1, 5, 5);
	categoryComboBox = new wxComboBox(panel, ID_CATEGORIES_COMBOBOX, CATEGORIES[0],
			wxDefaultPosition, wxDefaultSize, 4, CATEGORIES, wxCB_READONLY);
	leftColumnGrid->Add(categoryComboBox, 1, wxEXPAND);
	objectsListBox = new wxListBox(panel, ID_OBJECTS_LISTBOX, wxDefaultPosition, wxSize(200, -1), 0, NULL, wxLB_SINGLE);
	leftColumnGrid->Add(objectsListBox, 1, wxEXPAND);
	leftColumnGrid->AddGrowableRow(1, 1);

	auto midColumnGrid = new wxFlexGridSizer(2, 1, 5, 5);
	auto attrBox = new wxStaticBox(panel, -1, "Attributes");
	auto attrBoxSizer = new wxGridSizer(4, 5, 5);

	// attributes
	wxCheckBox * curCb = nullptr;
	curCb = attrCheckboxes[ID_ATTR_IS_GROUND_BORDER] = new wxCheckBox(attrBox, ID_ATTR_IS_GROUND_BORDER, "Is ground border");
	attrBoxSizer->Add(curCb, 0, wxALL, 5);
	curCb = attrCheckboxes[ID_ATTR_IS_ON_BOTTOM] = new wxCheckBox(attrBox, ID_ATTR_IS_ON_BOTTOM, "Is on bottom");
	attrBoxSizer->Add(curCb, 0, wxALL, 5);
	curCb = attrCheckboxes[ID_ATTR_IS_ON_TOP] = new wxCheckBox(attrBox, ID_ATTR_IS_ON_TOP, "Is on top");
	attrBoxSizer->Add(curCb, 0, wxALL, 5);
	curCb = attrCheckboxes[ID_ATTR_IS_CONTAINER] = new wxCheckBox(attrBox, ID_ATTR_IS_CONTAINER, "Is container");
	attrBoxSizer->Add(curCb, 0, wxALL, 5);
	curCb = attrCheckboxes[ID_ATTR_IS_STACKABLE] = new wxCheckBox(attrBox, ID_ATTR_IS_STACKABLE, "Is stackable");
	attrBoxSizer->Add(curCb, 0, wxALL, 5);
	curCb = attrCheckboxes[ID_ATTR_IS_FORCE_USE] = new wxCheckBox(attrBox, ID_ATTR_IS_FORCE_USE, "Force use");
	attrBoxSizer->Add(curCb, 0, wxALL, 5);
	curCb = attrCheckboxes[ID_ATTR_IS_MULTI_USE] = new wxCheckBox(attrBox, ID_ATTR_IS_MULTI_USE, "Multi use");
	attrBoxSizer->Add(curCb, 0, wxALL, 5);
	curCb = attrCheckboxes[ID_ATTR_IS_FLUID_CONTAINER] = new wxCheckBox(attrBox, ID_ATTR_IS_FLUID_CONTAINER, "Is fluid container");
	attrBoxSizer->Add(curCb, 0, wxALL, 5);
	curCb = attrCheckboxes[ID_ATTR_IS_SPLASH] = new wxCheckBox(attrBox, ID_ATTR_IS_SPLASH, "Is splash");
	attrBoxSizer->Add(curCb, 0, wxALL, 5);
	curCb = attrCheckboxes[ID_ATTR_BLOCKS_PROJECTILES] = new wxCheckBox(attrBox, ID_ATTR_BLOCKS_PROJECTILES, "Blocks projectiles");
	attrBoxSizer->Add(curCb, 0, wxALL, 5);
	curCb = attrCheckboxes[ID_ATTR_IS_PICKUPABLE] = new wxCheckBox(attrBox, ID_ATTR_IS_PICKUPABLE, "Is pickupable");
	attrBoxSizer->Add(curCb, 0, wxALL, 5);
	curCb = attrCheckboxes[ID_ATTR_IS_WALKABLE] = new wxCheckBox(attrBox, ID_ATTR_IS_WALKABLE, "Is walkable");
	attrBoxSizer->Add(curCb, 0, wxALL, 5);
	curCb = attrCheckboxes[ID_ATTR_IS_MOVABLE] = new wxCheckBox(attrBox, ID_ATTR_IS_MOVABLE, "Is movable");
	attrBoxSizer->Add(curCb, 0, wxALL, 5);
	curCb = attrCheckboxes[ID_ATTR_IS_PATHABLE] = new wxCheckBox(attrBox, ID_ATTR_IS_PATHABLE, "Is pathable");
	attrBoxSizer->Add(curCb, 0, wxALL, 5);
	curCb = attrCheckboxes[ID_ATTR_CAN_BE_HIDDEN] = new wxCheckBox(attrBox, ID_ATTR_CAN_BE_HIDDEN, "Can be hidden");
	attrBoxSizer->Add(curCb, 0, wxALL, 5);
	curCb = attrCheckboxes[ID_ATTR_IS_HANGABLE] = new wxCheckBox(attrBox, ID_ATTR_IS_HANGABLE, "Is hangable");
	attrBoxSizer->Add(curCb, 0, wxALL, 5);
	curCb = attrCheckboxes[ID_ATTR_IS_HOOK_SOUTH] = new wxCheckBox(attrBox, ID_ATTR_IS_HOOK_SOUTH, "Hook south");
	attrBoxSizer->Add(curCb, 0, wxALL, 5);
	curCb = attrCheckboxes[ID_ATTR_IS_HOOK_EAST] = new wxCheckBox(attrBox, ID_ATTR_IS_HOOK_EAST, "Hook east");
	attrBoxSizer->Add(curCb, 0, wxALL, 5);
	curCb = attrCheckboxes[ID_ATTR_IS_ROTATABLE] = new wxCheckBox(attrBox, ID_ATTR_IS_ROTATABLE, "Is rotatable");
	attrBoxSizer->Add(curCb, 0, wxALL, 5);
	curCb = attrCheckboxes[ID_ATTR_IS_TRANSLUCENT] = new wxCheckBox(attrBox, ID_ATTR_IS_TRANSLUCENT, "Is translucent");
	attrBoxSizer->Add(curCb, 0, wxALL, 5);
	curCb = attrCheckboxes[ID_ATTR_IS_LYING_CORPSE] = new wxCheckBox(attrBox, ID_ATTR_IS_LYING_CORPSE, "Is lying corpse");
	attrBoxSizer->Add(curCb, 0, wxALL, 5);
	curCb = attrCheckboxes[ID_ATTR_IS_FULL_GROUND] = new wxCheckBox(attrBox, ID_ATTR_IS_FULL_GROUND, "Is full ground");
	attrBoxSizer->Add(curCb, 0, wxALL, 5);
	curCb = attrCheckboxes[ID_ATTR_IGNORE_LOOK] = new wxCheckBox(attrBox, ID_ATTR_IGNORE_LOOK, "Ignore look");
	attrBoxSizer->Add(curCb, 0, wxALL, 5);
	curCb = attrCheckboxes[ID_ATTR_IS_USABLE] = new wxCheckBox(attrBox, ID_ATTR_IS_USABLE, "Is usable");
	attrBoxSizer->Add(curCb, 0, wxALL, 5);

	attrBox->SetSizer(attrBoxSizer);
	midColumnGrid->Add(attrBox, 1, wxEXPAND);
	midColumnGrid->AddGrowableRow(0, 1);
	midColumnGrid->AddGrowableCol(0, 1);

	auto rightColumnGrid = new wxFlexGridSizer(2, 1, 5, 5);

	auto spritesBoxSizer = new wxStaticBoxSizer(wxVERTICAL, panel, "Sprites");
	spritesPanel = new wxScrolledWindow(panel, -1, wxDefaultPosition, wxSize(200, -1));
	spritesBoxSizer->Add(spritesPanel, 1, wxEXPAND);
	spritesPanelSizer = new wxFlexGridSizer(2, 5, 5);
	spritesPanelSizer->AddGrowableCol(1, 1);
	spritesPanel->SetSizer(spritesPanelSizer);
	spritesPanel->SetScrollRate(0, 20);

	rightColumnGrid->Add(spritesBoxSizer, 1, wxEXPAND);
	rightColumnGrid->AddGrowableRow(0, 1);
	rightColumnGrid->AddGrowableCol(0, 1);

	fgs->Add(leftColumnGrid, 1, wxEXPAND);
	fgs->Add(midColumnGrid, 1, wxEXPAND);
	fgs->Add(rightColumnGrid, 1, wxEXPAND);
	fgs->AddGrowableRow(0, 1);
	fgs->AddGrowableCol(1, 1);

	vbox->Add(fgs, 1, wxALL | wxEXPAND, 10);
	panel->SetSizer(vbox);
}

void MainWindow::OnOpenDatSprDialog(wxCommandEvent & event)
{
	DatSprOpenDialog * dialog = new DatSprOpenDialog(this);
	dialog->ShowModal();
}

void MainWindow::OnDatSprLoaded(wxCommandEvent & event)
{
	currentCategory = CategoryItem;
	fillObjectsListBox();
	auto objects = DatSprReaderWriter::getInstance().getObjects(currentCategory);
	setAttributeValues(objects->at(0));
	fillObjectSprites(objects->at(0));
}

void MainWindow::OnObjectCategoryChanged(wxCommandEvent & event)
{
	const wxString & category = categoryComboBox->GetValue();
	if (category == CATEGORIES[CategoryItem])
	{
		currentCategory = CategoryItem;
	}
	else if (category == CATEGORIES[CategoryCreature])
	{
		currentCategory = CategoryCreature;
	}
	else if (category == CATEGORIES[CategoryEffect])
	{
		currentCategory = CategoryEffect;
	}
	else if (category == CATEGORIES[CategoryProjectile])
	{
		currentCategory = CategoryProjectile;
	}
	fillObjectsListBox();
	auto objects = DatSprReaderWriter::getInstance().getObjects(currentCategory);
	setAttributeValues(objects->at(0));
	fillObjectSprites(objects->at(0));
}

void MainWindow::fillObjectsListBox()
{
	objectsListBox->Clear();
	auto objects = DatSprReaderWriter::getInstance().getObjects(currentCategory);
	wxArrayString objectIds;
	objectIds.Alloc(objects->size());
	for (auto & v : *objects)
	{
		objectIds.Add(wxString::Format("%i", v->id));
	}
	objectsListBox->InsertItems(objectIds, 0);
	objectsListBox->SetSelection(0);
}

void MainWindow::OnObjectSelected(wxCommandEvent & event)
{
	auto objects = DatSprReaderWriter::getInstance().getObjects(currentCategory);
	int objectId = wxAtoi(event.GetString());
	if (currentCategory == CategoryItem)
	{
		objectId -= 100;
	}
	else
	{
		objectId -= 1;
	}
	shared_ptr <DatObject> object = objects->at(objectId);

	setAttributeValues(object);
	fillObjectSprites(object);
}

void MainWindow::setAttributeValues(shared_ptr <DatObject> object)
{
	// at first, resetting attributes to default
	for (int i = ID_ATTR_IS_GROUND_BORDER; i < ID_ATTR_LAST; ++i)
	{
		attrCheckboxes[i]->SetValue(false);
	}
	// then setting values
	if (object->isGroundBorder)
	{
		attrCheckboxes[ID_ATTR_IS_GROUND_BORDER]->SetValue(true);
	}
	if (object->isOnTop)
	{
		attrCheckboxes[ID_ATTR_IS_ON_TOP]->SetValue(true);
	}
	if (object->isContainer)
	{
		attrCheckboxes[ID_ATTR_IS_CONTAINER]->SetValue(true);
	}
	if (object->isStackable)
	{
		attrCheckboxes[ID_ATTR_IS_STACKABLE]->SetValue(true);
	}
	if (object->isForceUse)
	{
		attrCheckboxes[ID_ATTR_IS_FORCE_USE]->SetValue(true);
	}
	if (object->isMultiUse)
	{
		attrCheckboxes[ID_ATTR_IS_MULTI_USE]->SetValue(true);
	}
	if (object->isFluidContainer)
	{
		attrCheckboxes[ID_ATTR_IS_FLUID_CONTAINER]->SetValue(true);
	}
	if (object->isSplash)
	{
		attrCheckboxes[ID_ATTR_IS_SPLASH]->SetValue(true);
	}
	if (object->blocksProjectiles)
	{
		attrCheckboxes[ID_ATTR_BLOCKS_PROJECTILES]->SetValue(true);
	}
	if (object->isPickupable)
	{
		attrCheckboxes[ID_ATTR_IS_PICKUPABLE]->SetValue(true);
	}
	if (object->isWalkable)
	{
		attrCheckboxes[ID_ATTR_IS_WALKABLE]->SetValue(true);
	}
	if (object->isMovable)
	{
		attrCheckboxes[ID_ATTR_IS_MOVABLE]->SetValue(true);
	}
	if (object->isPathable)
	{
		attrCheckboxes[ID_ATTR_IS_PATHABLE]->SetValue(true);
	}
	if (object->canBeHidden)
	{
		attrCheckboxes[ID_ATTR_CAN_BE_HIDDEN]->SetValue(true);
	}
	if (object->isHangable)
	{
		attrCheckboxes[ID_ATTR_IS_HANGABLE]->SetValue(true);
	}
	if (object->isHookSouth)
	{
		attrCheckboxes[ID_ATTR_IS_HOOK_SOUTH]->SetValue(true);
	}
	if (object->isHookEast)
	{
		attrCheckboxes[ID_ATTR_IS_HOOK_EAST]->SetValue(true);
	}
	if (object->isRotatable)
	{
		attrCheckboxes[ID_ATTR_IS_ROTATABLE]->SetValue(true);
	}
	if (object->isTranslucent)
	{
		attrCheckboxes[ID_ATTR_IS_TRANSLUCENT]->SetValue(true);
	}
	if (object->isLyingCorpse)
	{
		attrCheckboxes[ID_ATTR_IS_LYING_CORPSE]->SetValue(true);
	}
	if (object->isFullGround)
	{
		attrCheckboxes[ID_ATTR_IS_FULL_GROUND]->SetValue(true);
	}
	if (object->ignoreLook)
	{
		attrCheckboxes[ID_ATTR_IGNORE_LOOK]->SetValue(true);
	}
	if (object->isUsable)
	{
		attrCheckboxes[ID_ATTR_IS_USABLE]->SetValue(true);
	}
}

void MainWindow::fillObjectSprites(shared_ptr <DatObject> object)
{
	spritesPanelSizer->Clear(true);

	auto sprites = DatSprReaderWriter::getInstance().getSprites();
	shared_ptr <Sprite> sprite = nullptr;
	wxImage * image = nullptr;
	wxBitmap * bitmap = nullptr;
	wxGenericStaticBitmap * staticBitmap = nullptr;
	wxStaticText * spriteIdLabel = nullptr;

	bool hasAtLeastOneSprite = false;
	unsigned int spriteId = 0;
	for (unsigned int i = 0; i < object->spriteCount; ++i)
	{
		spriteId = object->spriteIDs[i];
		if (sprites->find(spriteId) != sprites->end())
		{
			sprite = sprites->at(spriteId);
			if (sprite->valid)
			{
				hasAtLeastOneSprite = true;
				image = new wxImage(32, 32, sprite->rgb, sprite->alpha, true);
				bitmap = new wxBitmap(*image);
				staticBitmap = new wxGenericStaticBitmap(spritesPanel, -1, *bitmap);
				spritesPanelSizer->Add(staticBitmap, 0, wxALL, 5);
				spriteIdLabel = new wxStaticText(spritesPanel, -1, wxString::Format("%i", spriteId));
				spritesPanelSizer->Add(spriteIdLabel, 0, wxALL | wxALIGN_CENTER_VERTICAL, 5);
			}
		}
	}

	if (!hasAtLeastOneSprite)
	{
		auto empty = new wxStaticText(spritesPanel, -1, "This object has no sprites");
		spritesPanelSizer->Add(empty, 0, wxALL, 5);
	}

	spritesPanelSizer->Layout();
	spritesPanel->FitInside();
}

void MainWindow::OnExit(wxCommandEvent & event)
{
	Close(true);
}

void MainWindow::OnAbout(wxCommandEvent & event)
{
	wxMessageBox("Here will be some about text", "About", wxOK | wxICON_INFORMATION);
}
