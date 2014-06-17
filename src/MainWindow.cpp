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
	EVT_TEXT(ID_ANIM_WIDTH_INPUT, MainWindow::OnWidthOrHeightChanged)
	EVT_TEXT(ID_ANIM_HEIGHT_INPUT, MainWindow::OnWidthOrHeightChanged)
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
	mainPanel = new wxPanel(this, -1);
	auto vbox = new wxBoxSizer(wxVERTICAL);
	auto fgs = new wxFlexGridSizer(1, 3, 10, 10);

	auto leftColumnGrid = new wxFlexGridSizer(2, 1, 5, 5);
	categoryComboBox = new wxComboBox(mainPanel, ID_CATEGORIES_COMBOBOX, CATEGORIES[0],
			wxDefaultPosition, wxDefaultSize, 4, CATEGORIES, wxCB_READONLY);
	leftColumnGrid->Add(categoryComboBox, 1, wxEXPAND);
	objectsListBox = new wxListBox(mainPanel, ID_OBJECTS_LISTBOX, wxDefaultPosition, wxSize(200, -1), 0, NULL, wxLB_SINGLE);
	leftColumnGrid->Add(objectsListBox, 1, wxEXPAND);
	leftColumnGrid->AddGrowableRow(1, 1);

	auto midColumnGrid = new wxFlexGridSizer(2, 1, 5, 5);

	// animation block
	auto animationBoxSizer = new wxStaticBoxSizer(wxVERTICAL, mainPanel, "Animation");
	animationBoxExpandSizer = new wxFlexGridSizer(1, 1, 0, 0);
	animationBoxExpandSizer->AddGrowableRow(0, 1);
	animationBoxExpandSizer->AddGrowableCol(0, 1);
	animationPanel = new wxPanel(animationBoxSizer->GetStaticBox(), -1);
	animationPanelSizer = new wxBoxSizer(wxVERTICAL);

	wxImage arrowIconImage("res/icons/green_arrow_left.png", wxBITMAP_TYPE_PNG);
	animationMainGridSizer = new wxFlexGridSizer(3, 3, 5, 5);

	animationMainGridSizer->Add(32, 32);
	auto dirTopButton = new wxButton(animationPanel, ID_DIR_TOP_BUTTON, "", wxDefaultPosition, wxSize(32, 32));
	arrowIconImage = arrowIconImage.Rotate90();
	wxBitmap dirTopButtonIcon(arrowIconImage);
	dirTopButton->SetBitmap(dirTopButtonIcon, wxLEFT);
	animationMainGridSizer->Add(dirTopButton, 0, wxALIGN_CENTER);

	animationMainGridSizer->Add(32, 32);
	auto dirLeftButton = new wxButton(animationPanel, ID_DIR_LEFT_BUTTON, "", wxDefaultPosition, wxSize(32, 32));
	arrowIconImage = arrowIconImage.Rotate90(false);
	wxBitmap dirLeftButtonIcon(arrowIconImage);
	dirLeftButton->SetBitmap(dirLeftButtonIcon, wxLEFT);
	animationMainGridSizer->Add(dirLeftButton, 0, wxALIGN_CENTER);

	animationMainGridSizer->Add(32, 32);
	auto dirRightButton = new wxButton(animationPanel, ID_DIR_RIGHT_BUTTON, "", wxDefaultPosition, wxSize(32, 32));
	arrowIconImage = arrowIconImage.Rotate180();
	wxBitmap dirRightButtonIcon(arrowIconImage);
	dirRightButton->SetBitmap(dirRightButtonIcon, wxLEFT);
	animationMainGridSizer->Add(dirRightButton, 0, wxALIGN_CENTER);

	animationMainGridSizer->Add(32, 32);
	auto dirBottomButton = new wxButton(animationPanel, ID_DIR_BOTTOM_BUTTON, "", wxDefaultPosition, wxSize(32, 32));
	arrowIconImage = arrowIconImage.Rotate90();
	wxBitmap dirBottomButtonIcon(arrowIconImage);
	dirBottomButton->SetBitmap(dirBottomButtonIcon, wxLEFT);
	animationMainGridSizer->Add(dirBottomButton, 0, wxALIGN_CENTER);
	animationMainGridSizer->Add(32, 32);

	animationWidth = animationHeight = 1;
	buildAnimationSpriteHolders();
	animationPanelSizer->Add(animationMainGridSizer, 0, wxALIGN_CENTER);

	// frame control
	auto frameNumberSizer = new wxBoxSizer(wxHORIZONTAL);
	auto currentFrameText = new wxStaticText(animationPanel, -1, "Current frame:");
	frameNumberSizer->Add(currentFrameText, 0, wxALL, 10);
	currentFrameNumber = new wxStaticText(animationPanel, -1, "0");
	frameNumberSizer->Add(currentFrameNumber, 0, wxTOP | wxBOTTOM, 10);
	animationPanelSizer->Add(frameNumberSizer, 0, wxALIGN_CENTER);

	auto frameButtonsSizer = new wxBoxSizer(wxHORIZONTAL);
	auto prevFrameButton = new wxButton(animationPanel, ID_PREV_FRAME_BUTTON, "< previous");
	frameButtonsSizer->Add(prevFrameButton, 0, wxTOP | wxRIGHT, 5);
	auto nextFrameButton = new wxButton(animationPanel, ID_NEXT_FRAME_BUTTON, "next >");
	frameButtonsSizer->Add(nextFrameButton, 0, wxTOP | wxLEFT, 5);
	animationPanelSizer->Add(frameButtonsSizer, 0, wxALIGN_CENTER);

	// width and height settings
	animationPanelSizer->Add(0, 5); // a little spacer
	auto widthAndHeightSizer = new wxBoxSizer(wxHORIZONTAL);
	auto widthLabel = new wxStaticText(animationPanel, -1, "Width:");
	widthAndHeightSizer->Add(widthLabel, 0, wxALIGN_CENTER_VERTICAL);
	animationWidthInput = new wxTextCtrl(animationPanel, ID_ANIM_WIDTH_INPUT, "1", wxDefaultPosition, wxSize(25, -1), wxTE_RIGHT);
	widthAndHeightSizer->Add(animationWidthInput, 0, wxALL, 5);
	auto heightLabel = new wxStaticText(animationPanel, -1, "Height:");
	widthAndHeightSizer->Add(heightLabel, 0, wxLEFT | wxALIGN_CENTER_VERTICAL, 10);
	animationHeightInput = new wxTextCtrl(animationPanel, ID_ANIM_HEIGHT_INPUT, "1", wxDefaultPosition, wxSize(25, -1), wxTE_RIGHT);
	widthAndHeightSizer->Add(animationHeightInput, 0, wxALL, 5);
	animationPanelSizer->Add(widthAndHeightSizer, 0, wxALIGN_CENTER);

	// amount of frames setting
	auto amountOfFramesSizer = new wxBoxSizer(wxHORIZONTAL);
	auto amountOfFramesLabel = new wxStaticText(animationPanel, -1, "Amount of frames:");
	amountOfFramesSizer->Add(amountOfFramesLabel, 0, wxALIGN_CENTER_VERTICAL);
	amountOfFramesInput = new wxTextCtrl(animationPanel, ID_FRAMES_AMOUNT_INPUT, "1", wxDefaultPosition, wxSize(25, -1), wxTE_RIGHT);
	amountOfFramesSizer->Add(amountOfFramesInput, 0, wxALL, 5);
	animationPanelSizer->Add(amountOfFramesSizer, 0, wxALIGN_CENTER);

	// always animated setting
	alwaysAnimatedCheckbox = new wxCheckBox(animationPanel, ID_ALWAYS_ANIMATED_CHECKBOX, "Always animated");
	animationPanelSizer->Add(alwaysAnimatedCheckbox, 0, wxALL | wxALIGN_CENTER, 5);

	animationPanel->SetSizer(animationPanelSizer);
	animationBoxExpandSizer->Add(animationPanel, 0, wxALIGN_CENTER);
	animationBoxSizer->Add(animationBoxExpandSizer, 1, wxEXPAND);
	midColumnGrid->Add(animationBoxSizer, 1, wxEXPAND);

	// attributes
	auto attrsBox = new wxStaticBoxSizer(wxVERTICAL, mainPanel, "Attributes");
	auto attrsPanel = new wxPanel(mainPanel, -1);
	auto attrsPanelSizer = new wxGridSizer(4, 0, 0);

	wxCheckBox * curCb = nullptr;
	curCb = attrCheckboxes[ID_ATTR_IS_GROUND_BORDER] = new wxCheckBox(attrsPanel, ID_ATTR_IS_GROUND_BORDER, "Is ground border");
	attrsPanelSizer->Add(curCb, 0, wxALL, 3);
	curCb = attrCheckboxes[ID_ATTR_IS_ON_BOTTOM] = new wxCheckBox(attrsPanel, ID_ATTR_IS_ON_BOTTOM, "Is on bottom");
	attrsPanelSizer->Add(curCb, 0, wxALL, 3);
	curCb = attrCheckboxes[ID_ATTR_IS_ON_TOP] = new wxCheckBox(attrsPanel, ID_ATTR_IS_ON_TOP, "Is on top");
	attrsPanelSizer->Add(curCb, 0, wxALL, 3);
	curCb = attrCheckboxes[ID_ATTR_IS_CONTAINER] = new wxCheckBox(attrsPanel, ID_ATTR_IS_CONTAINER, "Is container");
	attrsPanelSizer->Add(curCb, 0, wxALL, 3);
	curCb = attrCheckboxes[ID_ATTR_IS_STACKABLE] = new wxCheckBox(attrsPanel, ID_ATTR_IS_STACKABLE, "Is stackable");
	attrsPanelSizer->Add(curCb, 0, wxALL, 3);
	curCb = attrCheckboxes[ID_ATTR_IS_FORCE_USE] = new wxCheckBox(attrsPanel, ID_ATTR_IS_FORCE_USE, "Force use");
	attrsPanelSizer->Add(curCb, 0, wxALL, 3);
	curCb = attrCheckboxes[ID_ATTR_IS_MULTI_USE] = new wxCheckBox(attrsPanel, ID_ATTR_IS_MULTI_USE, "Multi use");
	attrsPanelSizer->Add(curCb, 0, wxALL, 3);
	curCb = attrCheckboxes[ID_ATTR_IS_FLUID_CONTAINER] = new wxCheckBox(attrsPanel, ID_ATTR_IS_FLUID_CONTAINER, "Is fluid container");
	attrsPanelSizer->Add(curCb, 0, wxALL, 3);
	curCb = attrCheckboxes[ID_ATTR_IS_SPLASH] = new wxCheckBox(attrsPanel, ID_ATTR_IS_SPLASH, "Is splash");
	attrsPanelSizer->Add(curCb, 0, wxALL, 3);
	curCb = attrCheckboxes[ID_ATTR_BLOCKS_PROJECTILES] = new wxCheckBox(attrsPanel, ID_ATTR_BLOCKS_PROJECTILES, "Blocks projectiles");
	attrsPanelSizer->Add(curCb, 0, wxALL, 3);
	curCb = attrCheckboxes[ID_ATTR_IS_PICKUPABLE] = new wxCheckBox(attrsPanel, ID_ATTR_IS_PICKUPABLE, "Is pickupable");
	attrsPanelSizer->Add(curCb, 0, wxALL, 3);
	curCb = attrCheckboxes[ID_ATTR_IS_WALKABLE] = new wxCheckBox(attrsPanel, ID_ATTR_IS_WALKABLE, "Is walkable");
	attrsPanelSizer->Add(curCb, 0, wxALL, 3);
	curCb = attrCheckboxes[ID_ATTR_IS_MOVABLE] = new wxCheckBox(attrsPanel, ID_ATTR_IS_MOVABLE, "Is movable");
	attrsPanelSizer->Add(curCb, 0, wxALL, 3);
	curCb = attrCheckboxes[ID_ATTR_IS_PATHABLE] = new wxCheckBox(attrsPanel, ID_ATTR_IS_PATHABLE, "Is pathable");
	attrsPanelSizer->Add(curCb, 0, wxALL, 3);
	curCb = attrCheckboxes[ID_ATTR_CAN_BE_HIDDEN] = new wxCheckBox(attrsPanel, ID_ATTR_CAN_BE_HIDDEN, "Can be hidden");
	attrsPanelSizer->Add(curCb, 0, wxALL, 3);
	curCb = attrCheckboxes[ID_ATTR_IS_HANGABLE] = new wxCheckBox(attrsPanel, ID_ATTR_IS_HANGABLE, "Is hangable");
	attrsPanelSizer->Add(curCb, 0, wxALL, 3);
	curCb = attrCheckboxes[ID_ATTR_IS_HOOK_SOUTH] = new wxCheckBox(attrsPanel, ID_ATTR_IS_HOOK_SOUTH, "Hook south");
	attrsPanelSizer->Add(curCb, 0, wxALL, 3);
	curCb = attrCheckboxes[ID_ATTR_IS_HOOK_EAST] = new wxCheckBox(attrsPanel, ID_ATTR_IS_HOOK_EAST, "Hook east");
	attrsPanelSizer->Add(curCb, 0, wxALL, 3);
	curCb = attrCheckboxes[ID_ATTR_IS_ROTATABLE] = new wxCheckBox(attrsPanel, ID_ATTR_IS_ROTATABLE, "Is rotatable");
	attrsPanelSizer->Add(curCb, 0, wxALL, 3);
	curCb = attrCheckboxes[ID_ATTR_IS_TRANSLUCENT] = new wxCheckBox(attrsPanel, ID_ATTR_IS_TRANSLUCENT, "Is translucent");
	attrsPanelSizer->Add(curCb, 0, wxALL, 3);
	curCb = attrCheckboxes[ID_ATTR_IS_LYING_CORPSE] = new wxCheckBox(attrsPanel, ID_ATTR_IS_LYING_CORPSE, "Is lying corpse");
	attrsPanelSizer->Add(curCb, 0, wxALL, 3);
	curCb = attrCheckboxes[ID_ATTR_IS_FULL_GROUND] = new wxCheckBox(attrsPanel, ID_ATTR_IS_FULL_GROUND, "Is full ground");
	attrsPanelSizer->Add(curCb, 0, wxALL, 3);
	curCb = attrCheckboxes[ID_ATTR_IGNORE_LOOK] = new wxCheckBox(attrsPanel, ID_ATTR_IGNORE_LOOK, "Ignore look");
	attrsPanelSizer->Add(curCb, 0, wxALL, 3);
	curCb = attrCheckboxes[ID_ATTR_IS_USABLE] = new wxCheckBox(attrsPanel, ID_ATTR_IS_USABLE, "Is usable");
	attrsPanelSizer->Add(curCb, 0, wxALL, 3);

	attrsPanel->SetSizer(attrsPanelSizer);
	attrsBox->Add(attrsPanel, 1, wxEXPAND);
	midColumnGrid->Add(attrsBox, 1, wxEXPAND);
	midColumnGrid->AddGrowableRow(0, 1);
	midColumnGrid->AddGrowableCol(0, 1);

	auto rightColumnGrid = new wxFlexGridSizer(2, 1, 5, 5);

	auto spritesBoxSizer = new wxStaticBoxSizer(wxVERTICAL, mainPanel, "Sprites");
	spritesPanel = new wxScrolledWindow(mainPanel, -1, wxDefaultPosition, wxSize(200, -1));
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
	mainPanel->SetSizer(vbox);
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

void MainWindow::buildAnimationSpriteHolders()
{
	animationMainGridSizer->Remove(4);
	if (animationSpritesPanel)
	{
		animationSpritesPanel->DestroyChildren();
		animationSpritesPanel->Destroy();
	}
	animationSpritesPanel = new wxPanel(animationPanel, -1);
	animationSpritesSizer = new wxGridSizer(animationHeight, animationWidth, 0, 0);

	wxBitmap spriteHolderBitmap;
	spriteHolderBitmap.LoadFile("res/misc/sprite_placeholder.png", wxBITMAP_TYPE_PNG);
	wxGenericStaticBitmap * spriteHolder = nullptr;
	for (unsigned int i = 0; i < animationWidth * animationHeight; ++i)
	{
		spriteHolder = new wxGenericStaticBitmap(animationSpritesPanel, -1, spriteHolderBitmap);
		animationSpritesSizer->Add(spriteHolder);
	}

	animationSpritesPanel->SetSizer(animationSpritesSizer, true);
	animationMainGridSizer->Insert(4, animationSpritesPanel);
	animationBoxExpandSizer->Layout();
	//animationMainGridSizer->Layout();
	//animationPanel->Fit();
}

void MainWindow::OnWidthOrHeightChanged(wxCommandEvent & event)
{
	if (animationWidthInput && animationHeightInput)
	{
		animationWidth = wxAtoi(animationWidthInput->GetValue());
		animationHeight = wxAtoi(animationHeightInput->GetValue());
		buildAnimationSpriteHolders();
	}
}

void MainWindow::OnExit(wxCommandEvent & event)
{
	Close(true);
}

void MainWindow::OnAbout(wxCommandEvent & event)
{
	wxMessageBox("Here will be some about text", "About", wxOK | wxICON_INFORMATION);
}
