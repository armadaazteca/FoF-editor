#include <wx/wxprec.h>
#ifndef WX_PRECOMP
#include <wx/wx.h>
#endif
#include <wx/filename.h>
#include <wx/notebook.h>
#include <wx/rawbmp.h>
#include "Config.h"
#include "Events.h"
#include "Utils.h"
#include "Settings.h"
#include "MainWindow.h"
#include "DatSprOpenSaveDialog.h"
#include "DatSprReaderWriter.h"

const wxString MainWindow::DIRECTION_QUESTION = "This object doesn't have \"%s\" direction yet, create it?";
const wxString MainWindow::DIRECTION_QUESTION_TITLE = "Create direction?";
const wxString MainWindow::CATEGORIES[] = { "Items", "Creatures", "Effects", "Projectiles" };

wxBEGIN_EVENT_TABLE(MainWindow, wxFrame)
	EVT_MENU(wxID_NEW, MainWindow::OnCreateNewFiles)
	EVT_MENU(wxID_OPEN, MainWindow::OnOpenDatSprDialog)
	EVT_MENU(wxID_SAVE, MainWindow::OnOpenDatSprDialog)
	EVT_MENU(wxID_EXIT, MainWindow::OnExit)
	EVT_MENU(wxID_ABOUT, MainWindow::OnAbout)
	EVT_COMMAND(wxID_ANY, DAT_SPR_LOADED, MainWindow::OnDatSprLoaded)
	EVT_COMBOBOX(ID_CATEGORIES_COMBOBOX, MainWindow::OnObjectCategoryChanged)
	EVT_LISTBOX(ID_OBJECTS_LISTBOX, MainWindow::OnObjectSelected)
	EVT_TEXT(ID_ANIM_WIDTH_INPUT, MainWindow::OnAnimWidthChanged)
	EVT_TEXT(ID_ANIM_HEIGHT_INPUT, MainWindow::OnAnimHeightChanged)
	EVT_TEXT(ID_FRAMES_AMOUNT_INPUT, MainWindow::OnFramesAmountChanged)
	EVT_BUTTON(ID_DIR_TOP_BUTTON, MainWindow::OnClickOrientationButton)
	EVT_BUTTON(ID_DIR_RIGHT_BUTTON, MainWindow::OnClickOrientationButton)
	EVT_BUTTON(ID_DIR_BOTTOM_BUTTON, MainWindow::OnClickOrientationButton)
	EVT_BUTTON(ID_DIR_LEFT_BUTTON, MainWindow::OnClickOrientationButton)
	EVT_BUTTON(ID_PREV_FRAME_BUTTON, MainWindow::OnClickPrevFrameButton)
	EVT_BUTTON(ID_NEXT_FRAME_BUTTON, MainWindow::OnClickNextFrameButton)
	EVT_BUTTON(ID_NEW_OBJECT_BUTTON, MainWindow::OnClickNewObjectButton)
	EVT_BUTTON(ID_IMPORT_SPRITES_BUTTON, MainWindow::OnClickImportSpriteButton)
wxEND_EVENT_TABLE()

MainWindow::MainWindow(const wxString & title, const wxPoint & pos, const wxSize & size) : wxFrame(NULL, wxID_ANY, title, pos, size)
{
	currentCategory = CategoryItem;

	CreateStatusBar();

	// constructing main menu
	auto menuFile = new wxMenu();
	menuFile->Append(wxID_NEW);
	menuFile->Append(wxID_OPEN);
	menuFile->Append(wxID_SAVE);
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

	auto leftColumnGrid = new wxFlexGridSizer(3, 1, 5, 5);
	categoryComboBox = new wxComboBox(mainPanel, ID_CATEGORIES_COMBOBOX, CATEGORIES[0],
			wxDefaultPosition, wxDefaultSize, 4, CATEGORIES, wxCB_READONLY);
	leftColumnGrid->Add(categoryComboBox, 1, wxEXPAND);
	objectsListBox = new wxListBox(mainPanel, ID_OBJECTS_LISTBOX, wxDefaultPosition, wxSize(220, -1), 0, NULL, wxLB_SINGLE);
	leftColumnGrid->Add(objectsListBox, 1, wxEXPAND);
	leftColumnGrid->AddGrowableRow(1, 1);
	auto newObjectButton = new wxButton(mainPanel, ID_NEW_OBJECT_BUTTON, "New object");
	leftColumnGrid->Add(newObjectButton, 1, wxEXPAND);

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
	animationPanelSizer->Add(animationMainGridSizer, 0, wxALIGN_CENTER);

	animationSpritesPanel = nullptr; // initializing
	animationSpritesSizer = nullptr; // to prevent warnings
	stubImageRgb = unique_ptr <unsigned char []> (new unsigned char[Sprite::RGB_SIZE]);
	stubImageAlpha = unique_ptr <unsigned char []> (new unsigned char[Sprite::ALPHA_SIZE]);
	for (int i = 0, j = 0; i < 1024; ++i, j += 3)
	{
		stubImageRgb[j] = stubImageRgb[j + 1] = stubImageRgb[j + 2] = stubImageAlpha[i] = 0;
	}
	stubImage = make_shared <wxImage> (32, 32, stubImageRgb.get(), stubImageAlpha.get(), true);

	// frame control
	auto frameNumberSizer = new wxBoxSizer(wxHORIZONTAL);
	auto currentFrameText = new wxStaticText(animationPanel, -1, "Current frame:");
	frameNumberSizer->Add(currentFrameText, 0, wxALL, 10);
	currentFrame = 0;
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
	animationWidthInput = new wxTextCtrl(animationPanel, ID_ANIM_WIDTH_INPUT, "1", wxDefaultPosition,
	                                     wxSize(25, -1), wxTE_RIGHT);
	widthAndHeightSizer->Add(animationWidthInput, 0, wxALL, 5);
	auto heightLabel = new wxStaticText(animationPanel, -1, "Height:");
	widthAndHeightSizer->Add(heightLabel, 0, wxLEFT | wxALIGN_CENTER_VERTICAL, 10);
	animationHeightInput = new wxTextCtrl(animationPanel, ID_ANIM_HEIGHT_INPUT, "1", wxDefaultPosition,
	                                      wxSize(25, -1), wxTE_RIGHT);
	widthAndHeightSizer->Add(animationHeightInput, 0, wxALL, 5);
	animationPanelSizer->Add(widthAndHeightSizer, 0, wxALIGN_CENTER);

	// amount of frames setting
		auto amountOfFramesSizer = new wxBoxSizer(wxHORIZONTAL);
	auto amountOfFramesLabel = new wxStaticText(animationPanel, -1, "Amount of frames:");
	amountOfFramesSizer->Add(amountOfFramesLabel, 0, wxALIGN_CENTER_VERTICAL);
	amountOfFramesInput = new wxTextCtrl(animationPanel, ID_FRAMES_AMOUNT_INPUT, "1", wxDefaultPosition,
	                                     wxSize(25, -1), wxTE_RIGHT);
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
	attrsPanel = new wxPanel(mainPanel, -1);
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
	for (int i = ID_ATTR_IS_GROUND_BORDER; i < ID_ATTR_LAST; ++i)
	{
		attrCheckboxes[i]->Bind(wxEVT_CHECKBOX, &MainWindow::OnToggleAttrCheckbox, this);
	}

	attrsPanel->SetSizer(attrsPanelSizer);
	attrsBox->Add(attrsPanel, 1, wxEXPAND);
	midColumnGrid->Add(attrsBox, 1, wxEXPAND);
	midColumnGrid->AddGrowableRow(0, 1);
	midColumnGrid->AddGrowableCol(0, 1);

	auto rightColumnGrid = new wxFlexGridSizer(1, 1, 5, 5);

	auto spriteTabs = new wxNotebook(mainPanel, -1);
	objectSpritesPanel = new wxScrolledWindow(spriteTabs, -1, wxDefaultPosition, wxSize(220, -1));
	spriteTabs->AddPage(objectSpritesPanel, "Object sprites", true);
	objectSpritesPanelSizer = new wxFlexGridSizer(2, 5, 5);
	objectSpritesPanelSizer->AddGrowableCol(1, 1);
	objectSpritesPanel->SetSizer(objectSpritesPanelSizer);
	objectSpritesPanel->SetScrollRate(0, 20);

	auto newSpritesOuterPanel = new wxPanel(spriteTabs, -1);
	auto newSpritesOuterSizer = new wxFlexGridSizer(2, 1, 5, 5);
	newSpritesOuterPanel->SetSizer(newSpritesOuterSizer);
	spriteTabs->AddPage(newSpritesOuterPanel, "New sprites");

	newSpritesPanel = new wxScrolledWindow(newSpritesOuterPanel, -1, wxDefaultPosition, wxSize(220, -1));
	newSpritesPanelSizer = new wxFlexGridSizer(2, 5, 5);
	newSpritesPanelSizer->AddGrowableCol(1, 1);
	newSpritesPanel->SetSizer(newSpritesPanelSizer);
	newSpritesPanel->SetScrollRate(0, 20);
	newSpritesOuterSizer->Add(newSpritesPanel, 1, wxEXPAND);

	auto importSpritesButton = new wxButton(newSpritesOuterPanel, ID_IMPORT_SPRITES_BUTTON, "Import sprites");
	newSpritesOuterSizer->Add(importSpritesButton, 1, wxEXPAND);

	newSpritesOuterSizer->AddGrowableRow(0, 1);
	newSpritesOuterSizer->AddGrowableCol(0, 1);

	rightColumnGrid->Add(spriteTabs, 1, wxEXPAND);
	rightColumnGrid->AddGrowableRow(0, 1);
	rightColumnGrid->AddGrowableCol(0, 1);

	fgs->Add(leftColumnGrid, 1, wxEXPAND);
	fgs->Add(midColumnGrid, 1, wxEXPAND);
	fgs->Add(rightColumnGrid, 1, wxEXPAND);
	fgs->AddGrowableRow(0, 1);
	fgs->AddGrowableCol(1, 1);

	vbox->Add(fgs, 1, wxALL | wxEXPAND, 10);
	mainPanel->SetSizer(vbox);

	// variables
	currentXDiv = currentYDiv = 0;

	// disabling main panel and all nested controls until correct .dat / .spr files will be loaded or created
	mainPanel->Disable();
	animationPanel->Disable();
	attrsPanel->Disable();
}

void MainWindow::OnCreateNewFiles(wxCommandEvent & event)
{
	// TODO: confirmation of unsaved previous changes required here
	categoryComboBox->SetSelection(0);
	objectsListBox->Clear();
	animationSpritesPanel->DestroyChildren();
	animationSpritesPanel->Destroy();
	animationSpritesPanel = nullptr;
	animationMainGridSizer->Insert(4, 32, 32);
	objectSpritesPanelSizer->Clear(true);
	newSpritesPanelSizer->Clear(true);

	mainPanel->Disable();
	animationPanel->Disable();
	attrsPanel->Disable();

	DatSprReaderWriter::getInstance().initNewData();
	mainPanel->Enable();
}

void MainWindow::OnOpenDatSprDialog(wxCommandEvent & event)
{
	// TODO: confirmation of unsaved previous changes required here
	bool isModeOpen = event.GetId() == wxID_OPEN;
	auto & dsrw = DatSprReaderWriter::getInstance();
	if (!isModeOpen)
	{
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
		if (!hasObjects)
		{
			wxMessageBox("There are no objects to save", "Error", wxOK | wxICON_ERROR);
			return;
		}
	}
	DatSprOpenSaveDialog dialog(this, (isModeOpen ? DatSprOpenSaveDialog::MODE_OPEN : DatSprOpenSaveDialog::MODE_SAVE));
	dialog.ShowModal();
}

void MainWindow::OnDatSprLoaded(wxCommandEvent & event)
{
	mainPanel->Enable();
	currentCategory = CategoryItem;
	fillObjectsListBox();
	auto objects = DatSprReaderWriter::getInstance().getObjects(currentCategory);
	selectedObject = objects->size() > 0 ? objects->at(0) : nullptr;
	if (selectedObject)
	{
		animationPanel->Enable();
		attrsPanel->Enable();
	}
	setAttributeValues();
	buildAnimationSpriteHolders();
	fillObjectSprites();
	fillAnimationSection();
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
	selectedObject = objects->size() > 0 ? objects->at(0) : nullptr;
	setAttributeValues();
	fillObjectSprites();
	fillAnimationSection();
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
	if (objectIds.size() > 0)
	{
		objectsListBox->InsertItems(objectIds, 0);
		objectsListBox->SetSelection(0);
	}
}

void MainWindow::OnObjectSelected(wxCommandEvent & event)
{
	animationPanel->Enable();
	attrsPanel->Enable();

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
	selectedObject = objects->at(objectId);
	setAttributeValues();
	fillObjectSprites();
	fillAnimationSection();
}

void MainWindow::setAttributeValues(bool isNewObject)
{
	// at first, resetting attributes to default
	for (int i = ID_ATTR_IS_GROUND_BORDER; i < ID_ATTR_LAST; ++i)
	{
		attrCheckboxes[i]->SetValue(false);
	}
	if (isNewObject)
	{
		memset(selectedObject->allAttrs, 0, AttrLast);
		selectedObject->allAttrs[AttrLast] = AttrLast;
	}

	if (!selectedObject) return;

	// then setting values
	if (selectedObject->isGroundBorder)
	{
		attrCheckboxes[ID_ATTR_IS_GROUND_BORDER]->SetValue(true);
		if (isNewObject)
		{
			selectedObject->allAttrs[AttrGroundBorder] = AttrGroundBorder;
		}
	}
	if (selectedObject->isOnTop)
	{
		attrCheckboxes[ID_ATTR_IS_ON_TOP]->SetValue(true);
		if (isNewObject)
		{
			selectedObject->allAttrs[AttrOnTop] = AttrOnTop;
		}
	}
	if (selectedObject->isContainer)
	{
		attrCheckboxes[ID_ATTR_IS_CONTAINER]->SetValue(true);
		if (isNewObject)
		{
			selectedObject->allAttrs[AttrContainer] = AttrContainer;
		}
	}
	if (selectedObject->isStackable)
	{
		attrCheckboxes[ID_ATTR_IS_STACKABLE]->SetValue(true);
		if (isNewObject)
		{
			selectedObject->allAttrs[AttrStackable] = AttrStackable;
		}
	}
	if (selectedObject->isForceUse)
	{
		attrCheckboxes[ID_ATTR_IS_FORCE_USE]->SetValue(true);
		if (isNewObject)
		{
			selectedObject->allAttrs[AttrForceUse] = AttrForceUse;
		}
	}
	if (selectedObject->isMultiUse)
	{
		attrCheckboxes[ID_ATTR_IS_MULTI_USE]->SetValue(true);
		if (isNewObject)
		{
			selectedObject->allAttrs[AttrMultiUse] = AttrMultiUse;
		}
	}
	if (selectedObject->isFluidContainer)
	{
		attrCheckboxes[ID_ATTR_IS_FLUID_CONTAINER]->SetValue(true);
		if (isNewObject)
		{
			selectedObject->allAttrs[AttrFluidContainer] = AttrFluidContainer;
		}
	}
	if (selectedObject->isSplash)
	{
		attrCheckboxes[ID_ATTR_IS_SPLASH]->SetValue(true);
		if (isNewObject)
		{
			selectedObject->allAttrs[AttrSplash] = AttrSplash;
		}
	}
	if (selectedObject->blocksProjectiles)
	{
		attrCheckboxes[ID_ATTR_BLOCKS_PROJECTILES]->SetValue(true);
		if (isNewObject)
		{
			selectedObject->allAttrs[AttrBlockProjectile] = AttrBlockProjectile;
		}
	}
	if (selectedObject->isPickupable)
	{
		attrCheckboxes[ID_ATTR_IS_PICKUPABLE]->SetValue(true);
		if (isNewObject)
		{
			selectedObject->allAttrs[AttrPickupable] = AttrPickupable;
		}
	}
	if (selectedObject->isWalkable)
	{
		attrCheckboxes[ID_ATTR_IS_WALKABLE]->SetValue(true);
	}
	else if (isNewObject)
	{
		selectedObject->allAttrs[AttrNotWalkable] = AttrNotWalkable;
	}
	if (selectedObject->isMovable)
	{
		attrCheckboxes[ID_ATTR_IS_MOVABLE]->SetValue(true);
	}
	else if (isNewObject)
	{
		selectedObject->allAttrs[AttrNotMoveable] = AttrNotMoveable;
	}
	if (selectedObject->isPathable)
	{
		attrCheckboxes[ID_ATTR_IS_PATHABLE]->SetValue(true);
	}
	else if (isNewObject)
	{
		selectedObject->allAttrs[AttrNotPathable] = AttrNotPathable;
	}
	if (selectedObject->canBeHidden)
	{
		attrCheckboxes[ID_ATTR_CAN_BE_HIDDEN]->SetValue(true);
	}
	else if (isNewObject)
	{
		selectedObject->allAttrs[AttrDontHide] = AttrDontHide;
	}
	if (selectedObject->isHangable)
	{
		attrCheckboxes[ID_ATTR_IS_HANGABLE]->SetValue(true);
		if (isNewObject)
		{
			selectedObject->allAttrs[AttrHangable] = AttrHangable;
		}
	}
	if (selectedObject->isHookSouth)
	{
		attrCheckboxes[ID_ATTR_IS_HOOK_SOUTH]->SetValue(true);
		if (isNewObject)
		{
			selectedObject->allAttrs[AttrHookSouth] = AttrHookSouth;
		}
	}
	if (selectedObject->isHookEast)
	{
		attrCheckboxes[ID_ATTR_IS_HOOK_EAST]->SetValue(true);
		if (isNewObject)
		{
			selectedObject->allAttrs[AttrHookEast] = AttrHookEast;
		}
	}
	if (selectedObject->isRotatable)
	{
		attrCheckboxes[ID_ATTR_IS_ROTATABLE]->SetValue(true);
		if (isNewObject)
		{
			selectedObject->allAttrs[AttrRotateable] = AttrRotateable;
		}
	}
	if (selectedObject->isTranslucent)
	{
		attrCheckboxes[ID_ATTR_IS_TRANSLUCENT]->SetValue(true);
		if (isNewObject)
		{
			selectedObject->allAttrs[AttrTranslucent] = AttrTranslucent;
		}
	}
	if (selectedObject->isLyingCorpse)
	{
		attrCheckboxes[ID_ATTR_IS_LYING_CORPSE]->SetValue(true);
		if (isNewObject)
		{
			selectedObject->allAttrs[AttrLyingCorpse] = AttrLyingCorpse;
		}
	}
	if (selectedObject->isFullGround)
	{
		attrCheckboxes[ID_ATTR_IS_FULL_GROUND]->SetValue(true);
		if (isNewObject)
		{
			selectedObject->allAttrs[AttrFullGround] = AttrFullGround;
		}
	}
	if (selectedObject->ignoreLook)
	{
		attrCheckboxes[ID_ATTR_IGNORE_LOOK]->SetValue(true);
		if (isNewObject)
		{
			selectedObject->allAttrs[AttrLook] = AttrLook;
		}
	}
	if (selectedObject->isUsable)
	{
		attrCheckboxes[ID_ATTR_IS_USABLE]->SetValue(true);
		if (isNewObject)
		{
			selectedObject->allAttrs[AttrUsable] = AttrUsable;
		}
	}
}

void MainWindow::OnToggleAttrCheckbox(wxCommandEvent & event)
{
	bool value = (bool) event.GetInt();
	switch (event.GetId())
	{
		case ID_ATTR_IS_GROUND_BORDER:
			selectedObject->isGroundBorder = value;
		break;
		case ID_ATTR_IS_ON_TOP:
			selectedObject->isOnTop = value;
		break;
		case ID_ATTR_IS_CONTAINER:
			selectedObject->isContainer = value;
		break;
		case ID_ATTR_IS_STACKABLE:
			selectedObject->isStackable = value;
		break;
		case ID_ATTR_IS_FORCE_USE:
			selectedObject->isForceUse = value;
		break;
		case ID_ATTR_IS_MULTI_USE:
			selectedObject->isMultiUse = value;
		break;
		case ID_ATTR_IS_FLUID_CONTAINER:
			selectedObject->isFluidContainer = value;
		break;
		case ID_ATTR_IS_SPLASH:
			selectedObject->isSplash = value;
		break;
		case ID_ATTR_BLOCKS_PROJECTILES:
			selectedObject->blocksProjectiles = value;
		break;
		case ID_ATTR_IS_PICKUPABLE:
			selectedObject->isPickupable = value;
		break;
		case ID_ATTR_IS_WALKABLE:
			selectedObject->isWalkable = value;
		break;
		case ID_ATTR_IS_MOVABLE:
			selectedObject->isMovable = value;
		break;
		case ID_ATTR_IS_PATHABLE:
			selectedObject->isPathable = value;
		break;
		case ID_ATTR_CAN_BE_HIDDEN:
			selectedObject->canBeHidden = value;
		break;
		case ID_ATTR_IS_HANGABLE:
			selectedObject->isHangable = value;
		break;
		case ID_ATTR_IS_HOOK_SOUTH:
			selectedObject->isHookSouth = value;
		break;
		case ID_ATTR_IS_HOOK_EAST:
			selectedObject->isHookEast = value;
		break;
		case ID_ATTR_IS_ROTATABLE:
			selectedObject->isRotatable = value;
		break;
		case ID_ATTR_IS_TRANSLUCENT:
			selectedObject->isTranslucent = value;
		break;
		case ID_ATTR_IS_LYING_CORPSE:
			selectedObject->isLyingCorpse = value;
		break;
		case ID_ATTR_IS_FULL_GROUND:
			selectedObject->isFullGround = value;
		break;
		case ID_ATTR_IGNORE_LOOK:
			selectedObject->ignoreLook = value;
		break;
		case ID_ATTR_IS_USABLE:
			selectedObject->isUsable = value;
		break;
	}
}

void MainWindow::fillObjectSprites()
{
	objectSpritesPanelSizer->Clear(true);

	if (!selectedObject) return;

	auto sprites = DatSprReaderWriter::getInstance().getSprites();
	shared_ptr <Sprite> sprite = nullptr;
	wxGenericStaticBitmap * staticBitmap = nullptr;
	wxStaticText * spriteIdLabel = nullptr;

	bool hasAtLeastOneSprite = false;
	unsigned int spriteId = 0;
	for (unsigned int i = 0; i < selectedObject->spriteCount; ++i)
	{
		spriteId = selectedObject->spriteIDs[i];
		if (sprites->find(spriteId) != sprites->end())
		{
			sprite = sprites->at(spriteId);
			if (sprite->valid)
			{
				hasAtLeastOneSprite = true;
				wxImage image(32, 32, sprite->rgb.get(), sprite->alpha.get(), true);
				wxBitmap bitmap(image);
				staticBitmap = new wxGenericStaticBitmap(objectSpritesPanel, -1, bitmap);

				auto esi = new EditorSpriteIDs();
				esi->setType(EditorSpriteIDs::EXISTING);
				esi->getSpriteIDs().push_back(sprite->id);
				esi->setIndexInVector(editorSpriteIDs.size());
				editorSpriteIDs.push_back(esi);
				staticBitmap->SetClientData(esi);

				staticBitmap->Bind(wxEVT_LEFT_DOWN, &MainWindow::OnClickImportedOrObjectSprite, this);

				objectSpritesPanelSizer->Add(staticBitmap, 0, wxALL, 5);
				spriteIdLabel = new wxStaticText(objectSpritesPanel, -1, wxString::Format("%i", spriteId));
				objectSpritesPanelSizer->Add(spriteIdLabel, 0, wxALL | wxALIGN_CENTER_VERTICAL, 5);
			}
		}
	}

	if (!hasAtLeastOneSprite)
	{
		auto empty = new wxStaticText(objectSpritesPanel, -1, "This object has no sprites");
		objectSpritesPanelSizer->Add(empty, 0, wxALL, 5);
	}

	objectSpritesPanelSizer->Layout();
	objectSpritesPanel->FitInside();
}

void MainWindow::fillAnimationSection()
{
	if (!selectedObject) return;

	animationWidthInput->ChangeValue(wxString::Format("%i", selectedObject->width));
	animationHeightInput->ChangeValue(wxString::Format("%i", selectedObject->height));

	currentFrame = currentXDiv = currentYDiv = 0;
	currentFrameNumber->SetLabelText(wxString::Format("%i", currentFrame));
	amountOfFramesInput->ChangeValue(wxString::Format("%i", selectedObject->phasesCount));

	alwaysAnimatedCheckbox->SetValue(selectedObject->isAlwaysAnimated);

	fillAnimationSprites();
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
	animationSpritesSizer = new wxGridSizer(selectedObject->height, selectedObject->width, 0, 0);

	for (unsigned int i = 0; i < selectedObject->width * selectedObject->height; ++i)
	{
		animationSpriteBitmaps[i] = new wxGenericStaticBitmap(animationSpritesPanel, -1, *stubImage);
		animationSpritesSizer->Add(animationSpriteBitmaps[i]);

		// for sprites drag & drop
		auto dropTarget = new AnimationSpriteDropTarget();
		dropTarget->setMainWindow(this);
		dropTarget->setSpriteHolderIndex(i);
		animationSpriteBitmaps[i]->SetDropTarget(dropTarget);

		auto onEnter = [=](wxMouseEvent & event)
		{
			wxGenericStaticBitmap * sBmp = dynamic_cast <wxGenericStaticBitmap *> (event.GetEventObject());
			drawAnimationSpriteSelection(sBmp);
		};
		auto onLeave = [=](wxMouseEvent & event)
		{
			wxGenericStaticBitmap * sBmp = dynamic_cast <wxGenericStaticBitmap *> (event.GetEventObject());
			clearAnimationSpriteSelection(sBmp);
		};
		animationSpriteBitmaps[i]->Bind(wxEVT_ENTER_WINDOW, onEnter);
		animationSpriteBitmaps[i]->Bind(wxEVT_LEAVE_WINDOW, onLeave);
	}

	animationSpritesPanel->SetSizer(animationSpritesSizer, true);
	animationMainGridSizer->Insert(4, animationSpritesPanel);
	animationBoxExpandSizer->Layout();
}

void MainWindow::fillAnimationSprites()
{
	auto sprites = DatSprReaderWriter::getInstance().getSprites();
	shared_ptr <Sprite> sprite = nullptr;
	unsigned int spriteId = 0;

	int frameSize = selectedObject->width * selectedObject->height;
	for (int layer = 0; layer < selectedObject->layersCount; ++layer)
	{
		int startSprite = (currentXDiv + currentFrame * selectedObject->patternWidth + layer) * frameSize;
		int endSprite = startSprite + frameSize;
		for (int i = endSprite - 1, j = 0; i >= startSprite; --i, ++j)
		{
			spriteId = selectedObject->spriteIDs[i];
			if (spriteId != 0 && sprites->find(spriteId) != sprites->end())
			{
				sprite = sprites->at(spriteId);
				if (sprite->valid)
				{
					if (layer == 0) // filling bottom layer
					{
						wxImage image(32, 32, sprite->rgb.get(), sprite->alpha.get(), true);
						wxBitmap bitmap(image);
						animationSpriteBitmaps[j]->SetBitmap(bitmap);
					}
					else
					{
						wxMemoryDC oldImageMemDC;
						oldImageMemDC.SelectObjectAsSource(animationSpriteBitmaps[j]->GetBitmap());
						wxImage newImage(32, 32, sprite->rgb.get(), sprite->alpha.get(), true);
						wxBitmap newBitmap(newImage);
						oldImageMemDC.DrawBitmap(newBitmap, 0, 0, true);
					}
				}
				else
				{
					if (layer == 0) // filling bottom layer
					{
						wxBitmap emptyBitmap(*stubImage);
						animationSpriteBitmaps[j]->SetBitmap(emptyBitmap);
					}
				}
			}
			else
			{
				if (layer == 0) // filling bottom layer
				{
					wxBitmap emptyBitmap(*stubImage);
					animationSpriteBitmaps[j]->SetBitmap(emptyBitmap);
				}
			}
		}
	}
}

void MainWindow::OnAnimWidthChanged(wxCommandEvent & event)
{
	if (!animationWidthInput) return;
	wxString strval = animationWidthInput->GetValue();
	if (strval.Length() == 0) return;
	unsigned int val = wxAtoi(strval);
	if (val <= 1) val = 1;
	else if (val > Config::MAX_OBJECT_WIDTH) val = Config::MAX_OBJECT_WIDTH;
	animationWidthInput->ChangeValue(wxString::Format("%i", val));
	selectedObject->width = val;
	resizeObjectSpriteIDsArray(selectedObject);
	buildAnimationSpriteHolders();
	fillAnimationSprites();
}

void MainWindow::OnAnimHeightChanged(wxCommandEvent & event)
{
	if (!animationHeightInput) return;
	wxString strval = animationHeightInput->GetValue();
	if (strval.Length() == 0) return;
	unsigned int val = wxAtoi(strval);
	if (val <= 1) val = 1;
	else if (val > Config::MAX_OBJECT_HEIGHT) val = Config::MAX_OBJECT_HEIGHT;
	animationHeightInput->ChangeValue(wxString::Format("%i", val));
	selectedObject->height = val;
	resizeObjectSpriteIDsArray(selectedObject);
	buildAnimationSpriteHolders();
	fillAnimationSprites();
}

void MainWindow::OnFramesAmountChanged(wxCommandEvent & event)
{
	if (!amountOfFramesInput) return;
	wxString strval = amountOfFramesInput->GetValue();
	if (strval.Length() == 0) return;
	unsigned int val = wxAtoi(strval);
	if (val <= 1) val = 1;
	else if (val > Config::MAX_ANIM_FRAMES) val = Config::MAX_ANIM_FRAMES;
	amountOfFramesInput->ChangeValue(wxString::Format("%i", val));
	selectedObject->phasesCount = val;
	resizeObjectSpriteIDsArray(selectedObject);
	buildAnimationSpriteHolders();
	fillAnimationSprites();
}

void MainWindow::OnClickOrientationButton(wxCommandEvent & event)
{
	unsigned int xDiv = 0;
	switch (event.GetId())
	{
		case ID_DIR_TOP_BUTTON:
			xDiv = ORIENT_NORTH;
		break;
		case ID_DIR_RIGHT_BUTTON:
			if (selectedObject->patternWidth < 2)
			{
				wxMessageDialog confirmation(this, wxString::Format(DIRECTION_QUESTION, "east"), DIRECTION_QUESTION_TITLE, wxYES_NO | wxCANCEL);
				if (confirmation.ShowModal() == wxID_YES)
				{
					selectedObject->patternWidth = 2;
					resizeObjectSpriteIDsArray(selectedObject);
					xDiv = ORIENT_EAST;
				}
			}
			else
			{
				xDiv = ORIENT_EAST;
			}
		break;
		case ID_DIR_BOTTOM_BUTTON:
			if (selectedObject->patternWidth < 3)
			{
				wxMessageDialog confirmation(this, wxString::Format(DIRECTION_QUESTION, "south"), DIRECTION_QUESTION_TITLE, wxYES_NO | wxCANCEL);
				if (confirmation.ShowModal() == wxID_YES)
				{
					selectedObject->patternWidth = 3;
					resizeObjectSpriteIDsArray(selectedObject);
					xDiv = ORIENT_SOUTH;
				}
			}
			else
			{
				xDiv = ORIENT_SOUTH;
			}
		break;
		case ID_DIR_LEFT_BUTTON:
			if (selectedObject->patternWidth < 4)
			{
				wxMessageDialog confirmation(this, wxString::Format(DIRECTION_QUESTION, "west"), DIRECTION_QUESTION_TITLE, wxYES_NO | wxCANCEL);
				if (confirmation.ShowModal() == wxID_YES)
				{
					selectedObject->patternWidth = 4;
					resizeObjectSpriteIDsArray(selectedObject);
					xDiv = ORIENT_WEST;
				}
			}
			else
			{
				xDiv = ORIENT_WEST;
			}
		break;
	}
	if (xDiv < selectedObject->patternWidth)
	{
		currentXDiv = xDiv;
	}

	// resetting frame when switching orientation
	currentFrame = 0;
	currentFrameNumber->SetLabelText(wxString::Format("%i", currentFrame));

	fillAnimationSprites();
}

void MainWindow::OnClickPrevFrameButton(wxCommandEvent & event)
{
	if (currentFrame == 0) currentFrame = selectedObject->phasesCount - 1;
	else currentFrame--;
	currentFrameNumber->SetLabelText(wxString::Format("%i", currentFrame));
	fillAnimationSprites();
}

void MainWindow::OnClickNextFrameButton(wxCommandEvent & event)
{
	currentFrame++;
	if (currentFrame >= selectedObject->phasesCount) currentFrame = 0;
	currentFrameNumber->SetLabelText(wxString::Format("%i", currentFrame));
	fillAnimationSprites();
}

void MainWindow::OnClickNewObjectButton(wxCommandEvent & event)
{
	auto objects = DatSprReaderWriter::getInstance().getObjects(currentCategory);
	auto newObject = make_shared <DatObject> ();
	newObject->id = objects->size() + (currentCategory == CategoryItem ? 100 : 1);
	newObject->width = newObject->height = 1;
	newObject->patternWidth = newObject->patternHeight = newObject->patternDepth = 1;
	newObject->layersCount = newObject->phasesCount = newObject->spriteCount = 1;
	newObject->spriteIDs = unique_ptr <unsigned int[]> (new unsigned int[1] { 0 });
	newObject->allAttrs[AttrLast] = AttrLast;
	objects->push_back(newObject);
	selectedObject = newObject;
	objectsListBox->Insert(wxString::Format("%i", newObject->id), objectsListBox->GetCount());
	objectsListBox->SetSelection(objectsListBox->GetCount() - 1);

	animationPanel->Enable();
	attrsPanel->Enable();

	setAttributeValues();
	buildAnimationSpriteHolders();
	fillObjectSprites();
	fillAnimationSection();
}

void MainWindow::OnClickImportSpriteButton(wxCommandEvent & event)
{
	Settings & settings = Settings::getInstance();
	const wxString & spriteImportPath = settings.get("lastSpriteImportPath");
	wxFileDialog browseDialog(this, wxFileSelectorPromptStr, spriteImportPath, "", "", wxFD_MULTIPLE);
	if (browseDialog.ShowModal() == wxID_OK)
	{
		wxArrayString paths;
		browseDialog.GetPaths(paths);
		// saving directory path for future usage
		settings.set("lastSpriteImportPath", wxFileName(paths[0]).GetPath());
		settings.save();

		shared_ptr <wxBitmap> bitmap = nullptr;
		wxGenericStaticBitmap * staticBitmap = nullptr;
		wxStaticText * spriteLabel = nullptr;
		wxFileName filename;
		unsigned int bitmapId = 0;
		for (auto const & p : paths)
		{
			bitmapId = importedSprites.size();
			bitmap = make_shared <wxBitmap> ();
			bitmap->LoadFile(p, wxBITMAP_TYPE_PNG);
			importedSprites.push_back(bitmap);
			staticBitmap = new wxGenericStaticBitmap(newSpritesPanel, -1, *bitmap);

			auto esi = new EditorSpriteIDs();
			esi->setType(EditorSpriteIDs::IMPORTED);
			esi->getSpriteIDs().push_back(bitmapId);
			esi->setIndexInVector(editorSpriteIDs.size());
			editorSpriteIDs.push_back(esi);
			staticBitmap->SetClientData(esi);

			staticBitmap->Bind(wxEVT_LEFT_DOWN, &MainWindow::OnClickImportedOrObjectSprite, this);

			newSpritesPanelSizer->Add(staticBitmap, 0, wxALL, 5);
			filename.Assign(p);
			spriteLabel = new wxStaticText(newSpritesPanel, -1, filename.GetName());
			newSpritesPanelSizer->Add(spriteLabel, 0, wxALL | wxALIGN_CENTER_VERTICAL, 5);
		}
		newSpritesPanelSizer->Layout();
		newSpritesPanel->FitInside();
	}
}

void MainWindow::OnClickImportedOrObjectSprite(wxMouseEvent & event)
{
	auto staticBitmap = dynamic_cast <wxGenericStaticBitmap *> (event.GetEventObject());
	wxDropSource dragSource(staticBitmap);
	auto esi = (EditorSpriteIDs *) staticBitmap->GetClientData();
	char strEsiIndex[5];
	sprintf(strEsiIndex, "%i", esi->getIndexInVector());
	wxTextDataObject data(strEsiIndex);
	dragSource.SetData(data);
	dragSource.DoDragDrop(true);
}

void MainWindow::drawAnimationSpriteSelection(wxGenericStaticBitmap * staticBitmap)
{
	auto currentBitmap = new wxBitmap(staticBitmap->GetBitmap().GetSubBitmap(wxRect(0, 0, 32, 32)));
	staticBitmap->SetClientData(currentBitmap);
	wxBitmap tmpBmp(32, 32);
	wxMemoryDC tmpDC;
	tmpDC.SelectObject(tmpBmp);
	tmpDC.Clear();
	tmpDC.SetBrush(*wxTRANSPARENT_BRUSH);
	tmpDC.DrawBitmap(*currentBitmap, 0, 0);
	tmpDC.SetPen(*wxBLACK_PEN);
	tmpDC.DrawRectangle(0, 0, 32, 32);
	tmpDC.SetPen(*wxWHITE_PEN);
	tmpDC.DrawRectangle(1, 1, 30, 30);
	tmpDC.SelectObject(wxNullBitmap);
	staticBitmap->SetBitmap(tmpBmp);
}

void MainWindow::clearAnimationSpriteSelection(wxGenericStaticBitmap * staticBitmap)
{
	if (staticBitmap->GetClientData())
	{
		auto bmp = reinterpret_cast <wxBitmap *> (staticBitmap->GetClientData());
		staticBitmap->SetBitmap(*bmp);
		staticBitmap->SetClientData(nullptr);
		delete bmp;
	}
}

void MainWindow::resizeObjectSpriteIDsArray(shared_ptr <DatObject> object)
{
	auto oldSpriteCount = object->spriteCount;
	auto oldSpriteIDs = unique_ptr <unsigned int[]> (object->spriteIDs.release());
	object->spriteCount = object->width * object->height * object->layersCount * object->patternWidth
                      * object->patternHeight * object->patternDepth * object->phasesCount;
	object->spriteIDs = unique_ptr <unsigned int[]> (new unsigned int[object->spriteCount]);
	for (unsigned int i = 0; i < oldSpriteCount; ++i)
	{
		object->spriteIDs[i] = oldSpriteIDs[i];
	}
	for (unsigned int i = oldSpriteCount; i < object->spriteCount; ++i)
	{
		object->spriteIDs[i] = 0;
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

MainWindow::~MainWindow()
{
	for (auto esi : editorSpriteIDs)
	{
		delete esi;
	}
}

bool MainWindow::AnimationSpriteDropTarget::OnDropText(wxCoord x, wxCoord y, const wxString & data)
{
	bool result = false;

	unsigned int esiIndexInVector = wxAtoi(data);
	auto esi = mainWindow->editorSpriteIDs[esiIndexInVector];
	auto obj = mainWindow->selectedObject;
	unsigned int frameSize = obj->width * obj->height;
	unsigned int spriteIdStartIndex = (mainWindow->currentXDiv + mainWindow->currentFrame
	                                * obj->patternWidth) * frameSize + (frameSize - 1 - spriteHolderIndex);

	if (esi->getType() == EditorSpriteIDs::IMPORTED) // imported sprite
	{
		auto sprites = DatSprReaderWriter::getInstance().getSprites();
		auto & esiSpriteIDs = esi->getSpriteIDs();
		auto bitmap = mainWindow->importedSprites[esiSpriteIDs[0]];
		auto image = bitmap->ConvertToImage();

		void * pdp = nullptr; // pixel data pointer
		bool hasAlpha = image.HasAlpha();
		if (hasAlpha)
		{
			pdp = new wxAlphaPixelData(*bitmap);
		}
		else
		{
			pdp = new wxNativePixelData(*bitmap);
		}

		int width = ceil(bitmap->GetWidth() / (float) Config::SPRITE_SIZE);
		int height = ceil(bitmap->GetHeight() / (float) Config::SPRITE_SIZE);
		esi->setType(MainWindow::EditorSpriteIDs::EXISTING);
		esi->setBigSpriteSize(width, height);
		esiSpriteIDs.clear();
		for (int h = 0; h < height; ++h)
		{
			for (int w = 0; w < width; ++w)
			{
				auto sprite = make_shared <Sprite> ();
				sprite->id = DatSprReaderWriter::getInstance().incrementMaxSpriteId();
				sprite->rgb = unique_ptr <unsigned char[]> (new unsigned char[Sprite::RGB_SIZE]);
				sprite->alpha = unique_ptr <unsigned char[]> (new unsigned char[Sprite::ALPHA_SIZE]);

				if (hasAlpha)
				{
					sprite->hasRealAlpha = true;
					wxAlphaPixelData * apdp = (wxAlphaPixelData *) pdp;
					auto it = apdp->GetPixels();
					int x = w * Config::SPRITE_SIZE;
					int y = h * Config::SPRITE_SIZE;
					it.MoveTo(*apdp, x, y);
					for (int oy = 0; oy < Config::SPRITE_SIZE; ++oy)
					{
						it.MoveTo(*apdp, x, y + oy);
						int dataStart = Config::SPRITE_SIZE * oy, dataEnd = dataStart + Config::SPRITE_SIZE;
						for (int i = dataStart * 3, j = dataStart; j < dataEnd; i += 3, ++j)
						{
							assert(i < Sprite::RGB_SIZE);
							assert(j < Sprite::ALPHA_SIZE);
							sprite->rgb[i] = it.Red();
							sprite->rgb[i + 1] = it.Green();
							sprite->rgb[i + 2] = it.Blue();
							sprite->alpha[j] = it.Alpha();
							++it;
						}
					}
				}
				else
				{
					wxNativePixelData * npdp = (wxNativePixelData *) pdp;
					auto it = npdp->GetPixels();
					int x = w * Config::SPRITE_SIZE;
					int y = h * Config::SPRITE_SIZE;
					it.MoveTo(*npdp, x, y);
					for (int oy = 0; oy < Config::SPRITE_SIZE; ++oy)
					{
						it.MoveTo(*npdp, x, y + oy);
						int dataStart = Config::SPRITE_SIZE * oy, dataEnd = dataStart + Config::SPRITE_SIZE;
						for (int i = dataStart * 3, j = dataStart; j < dataEnd; i += 3, ++j)
						{
							assert(i < Sprite::RGB_SIZE);
							assert(j < Sprite::ALPHA_SIZE);
							sprite->rgb[i] = it.Red();
							sprite->rgb[i + 1] = it.Green();
							sprite->rgb[i + 2] = it.Blue();
							sprite->alpha[j] = 255;
							++it;
						}
					}
				}

				sprite->valid = sprite->changed = true;
				(*sprites)[sprite->id] = sprite;
				unsigned int spriteIndex = spriteIdStartIndex - h * obj->width - w;
				if (spriteIndex >= 0 && spriteIndex < obj->spriteCount)
				{
					obj->spriteIDs[spriteIndex] = sprite->id;
				}

				esiSpriteIDs.push_back(sprite->id);
			}
		}

		if (hasAlpha)
		{
			wxAlphaPixelData * apdp = (wxAlphaPixelData *) pdp;
			delete apdp;
		}
		else
		{
			wxNativePixelData * npdp = (wxNativePixelData *) pdp;
			delete npdp;
		}

		mainWindow->fillObjectSprites();
		mainWindow->fillAnimationSprites();

		result = true;
	}
	else if (esi->getType() == EditorSpriteIDs::EXISTING) // existing sprite
	{
		auto esiSpriteIDs = esi->getSpriteIDs();
		unsigned int spriteID = 0;
		int width = esi->getBigSpriteWidth(), height = esi->getBigSpriteHeight();
		for (int h = 0; h < height; ++h)
		{
			for (int w = 0; w < width; ++w)
			{
				unsigned int spriteIndex = spriteIdStartIndex - h * obj->width - w;
				if (spriteIndex >= 0 && spriteIndex < obj->spriteCount)
				{
					obj->spriteIDs[spriteIndex] = esiSpriteIDs[spriteID++];
				}
			}
		}
		mainWindow->fillObjectSprites();
		mainWindow->fillAnimationSprites();

		result = true;
	}

	// resetting cached bitmap used for selection handling
	auto staticBitmap = mainWindow->animationSpriteBitmaps[spriteHolderIndex];
	staticBitmap->SetClientData(nullptr);

	return result;
}

wxDragResult MainWindow::AnimationSpriteDropTarget::OnEnter(wxCoord x, wxCoord y, wxDragResult defResult)
{
	auto sBmp = mainWindow->animationSpriteBitmaps[spriteHolderIndex];
	mainWindow->drawAnimationSpriteSelection(sBmp);
	return defResult;
}

void MainWindow::AnimationSpriteDropTarget::OnLeave()
{
	auto sBmp = mainWindow->animationSpriteBitmaps[spriteHolderIndex];
	mainWindow->clearAnimationSpriteSelection(sBmp);
}
