#include <math.h>
#include <fstream>
#include <wx/wxprec.h>
#ifndef WX_PRECOMP
#include <wx/wx.h>
#endif
#include <wx/filename.h>
#include <wx/notebook.h>
#include <wx/rawbmp.h>
#include <wx/valnum.h>
#include "Config.h"
#include "Events.h"
#include "Utils.h"
#include "Settings.h"
#include "MainWindow.h"
#include "DatSprOpenSaveDialog.h"
#include "GenerateRMEDialog.h"
#include "QuickGuideDialog.h"
#include "AboutDialog.h"
#include "DatSprReaderWriter.h"

const wxString & MainWindow::DIRECTION_QUESTION = "This object doesn't have \"%s\" direction yet, create it?";
const wxString & MainWindow::DIRECTION_QUESTION_TITLE = "Create direction?";
const wxString MainWindow::CATEGORIES[] = { "Items", "Creatures", "Effects", "Projectiles" };
const wxString & STR_ZERO = "0";
const wxString & OBJ_ATTR_CHANGE_STATUS_MSG = "Object \"%s\" attribute set to %s";

wxBEGIN_EVENT_TABLE(MainWindow, wxFrame)
	EVT_CLOSE(MainWindow::OnClose)
	EVT_MENU(wxID_NEW, MainWindow::OnCreateNewFiles)
	EVT_MENU(wxID_OPEN, MainWindow::OnOpenDatSprDialog)
	EVT_MENU(wxID_SAVE, MainWindow::OnOpenDatSprDialog)
	EVT_MENU(wxID_EXIT, MainWindow::OnExit)
	EVT_MENU(ID_MENU_GENERATE_RME, MainWindow::OnGenerateRMEDialog)
	EVT_MENU(ID_MENU_QUICK_GUIDE, MainWindow::OnQuickGuide)
	EVT_MENU(wxID_ABOUT, MainWindow::OnAbout)
	EVT_COMMAND(wxID_ANY, DAT_SPR_LOADED, MainWindow::OnDatSprLoaded)
	EVT_COMMAND(wxID_ANY, DAT_SPR_SAVED, MainWindow::OnDatSprSaved)
	EVT_COMBOBOX(ID_CATEGORIES_COMBOBOX, MainWindow::OnObjectCategoryChanged)
	EVT_LISTBOX(ID_OBJECTS_LISTBOX, MainWindow::OnObjectSelected)
	EVT_TEXT(ID_ANIM_WIDTH_INPUT, MainWindow::OnAnimWidthChanged)
	EVT_TEXT(ID_ANIM_HEIGHT_INPUT, MainWindow::OnAnimHeightChanged)
	EVT_TEXT(ID_FRAMES_AMOUNT_INPUT, MainWindow::OnFramesAmountChanged)
	EVT_BUTTON(ID_DIR_TOP_LEFT_BUTTON, MainWindow::OnClickOrientationButton)
	EVT_BUTTON(ID_DIR_TOP_BUTTON, MainWindow::OnClickOrientationButton)
	EVT_BUTTON(ID_DIR_TOP_RIGHT_BUTTON, MainWindow::OnClickOrientationButton)
	EVT_BUTTON(ID_DIR_LEFT_BUTTON, MainWindow::OnClickOrientationButton)
	EVT_BUTTON(ID_DIR_RIGHT_BUTTON, MainWindow::OnClickOrientationButton)
	EVT_BUTTON(ID_DIR_BOTTOM_LEFT_BUTTON, MainWindow::OnClickOrientationButton)
	EVT_BUTTON(ID_DIR_BOTTOM_BUTTON, MainWindow::OnClickOrientationButton)
	EVT_BUTTON(ID_DIR_BOTTOM_RIGHT_BUTTON, MainWindow::OnClickOrientationButton)
	EVT_BUTTON(ID_PREV_FRAME_BUTTON, MainWindow::OnClickPrevFrameButton)
	EVT_BUTTON(ID_NEXT_FRAME_BUTTON, MainWindow::OnClickNextFrameButton)
	EVT_BUTTON(ID_NEW_OBJECT_BUTTON, MainWindow::OnClickNewObjectButton)
	EVT_BUTTON(ID_IMPORT_SPRITES_BUTTON, MainWindow::OnClickImportSpriteButton)
	EVT_CHECKBOX(ID_ALWAYS_ANIMATED_CHECKBOX, MainWindow::OnToggleAlwaysAnimatedAttr)
	EVT_CHECKBOX(ID_ATTR_IS_FULL_GROUND, MainWindow::OnToggleIsFullGroundAttr)
	EVT_CHECKBOX(ID_ATTR_HAS_LIGHT, MainWindow::OnToggleHasLightAttr)
	EVT_CHECKBOX(ID_ATTR_HAS_OFFSET, MainWindow::OnToggleHasOffsetAttr)
	EVT_CHECKBOX(ID_ATTR_HAS_ELEVATION, MainWindow::OnToggleHasElevationAttr)
	EVT_TEXT(ID_GROUND_SPEED_INPUT, MainWindow::OnGroundSpeedChanged)
	EVT_COLOURPICKER_CHANGED(ID_LIGHT_COLOR_PICKER, MainWindow::OnLightColorChanged)
	EVT_TEXT(ID_LIGHT_INTENSITY_INPUT, MainWindow::OnLightIntensityChanged)
	EVT_TEXT(ID_OFFSET_X_INPUT, MainWindow::OnOffsetXYChanged)
	EVT_TEXT(ID_OFFSET_Y_INPUT, MainWindow::OnOffsetXYChanged)
	EVT_TEXT(ID_ELEVATION_INPUT, MainWindow::OnElevationChanged)
wxEND_EVENT_TABLE()

MainWindow::MainWindow(const wxString & title, const wxPoint & pos, const wxSize & size) : wxFrame(NULL, wxID_ANY, title, pos, size)
{
	currentCategory = CategoryItem;

	statusBar = CreateStatusBar();
	statusBar->SetStatusText("\"File -> New\" or Ctrl+N to create new files; \"File -> Open\" or Ctrl+O to open existing files");

	// constructing main menu
	auto menuFile = new wxMenu();
	menuFile->Append(wxID_NEW);
	menuFile->Append(wxID_OPEN);
	menuFile->Append(wxID_SAVE);
	menuFile->AppendSeparator();
	menuFile->Append(wxID_EXIT);
	auto menuTools = new wxMenu();
	menuTools->Append(ID_MENU_GENERATE_RME, "Generate RME resources");
	auto menuHelp = new wxMenu();
	menuHelp->Append(ID_MENU_QUICK_GUIDE, "Quick guide");
	menuHelp->Append(wxID_ABOUT);
	auto menuBar = new wxMenuBar();
	menuBar->Append(menuFile, "&File");
	menuBar->Append(menuTools, "&Tools");
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

	auto onMouseWheel = [&](wxMouseEvent & event)
	{
		objectsListBox->ScrollLines((event.GetWheelRotation() > 0 ? -1 : 1) * Config::OBJECTS_LIST_BOX_WHEEL_SCROLL_LINES);
	};
	objectsListBox->Bind(wxEVT_MOUSEWHEEL, onMouseWheel);

	auto midColumnGrid = new wxFlexGridSizer(3, 1, 5, 5);

	// animation block
	auto animationBoxSizer = new wxStaticBoxSizer(wxVERTICAL, mainPanel, "Animation");
	animationBoxExpandSizer = new wxFlexGridSizer(1, 1, 0, 0);
	animationBoxExpandSizer->AddGrowableRow(0, 1);
	animationBoxExpandSizer->AddGrowableCol(0, 1);
	animationPanel = new wxPanel(animationBoxSizer->GetStaticBox(), -1);
	animationPanelSizer = new wxBoxSizer(wxVERTICAL);

	wxImage arrowIconImage("res/icons/green_arrow_left.png", wxBITMAP_TYPE_PNG);
	wxImage arrowIconDiagonalImage("res/icons/green_arrow_top_left.png", wxBITMAP_TYPE_PNG);
	animationMainGridSizer = new wxFlexGridSizer(3, 3, 5, 5);

	auto dirTopLeftButton = new wxButton(animationPanel, ID_DIR_TOP_LEFT_BUTTON, "", wxDefaultPosition, wxSize(32, 32));
	wxBitmap dirTopLeftButtonIcon(arrowIconDiagonalImage);
	dirTopLeftButton->SetBitmap(dirTopLeftButtonIcon, wxLEFT);
	animationMainGridSizer->Add(dirTopLeftButton, 0, wxALIGN_CENTER);

	auto dirTopButton = new wxButton(animationPanel, ID_DIR_TOP_BUTTON, "", wxDefaultPosition, wxSize(32, 32));
	arrowIconImage = arrowIconImage.Rotate90();
	wxBitmap dirTopButtonIcon(arrowIconImage);
	dirTopButton->SetBitmap(dirTopButtonIcon, wxLEFT);
	animationMainGridSizer->Add(dirTopButton, 0, wxALIGN_CENTER);

	auto dirTopRightButton = new wxButton(animationPanel, ID_DIR_TOP_RIGHT_BUTTON, "", wxDefaultPosition, wxSize(32, 32));
	arrowIconDiagonalImage = arrowIconDiagonalImage.Rotate90();
	wxBitmap dirTopRightButtonIcon(arrowIconDiagonalImage);
	dirTopRightButton->SetBitmap(dirTopRightButtonIcon, wxLEFT);
	animationMainGridSizer->Add(dirTopRightButton, 0, wxALIGN_CENTER);

	auto dirLeftButton = new wxButton(animationPanel, ID_DIR_LEFT_BUTTON, "", wxDefaultPosition, wxSize(32, 32));
	arrowIconImage = arrowIconImage.Rotate90(false);
	wxBitmap dirLeftButtonIcon(arrowIconImage);
	dirLeftButton->SetBitmap(dirLeftButtonIcon, wxLEFT);
	animationMainGridSizer->Add(dirLeftButton, 0, wxALIGN_CENTER);

	animationMainGridSizer->Add(32, 32); // placeholder for sprite holders

	auto dirRightButton = new wxButton(animationPanel, ID_DIR_RIGHT_BUTTON, "", wxDefaultPosition, wxSize(32, 32));
	arrowIconImage = arrowIconImage.Rotate180();
	wxBitmap dirRightButtonIcon(arrowIconImage);
	dirRightButton->SetBitmap(dirRightButtonIcon, wxLEFT);
	animationMainGridSizer->Add(dirRightButton, 0, wxALIGN_CENTER);

	auto dirBottomLeftButton = new wxButton(animationPanel, ID_DIR_BOTTOM_LEFT_BUTTON, "", wxDefaultPosition, wxSize(32, 32));
	arrowIconDiagonalImage = arrowIconDiagonalImage.Rotate180();
	wxBitmap dirBottomLeftButtonIcon(arrowIconDiagonalImage);
	dirBottomLeftButton->SetBitmap(dirBottomLeftButtonIcon, wxLEFT);
	animationMainGridSizer->Add(dirBottomLeftButton, 0, wxALIGN_CENTER);

	auto dirBottomButton = new wxButton(animationPanel, ID_DIR_BOTTOM_BUTTON, "", wxDefaultPosition, wxSize(32, 32));
	arrowIconImage = arrowIconImage.Rotate90();
	wxBitmap dirBottomButtonIcon(arrowIconImage);
	dirBottomButton->SetBitmap(dirBottomButtonIcon, wxLEFT);
	animationMainGridSizer->Add(dirBottomButton, 0, wxALIGN_CENTER);

	auto dirBottomRightButton = new wxButton(animationPanel, ID_DIR_BOTTOM_RIGHT_BUTTON, "", wxDefaultPosition, wxSize(32, 32));
	arrowIconDiagonalImage = arrowIconDiagonalImage.Rotate90(false);
	wxBitmap dirBottomRightButtonIcon(arrowIconDiagonalImage);
	dirBottomRightButton->SetBitmap(dirBottomRightButtonIcon, wxLEFT);
	animationMainGridSizer->Add(dirBottomRightButton, 0, wxALIGN_CENTER);

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
	currentFrameNumber = new wxStaticText(animationPanel, -1, STR_ZERO);
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

	// boolean attributes
	auto booleanAttrsBox = new wxStaticBoxSizer(wxVERTICAL, mainPanel, "Boolean attributes");
	booleanAttrsPanel = new wxPanel(mainPanel, -1);
	auto booleanAttrsPanelSizer = new wxGridSizer(4, 0, 0);

	wxCheckBox * curCb = nullptr;
	curCb = booleanAttrCheckboxes[ID_ATTR_IS_CONTAINER] = new wxCheckBox(booleanAttrsPanel, ID_ATTR_IS_CONTAINER, "Is container");
	booleanAttrsPanelSizer->Add(curCb, 0, wxALL, 3);
	curCb = booleanAttrCheckboxes[ID_ATTR_IS_STACKABLE] = new wxCheckBox(booleanAttrsPanel, ID_ATTR_IS_STACKABLE, "Is stackable");
	booleanAttrsPanelSizer->Add(curCb, 0, wxALL, 3);
	curCb = booleanAttrCheckboxes[ID_ATTR_IS_MULTI_USE] = new wxCheckBox(booleanAttrsPanel, ID_ATTR_IS_MULTI_USE, "Multi use");
	booleanAttrsPanelSizer->Add(curCb, 0, wxALL, 3);
	curCb = booleanAttrCheckboxes[ID_ATTR_IS_WALKABLE] = new wxCheckBox(booleanAttrsPanel, ID_ATTR_IS_WALKABLE, "Is walkable");
	booleanAttrsPanelSizer->Add(curCb, 0, wxALL, 3);
	curCb = booleanAttrCheckboxes[ID_ATTR_IS_PATHABLE] = new wxCheckBox(booleanAttrsPanel, ID_ATTR_IS_PATHABLE, "Is pathable");
	booleanAttrsPanelSizer->Add(curCb, 0, wxALL, 3);
	curCb = booleanAttrCheckboxes[ID_ATTR_IS_MOVABLE] = new wxCheckBox(booleanAttrsPanel, ID_ATTR_IS_MOVABLE, "Is movable");
	booleanAttrsPanelSizer->Add(curCb, 0, wxALL, 3);
	curCb = booleanAttrCheckboxes[ID_ATTR_BLOCKS_PROJECTILES] = new wxCheckBox(booleanAttrsPanel, ID_ATTR_BLOCKS_PROJECTILES, "Blocks projectiles");
	booleanAttrsPanelSizer->Add(curCb, 0, wxALL, 3);
	curCb = booleanAttrCheckboxes[ID_ATTR_IS_PICKUPABLE] = new wxCheckBox(booleanAttrsPanel, ID_ATTR_IS_PICKUPABLE, "Is pickupable");
	booleanAttrsPanelSizer->Add(curCb, 0, wxALL, 3);
	curCb = booleanAttrCheckboxes[ID_ATTR_IGNORE_LOOK] = new wxCheckBox(booleanAttrsPanel, ID_ATTR_IGNORE_LOOK, "Ignore look");
	booleanAttrsPanelSizer->Add(curCb, 0, wxALL, 3);
	curCb = booleanAttrCheckboxes[ID_ATTR_IS_HANGABLE] = new wxCheckBox(booleanAttrsPanel, ID_ATTR_IS_HANGABLE, "Is hangable");
	booleanAttrsPanelSizer->Add(curCb, 0, wxALL, 3);
	curCb = booleanAttrCheckboxes[ID_ATTR_IS_LYING_CORPSE] = new wxCheckBox(booleanAttrsPanel, ID_ATTR_IS_LYING_CORPSE, "Is lying corpse");
	booleanAttrsPanelSizer->Add(curCb, 0, wxALL, 3);
	curCb = booleanAttrCheckboxes[ID_ATTR_HAS_MOUNT] = new wxCheckBox(booleanAttrsPanel, ID_ATTR_HAS_MOUNT, "Has mount");
	booleanAttrsPanelSizer->Add(curCb, 0, wxALL, 3);
	for (int i = ID_ATTR_IS_CONTAINER; i < ID_ATTR_BOOLEAN_LAST; ++i)
	{
		booleanAttrCheckboxes[i]->Bind(wxEVT_CHECKBOX, &MainWindow::OnToggleAttrCheckbox, this);
	}

	booleanAttrsPanel->SetSizer(booleanAttrsPanelSizer);
	booleanAttrsBox->Add(booleanAttrsPanel, 1, wxEXPAND);
	midColumnGrid->Add(booleanAttrsBox, 1, wxEXPAND);

	auto valueAttrsBox = new wxStaticBoxSizer(wxVERTICAL, mainPanel, "Value attributes");
	valueAttrsPanel = new wxPanel(mainPanel, -1);
	auto valueAttrsPanelSizer = new wxFlexGridSizer(4, 0, 0);

	curCb = valueAttrCheckboxes[ID_ATTR_IS_FULL_GROUND] = new wxCheckBox(valueAttrsPanel, ID_ATTR_IS_FULL_GROUND, "Is full ground");
	valueAttrsPanelSizer->Add(curCb, 0, wxALL, 3);
	curCb = valueAttrCheckboxes[ID_ATTR_HAS_LIGHT] = new wxCheckBox(valueAttrsPanel, ID_ATTR_HAS_LIGHT, "Has light");
	valueAttrsPanelSizer->Add(curCb, 0, wxALL, 3);
	curCb = valueAttrCheckboxes[ID_ATTR_HAS_OFFSET] = new wxCheckBox(valueAttrsPanel, ID_ATTR_HAS_OFFSET, "Has offset");
	valueAttrsPanelSizer->Add(curCb, 0, wxALL, 3);
	curCb = valueAttrCheckboxes[ID_ATTR_HAS_ELEVATION] = new wxCheckBox(valueAttrsPanel, ID_ATTR_HAS_ELEVATION, "Has elevation");
	valueAttrsPanelSizer->Add(curCb, 0, wxALL, 3);

	groundSpeedLabel = new wxStaticText(valueAttrsPanel, -1, "Ground speed:");
	groundSpeedLabel->Disable();
	valueAttrsPanelSizer->Add(groundSpeedLabel, 0, wxLEFT | wxRIGHT, 3);
	lightColorLabel = new wxStaticText(valueAttrsPanel, -1, "Light color:");
	lightColorLabel->Disable();
	valueAttrsPanelSizer->Add(lightColorLabel, 0, wxLEFT | wxRIGHT, 3);
	offsetXLabel = new wxStaticText(valueAttrsPanel, -1, "Offset X:");
	offsetXLabel->Disable();
	valueAttrsPanelSizer->Add(offsetXLabel, 0, wxLEFT | wxRIGHT, 3);
	elevationLabel = new wxStaticText(valueAttrsPanel, -1, "Elevation:");
	elevationLabel->Disable();
	valueAttrsPanelSizer->Add(elevationLabel, 0, wxLEFT | wxRIGHT, 3);

	wxIntegerValidator <short> groundSpeedValidator;
	groundSpeedInput = new wxTextCtrl(valueAttrsPanel, ID_GROUND_SPEED_INPUT, STR_ZERO, wxDefaultPosition,
	                                  wxDefaultSize, wxTE_RIGHT, groundSpeedValidator);
	groundSpeedInput->Disable();
	valueAttrsPanelSizer->Add(groundSpeedInput, 0, wxALL, 3);
	lightColorPicker = new wxColourPickerCtrl(valueAttrsPanel, ID_LIGHT_COLOR_PICKER, *wxWHITE);
	lightColorPicker->Disable();
	valueAttrsPanelSizer->Add(lightColorPicker, 0, wxALL, 3);
	wxIntegerValidator <short> offsetXValidator;
	offsetXInput = new wxTextCtrl(valueAttrsPanel, ID_OFFSET_X_INPUT, STR_ZERO, wxDefaultPosition,
	                              wxDefaultSize, wxTE_RIGHT, offsetXValidator);
	offsetXInput->Disable();
	valueAttrsPanelSizer->Add(offsetXInput, 0, wxALL, 3);
	wxIntegerValidator <short> elevationValidator;
	elevationInput = new wxTextCtrl(valueAttrsPanel, ID_ELEVATION_INPUT, STR_ZERO, wxDefaultPosition,
	                                wxDefaultSize, wxTE_RIGHT, elevationValidator);
	elevationInput->Disable();
	valueAttrsPanelSizer->Add(elevationInput, 0, wxALL, 3);

	valueAttrsPanelSizer->Add(0, 0); // empty cell
	lightIntensityLabel = new wxStaticText(valueAttrsPanel, -1, "Light intensity:");
	lightIntensityLabel->Disable();
	valueAttrsPanelSizer->Add(lightIntensityLabel, 0, wxLEFT | wxRIGHT, 3);
	offsetYLabel = new wxStaticText(valueAttrsPanel, -1, "Offset Y:");
	offsetYLabel->Disable();
	valueAttrsPanelSizer->Add(offsetYLabel, 0, wxLEFT | wxRIGHT, 3);
	valueAttrsPanelSizer->Add(0, 0); // empty cell

	valueAttrsPanelSizer->Add(0, 0); // empty cell
	wxIntegerValidator <unsigned short> lightIntensityValiator;
	lightIntensityInput = new wxTextCtrl(valueAttrsPanel, ID_LIGHT_INTENSITY_INPUT, STR_ZERO, wxDefaultPosition,
		                                   wxDefaultSize, wxTE_RIGHT, lightIntensityValiator);
	lightIntensityInput->Disable();
	valueAttrsPanelSizer->Add(lightIntensityInput, 0, wxALL, 3);
	wxIntegerValidator <short> offsetYValidator;
	offsetYInput = new wxTextCtrl(valueAttrsPanel, ID_OFFSET_Y_INPUT, STR_ZERO, wxDefaultPosition,
	                              wxDefaultSize, wxTE_RIGHT, offsetYValidator);
	offsetYInput->Disable();
	valueAttrsPanelSizer->Add(offsetYInput, 0, wxALL, 3);

	valueAttrsPanelSizer->AddGrowableCol(0, 0);
	valueAttrsPanelSizer->AddGrowableCol(1, 0);
	valueAttrsPanelSizer->AddGrowableCol(2, 0);
	valueAttrsPanelSizer->AddGrowableCol(3, 0);
	valueAttrsPanel->SetSizer(valueAttrsPanelSizer);
	valueAttrsBox->Add(valueAttrsPanel, 1, wxEXPAND);
	midColumnGrid->Add(valueAttrsBox, 1, wxEXPAND);

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
	booleanAttrsPanel->Disable();
	valueAttrsPanel->Disable();
}

void MainWindow::OnCreateNewFiles(wxCommandEvent & event)
{
	if (!checkDirty()) return;

	currentCategory = CategoryItem;
	categoryComboBox->SetSelection(0);
	objectsListBox->Clear();
	if (animationSpritesPanel)
	{
		animationSpritesPanel->DestroyChildren();
		animationSpritesPanel->Destroy();
		animationSpritesPanel = nullptr;
		animationMainGridSizer->Insert(4, 32, 32);
	}
	objectSpritesPanelSizer->Clear(true);
	newSpritesPanelSizer->Clear(true);

	mainPanel->Disable();
	animationPanel->Disable();
	booleanAttrsPanel->Disable();
	valueAttrsPanel->Disable();

	DatSprReaderWriter::getInstance().initNewData();
	mainPanel->Enable();

	isDirty = false;

	statusBar->SetStatusText("New files initialized");
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
		booleanAttrsPanel->Enable();
		valueAttrsPanel->Enable();
	}
	setAttributeValues();
	fillObjectSprites();
	buildAnimationSpriteHolders();
	fillAnimationSection();

	statusBar->SetStatusText("Files have been loaded successfully");
}

void MainWindow::OnDatSprSaved(wxCommandEvent & event)
{
	isDirty = false;
	statusBar->SetStatusText("Files have been saved successfully");
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
	buildAnimationSpriteHolders();
	fillAnimationSection();
	
	animationPanel->Enable(selectedObject ? true : false);
	booleanAttrsPanel->Enable(selectedObject ? true : false);
	valueAttrsPanel->Enable(selectedObject ? true : false);

	statusBar->SetStatusText(wxString::Format("Current category switched to \"%s\"", CATEGORIES[currentCategory]));
}

bool MainWindow::checkDirty()
{
	if (isDirty)
	{
		wxMessageDialog msg(this, "Files have been modified, do you want to save or discard changes?", "Confirmation",
		                    wxYES_NO | wxCANCEL);
		msg.SetYesNoLabels("Save now...", "Discard");
		int result = msg.ShowModal();
		wxCommandEvent event;
		switch (result)
		{
			case wxID_YES:
				event.SetId(wxID_SAVE);
				OnOpenDatSprDialog(event);
				return false;
			break;
			case wxID_NO:
				return true;
			break;
			case wxID_CANCEL:
				return false;
			break;
		}
	}
	return true;
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
	booleanAttrsPanel->Enable();
	valueAttrsPanel->Enable();

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
	buildAnimationSpriteHolders();
	fillAnimationSection();

	statusBar->SetStatusText(wxString::Format("Selected object with ID <%i>", selectedObject->id));
}

void MainWindow::setAttributeValues(bool isNewObject)
{
	// at first, resetting all attributes to default
	for (int i = ID_ATTR_IS_CONTAINER; i < ID_ATTR_BOOLEAN_LAST; ++i)
	{
		booleanAttrCheckboxes[i]->SetValue(false);
	}
	for (int i = ID_ATTR_IS_FULL_GROUND; i <= ID_ATTR_HAS_ELEVATION; ++i)
	{
		valueAttrCheckboxes[i]->SetValue(false);
	}
	groundSpeedLabel->Disable();
	groundSpeedInput->ChangeValue(STR_ZERO);
	groundSpeedInput->Disable();
	lightColorLabel->Disable();
	lightColorPicker->SetColour(*wxWHITE);
	lightColorPicker->Disable();
	lightIntensityLabel->Disable();
	lightIntensityInput->ChangeValue(STR_ZERO);
	lightIntensityInput->Disable();
	offsetXLabel->Disable();
	offsetXInput->ChangeValue(STR_ZERO);
	offsetXInput->Disable();
	offsetYLabel->Disable();
	offsetYInput->ChangeValue(STR_ZERO);
	offsetYInput->Disable();
	elevationLabel->Disable();
	elevationInput->ChangeValue(STR_ZERO);
	elevationInput->Disable();

	if (isNewObject)
	{
		memset(selectedObject->allAttrs, 0, AttrLast);
		selectedObject->allAttrs[AttrLast] = true;
	}

	if (!selectedObject) return;

	// then setting values
	// boolean attributes first
	if (selectedObject->isContainer)
	{
		booleanAttrCheckboxes[ID_ATTR_IS_CONTAINER]->SetValue(true);
		if (isNewObject)
		{
			selectedObject->allAttrs[AttrContainer] = true;
		}
	}
	if (selectedObject->isStackable)
	{
		booleanAttrCheckboxes[ID_ATTR_IS_STACKABLE]->SetValue(true);
		if (isNewObject)
		{
			selectedObject->allAttrs[AttrStackable] = true;
		}
	}
	if (selectedObject->isMultiUse)
	{
		booleanAttrCheckboxes[ID_ATTR_IS_MULTI_USE]->SetValue(true);
		if (isNewObject)
		{
			selectedObject->allAttrs[AttrMultiUse] = true;
		}
	}
	if (selectedObject->isWalkable)
	{
		booleanAttrCheckboxes[ID_ATTR_IS_WALKABLE]->SetValue(true);
	}
	else if (isNewObject)
	{
		selectedObject->allAttrs[AttrNotWalkable] = true;
	}
	if (selectedObject->isPathable)
	{
		booleanAttrCheckboxes[ID_ATTR_IS_PATHABLE]->SetValue(true);
	}
	else if (isNewObject)
	{
		selectedObject->allAttrs[AttrNotPathable] = true;
	}
	if (selectedObject->isMovable)
	{
		booleanAttrCheckboxes[ID_ATTR_IS_MOVABLE]->SetValue(true);
	}
	else if (isNewObject)
	{
		selectedObject->allAttrs[AttrNotMoveable] = true;
	}
	if (selectedObject->blocksProjectiles)
	{
		booleanAttrCheckboxes[ID_ATTR_BLOCKS_PROJECTILES]->SetValue(true);
		if (isNewObject)
		{
			selectedObject->allAttrs[AttrBlockProjectile] = true;
		}
	}
	if (selectedObject->isPickupable)
	{
		booleanAttrCheckboxes[ID_ATTR_IS_PICKUPABLE]->SetValue(true);
		if (isNewObject)
		{
			selectedObject->allAttrs[AttrPickupable] = true;
		}
	}
	if (selectedObject->ignoreLook)
	{
		booleanAttrCheckboxes[ID_ATTR_IGNORE_LOOK]->SetValue(true);
		if (isNewObject)
		{
			selectedObject->allAttrs[AttrLook] = true;
		}
	}
	if (selectedObject->isHangable)
	{
		booleanAttrCheckboxes[ID_ATTR_IS_HANGABLE]->SetValue(true);
		if (isNewObject)
		{
			selectedObject->allAttrs[AttrHangable] = true;
		}
	}
	if (selectedObject->isLyingCorpse)
	{
		booleanAttrCheckboxes[ID_ATTR_IS_LYING_CORPSE]->SetValue(true);
		if (isNewObject)
		{
			selectedObject->allAttrs[AttrLyingCorpse] = true;
		}
	}
	if (selectedObject->hasMount)
	{
		booleanAttrCheckboxes[ID_ATTR_HAS_MOUNT]->SetValue(true);
		if (isNewObject)
		{
			selectedObject->allAttrs[AttrMount] = true;
		}
	}
	if (selectedObject->isAlwaysAnimated)
	{
		alwaysAnimatedCheckbox->SetValue(true);
		if (isNewObject)
		{
			selectedObject->allAttrs[AttrAnimateAlways] = true;
		}
	}

	// value attributes
	if (selectedObject->isFullGround)
	{
		valueAttrCheckboxes[ID_ATTR_IS_FULL_GROUND]->SetValue(true);
		groundSpeedLabel->Enable();
		groundSpeedInput->ChangeValue(wxString::Format("%i", selectedObject->groundSpeed));
		groundSpeedInput->Enable();
		if (isNewObject)
		{
			selectedObject->allAttrs[AttrFullGround] = true;
		}
	}
	else
	{
		valueAttrCheckboxes[ID_ATTR_IS_FULL_GROUND]->SetValue(false);
		groundSpeedLabel->Disable();
		groundSpeedInput->ChangeValue(STR_ZERO);
		groundSpeedInput->Disable();
	}
	if (selectedObject->isLightSource)
	{
		valueAttrCheckboxes[ID_ATTR_HAS_LIGHT]->SetValue(true);
		lightColorLabel->Enable();
		// TODO: set light color here as well
		lightColorPicker->Enable();
		lightIntensityLabel->Enable();
		lightIntensityInput->ChangeValue(wxString::Format("%i", selectedObject->lightIntensity));
		lightIntensityInput->Enable();
		if (isNewObject)
		{
			selectedObject->allAttrs[AttrLight] = true;
		}
	}
	else
	{
		valueAttrCheckboxes[ID_ATTR_HAS_LIGHT]->SetValue(false);
		lightColorLabel->Disable();
		lightColorPicker->SetColour(*wxWHITE);
		lightColorPicker->Disable();
		lightIntensityLabel->Disable();
		lightIntensityInput->ChangeValue(STR_ZERO);
		lightIntensityInput->Disable();
	}
	if (selectedObject->hasDisplacement)
	{
		valueAttrCheckboxes[ID_ATTR_HAS_OFFSET]->SetValue(true);
		offsetXLabel->Enable();
		offsetXInput->ChangeValue(wxString::Format("%i", selectedObject->displacementX));
		offsetXInput->Enable();
		offsetYLabel->Enable();
		offsetYInput->ChangeValue(wxString::Format("%i", selectedObject->displacementY));
		offsetYInput->Enable();
		if (isNewObject)
		{
			selectedObject->allAttrs[AttrDisplacement] = true;
		}
	}
	else
	{
		valueAttrCheckboxes[ID_ATTR_HAS_OFFSET]->SetValue(false);
		offsetXLabel->Disable();
		offsetXInput->ChangeValue(STR_ZERO);
		offsetXInput->Disable();
		offsetYLabel->Disable();
		offsetYInput->ChangeValue(STR_ZERO);
		offsetYInput->Disable();
	}
	if (selectedObject->isRaised)
	{
		valueAttrCheckboxes[ID_ATTR_HAS_ELEVATION]->SetValue(true);
		elevationLabel->Enable();
		elevationInput->ChangeValue(wxString::Format("%i", selectedObject->elevation));
		elevationInput->Enable();
		if (isNewObject)
		{
			selectedObject->allAttrs[AttrElevation] = true;
		}
	}
	else
	{
		valueAttrCheckboxes[ID_ATTR_HAS_ELEVATION]->SetValue(false);
		elevationLabel->Disable();
		elevationInput->ChangeValue(STR_ZERO);
		elevationInput->Disable();
	}
}

void MainWindow::OnToggleAttrCheckbox(wxCommandEvent & event)
{
	bool value = event.GetInt() != 0;
	switch (event.GetId())
	{
		case ID_ATTR_IS_CONTAINER:
			selectedObject->isContainer = value;
			selectedObject->allAttrs[AttrContainer] = value;
			statusBar->SetStatusText(wxString::Format(OBJ_ATTR_CHANGE_STATUS_MSG, "Is container", value ? "on" : "off"));
		break;
		case ID_ATTR_IS_STACKABLE:
			selectedObject->isStackable = value;
			selectedObject->allAttrs[AttrStackable] = value;
			statusBar->SetStatusText(wxString::Format(OBJ_ATTR_CHANGE_STATUS_MSG, "Is stackable", value ? "on" : "off"));
		break;
		case ID_ATTR_IS_MULTI_USE:
			selectedObject->isMultiUse = value;
			selectedObject->allAttrs[AttrMultiUse] = value;
			statusBar->SetStatusText(wxString::Format(OBJ_ATTR_CHANGE_STATUS_MSG, "Multi-use", value ? "on" : "off"));
		break;
		case ID_ATTR_IS_WALKABLE:
			selectedObject->isWalkable = value;
			selectedObject->allAttrs[AttrNotWalkable] = !value;
			statusBar->SetStatusText(wxString::Format(OBJ_ATTR_CHANGE_STATUS_MSG, "Is walkable", value ? "on" : "off"));
		break;
		case ID_ATTR_IS_PATHABLE:
			selectedObject->isPathable = value;
			selectedObject->allAttrs[AttrNotPathable] = !value;
			statusBar->SetStatusText(wxString::Format(OBJ_ATTR_CHANGE_STATUS_MSG, "Is pathable", value ? "on" : "off"));
		break;
		case ID_ATTR_IS_MOVABLE:
			selectedObject->isMovable = value;
			selectedObject->allAttrs[AttrNotMoveable] = !value;
			statusBar->SetStatusText(wxString::Format(OBJ_ATTR_CHANGE_STATUS_MSG, "Is movable", value ? "on" : "off"));
		break;
		case ID_ATTR_BLOCKS_PROJECTILES:
			selectedObject->blocksProjectiles = value;
			selectedObject->allAttrs[AttrBlockProjectile] = value;
			statusBar->SetStatusText(wxString::Format(OBJ_ATTR_CHANGE_STATUS_MSG, "Block projectiles", value ? "on" : "off"));
		break;
		case ID_ATTR_IS_PICKUPABLE:
			selectedObject->isPickupable = value;
			selectedObject->allAttrs[AttrPickupable] = value;
			statusBar->SetStatusText(wxString::Format(OBJ_ATTR_CHANGE_STATUS_MSG, "Is pickupable", value ? "on" : "off"));
		break;
		case ID_ATTR_IGNORE_LOOK:
			selectedObject->ignoreLook = value;
			selectedObject->allAttrs[AttrLook] = value;
			statusBar->SetStatusText(wxString::Format(OBJ_ATTR_CHANGE_STATUS_MSG, "Ignore look", value ? "on" : "off"));
		break;
		case ID_ATTR_IS_HANGABLE:
			selectedObject->isHangable = value;
			selectedObject->allAttrs[AttrHangable] = value;
			statusBar->SetStatusText(wxString::Format(OBJ_ATTR_CHANGE_STATUS_MSG, "Is hangable", value ? "on" : "off"));
		break;
		case ID_ATTR_IS_LYING_CORPSE:
			selectedObject->isLyingCorpse = value;
			selectedObject->allAttrs[AttrLyingCorpse] = value;
			statusBar->SetStatusText(wxString::Format(OBJ_ATTR_CHANGE_STATUS_MSG, "Is lying corpse", value ? "on" : "off"));
		break;
		case ID_ATTR_HAS_MOUNT:
			selectedObject->hasMount = value;
			selectedObject->allAttrs[AttrMount] = value;
			statusBar->SetStatusText(wxString::Format(OBJ_ATTR_CHANGE_STATUS_MSG, "Has mount", value ? "on" : "off"));
		break;
	}
	isDirty = true;
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
	if (selectedObject)
	{
		animationWidthInput->ChangeValue(wxString::Format("%i", selectedObject->width));
		animationHeightInput->ChangeValue(wxString::Format("%i", selectedObject->height));

		currentFrame = currentXDiv = currentYDiv = 0;
		currentFrameNumber->SetLabelText(wxString::Format("%i", currentFrame));
		amountOfFramesInput->ChangeValue(wxString::Format("%i", selectedObject->phasesCount));

		alwaysAnimatedCheckbox->SetValue(selectedObject->isAlwaysAnimated);

		fillAnimationSprites();
	}
	else
	{
		animationWidthInput->ChangeValue(STR_ZERO);
		animationHeightInput->ChangeValue(STR_ZERO);

		currentFrame = currentXDiv = currentYDiv = 0;
		currentFrameNumber->SetLabelText(STR_ZERO);
		amountOfFramesInput->ChangeValue(STR_ZERO);

		alwaysAnimatedCheckbox->SetValue(false);
	}
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
	int width = selectedObject ? selectedObject->width : 1, height = selectedObject ? selectedObject->height : 1;
	animationSpritesSizer = new wxGridSizer(height, width, 0, 0);

	unsigned int wByH = width * height;
	for (unsigned int i = 0; i < wByH; ++i)
	{
		animationSpriteBitmaps[i] = new wxGenericStaticBitmap(animationSpritesPanel, -1, *stubImage);
		animationSpritesSizer->Add(animationSpriteBitmaps[i]);

		// for sprites drag & drop
		auto dropTarget = new AnimationSpriteDropTarget();
		dropTarget->setMainWindow(this);
		dropTarget->setSpriteHolderIndex(i);
		animationSpriteBitmaps[i]->SetDropTarget(dropTarget);

		auto onEnter = [&](wxMouseEvent & event)
		{
			wxGenericStaticBitmap * sBmp = dynamic_cast <wxGenericStaticBitmap *> (event.GetEventObject());
			drawAnimationSpriteSelection(sBmp);
		};
		auto onLeave = [&](wxMouseEvent & event)
		{
			wxGenericStaticBitmap * sBmp = dynamic_cast <wxGenericStaticBitmap *> (event.GetEventObject());
			clearAnimationSpriteSelection(sBmp);
		};
		auto onRightClick = [&](wxMouseEvent & event)
		{
			wxGenericStaticBitmap * sBmp = dynamic_cast <wxGenericStaticBitmap *> (event.GetEventObject());
			wxBitmap emptyBitmap(*stubImage);
			sBmp->SetBitmap(emptyBitmap);
			drawAnimationSpriteSelection(sBmp);
			AnimationSpriteDropTarget * dropTarget = dynamic_cast <AnimationSpriteDropTarget *> (sBmp->GetDropTarget());
			unsigned int spriteHolderIndex = dropTarget->getSpriteHolderIndex();
			unsigned int frameSize = selectedObject->width * selectedObject->height;
			unsigned int spriteIdIndex = (currentXDiv + currentFrame
			                           * selectedObject->patternWidth) * frameSize + (frameSize - 1 - spriteHolderIndex);
			selectedObject->spriteIDs[spriteIdIndex] = 0;
			fillObjectSprites();
		};
		animationSpriteBitmaps[i]->Bind(wxEVT_ENTER_WINDOW, onEnter);
		animationSpriteBitmaps[i]->Bind(wxEVT_LEAVE_WINDOW, onLeave);
		animationSpriteBitmaps[i]->Bind(wxEVT_RIGHT_UP, onRightClick);
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

	isDirty = true;

	statusBar->SetStatusText(wxString::Format("Object width changed to %i", val));
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

	isDirty = true;

	statusBar->SetStatusText(wxString::Format("Object height changed to %i", val));
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

	isDirty = true;

	statusBar->SetStatusText(wxString::Format("Animation amount of frames changed to %i", val));
}

void MainWindow::OnClickOrientationButton(wxCommandEvent & event)
{
	static const wxString & statusMessage = "Current animation orientation set to \"%s\"";

	unsigned int xDiv = 0;

	auto selectOrientation = [&](OrientationToXDiv orient, const char * orientName, unsigned char patternWidth)
	{
		if (selectedObject->patternWidth < patternWidth)
		{
			wxMessageDialog confirmation(this, wxString::Format(DIRECTION_QUESTION, orientName), DIRECTION_QUESTION_TITLE, wxYES_NO | wxCANCEL);
			if (confirmation.ShowModal() == wxID_YES)
			{
				selectedObject->patternWidth = patternWidth;
				resizeObjectSpriteIDsArray(selectedObject);
				xDiv = orient;
				statusBar->SetStatusText(wxString::Format(statusMessage, orientName));
			}
		}
		else
		{
			xDiv = orient;
			statusBar->SetStatusText(wxString::Format(statusMessage, orientName));
		}
	};

	switch (event.GetId())
	{
		case ID_DIR_TOP_BUTTON:
			xDiv = ORIENT_NORTH;
			statusBar->SetStatusText(wxString::Format(statusMessage, "north"));
		break;
		case ID_DIR_RIGHT_BUTTON:
			selectOrientation(ORIENT_EAST, "east", 2);
		break;
		case ID_DIR_BOTTOM_BUTTON:
			selectOrientation(ORIENT_SOUTH, "south", 3);
		break;
		case ID_DIR_LEFT_BUTTON:
			selectOrientation(ORIENT_WEST, "west", 4);
		break;
		case ID_DIR_TOP_LEFT_BUTTON:
			selectOrientation(ORIENT_NORTH_WEST, "north-west", 5);
		break;
		case ID_DIR_TOP_RIGHT_BUTTON:
			selectOrientation(ORIENT_NORTH_EAST, "north-east", 6);
		break;
		case ID_DIR_BOTTOM_RIGHT_BUTTON:
			selectOrientation(ORIENT_SOUTH_EAST, "south-east", 7);
		break;
		case ID_DIR_BOTTOM_LEFT_BUTTON:
			selectOrientation(ORIENT_SOUTH_WEST, "south-west", 8);
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
	newObject->allAttrs[AttrLast] = true;
	objects->push_back(newObject);
	selectedObject = newObject;
	objectsListBox->Insert(wxString::Format("%i", newObject->id), objectsListBox->GetCount());
	objectsListBox->SetSelection(objectsListBox->GetCount() - 1);

	animationPanel->Enable();
	booleanAttrsPanel->Enable();
	valueAttrsPanel->Enable();

	setAttributeValues();
	fillObjectSprites();
	buildAnimationSpriteHolders();
	fillAnimationSection();

	isDirty = true;

	statusBar->SetStatusText(wxString::Format("New object with ID <%i> has been created in the \"%s\" category",
	                         newObject->id, CATEGORIES[currentCategory]));
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
		wxString basename;
		unsigned int bitmapId = 0;
		wxString errors;
		for (auto const & p : paths)
		{
			filename.Assign(p);
			basename = filename.GetFullName();

			// validating that it's a PNG format
			fstream file;
			file.open(p.mb_str(), ios::in | ios::binary);
			if (file.is_open())
			{
				file.seekg(1);
				char signature[3];
				file.read(signature, 3);
				file.close();
				if (signature[0] != 'P' || signature[1] != 'N' || signature[2] != 'G')
				{
					errors.Append(wxString::Format("%s has not been imported, because it's not in PNG format\n\n", basename));
					continue;
				}
			}
			else
			{
				errors.Append(wxString::Format("%s has not been imported, because file cannot be opened\n\n", basename));
				continue;
			}

			bitmapId = importedSprites.size();
			bitmap = make_shared <wxBitmap> ();
			bitmap->LoadFile(p, wxBITMAP_TYPE_PNG);
			if (!bitmap->IsOk())
			{
				errors.Append(wxString::Format("%s has not been imported, because loading file has failed\n\n", basename));
				continue;
			}
			if (bitmap->GetWidth() % 32 != 0 || bitmap->GetHeight() % 32 != 0)
			{
				errors.Append(wxString::Format("%s has not been imported, because its width and height must be multiplier of 32\n\n", basename));
				continue;
			}
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
			spriteLabel = new wxStaticText(newSpritesPanel, -1, basename);
			newSpritesPanelSizer->Add(spriteLabel, 0, wxALL | wxALIGN_CENTER_VERTICAL, 5);
		}
		newSpritesPanelSizer->Layout();
		newSpritesPanel->FitInside();

		if (errors.Length() > 0)
		{
			wxMessageDialog errorMsg(this, errors, "Error", wxOK | wxICON_ERROR);
			errorMsg.SetMinSize(wxSize(300, -1));
			errorMsg.ShowModal();
			statusBar->SetStatusText("Errors occured while importing new files");
		}
		else if (paths.size() > 0)
		{
			if (paths.size() == 1)
			{
				filename.Assign(paths[0]);
				statusBar->SetStatusText(wxString::Format("New sprite, named \"%s\" has been imported", filename.GetName()));
			}
			else
			{
				statusBar->SetStatusText(wxString::Format("%i new sprites have been imported", paths.size()));
			}
		}
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

void MainWindow::OnToggleAlwaysAnimatedAttr(wxCommandEvent & event)
{
	bool value = event.GetInt() != 0;
	selectedObject->isAlwaysAnimated = value;
	selectedObject->allAttrs[AttrAnimateAlways] = value;

	isDirty = true;

	statusBar->SetStatusText(wxString::Format(OBJ_ATTR_CHANGE_STATUS_MSG, "Always animated", value ? "on" : "off"));
}

void MainWindow::OnToggleIsFullGroundAttr(wxCommandEvent & event)
{
	bool value = event.GetInt() != 0;
	groundSpeedLabel->Enable(value);
	groundSpeedInput->Enable(value);
	selectedObject->isGround = selectedObject->isFullGround = value;
	selectedObject->allAttrs[AttrGround] = selectedObject->allAttrs[AttrFullGround] = value;
	selectedObject->groundSpeed = wxAtoi(groundSpeedInput->GetValue());

	isDirty = true;

	statusBar->SetStatusText(wxString::Format(OBJ_ATTR_CHANGE_STATUS_MSG, "Is full ground", value ? "on" : "off"));
}

void MainWindow::OnToggleHasLightAttr(wxCommandEvent & event)
{
	bool value = event.GetInt() != 0;
	lightColorLabel->Enable(value);
	lightColorPicker->Enable(value);
	lightIntensityLabel->Enable(value);
	lightIntensityInput->Enable(value);
	selectedObject->isLightSource = value;
	selectedObject->allAttrs[AttrLight] = value;
	// TODO: handle light color here
	selectedObject->lightIntensity = wxAtoi(lightIntensityInput->GetValue());

	isDirty = true;

	statusBar->SetStatusText(wxString::Format(OBJ_ATTR_CHANGE_STATUS_MSG, "Has light", value ? "on" : "off"));
}

void MainWindow::OnToggleHasOffsetAttr(wxCommandEvent & event)
{
	bool value = event.GetInt() != 0;
	offsetXLabel->Enable(value);
	offsetXInput->Enable(value);
	offsetYLabel->Enable(value);
	offsetYInput->Enable(value);
	selectedObject->hasDisplacement = value;
	selectedObject->allAttrs[AttrDisplacement] = value;
	selectedObject->displacementX = wxAtoi(offsetXInput->GetValue());
	selectedObject->displacementY = wxAtoi(offsetYInput->GetValue());

	isDirty = true;

	statusBar->SetStatusText(wxString::Format(OBJ_ATTR_CHANGE_STATUS_MSG, "Has offset", value ? "on" : "off"));
}

void MainWindow::OnToggleHasElevationAttr(wxCommandEvent & event)
{
	bool value = event.GetInt() != 0;
	elevationLabel->Enable(value);
	elevationInput->Enable(value);
	selectedObject->isRaised = value;
	selectedObject->allAttrs[AttrElevation] = value;
	selectedObject->elevation = wxAtoi(elevationInput->GetValue());

	isDirty = true;

	statusBar->SetStatusText(wxString::Format(OBJ_ATTR_CHANGE_STATUS_MSG, "Has elevation", value ? "on" : "off"));
}

void MainWindow::OnGroundSpeedChanged(wxCommandEvent & event)
{
	if (selectedObject)
	{
		selectedObject->groundSpeed = wxAtoi(groundSpeedInput->GetValue());
		statusBar->SetStatusText(wxString::Format("Object \"Ground speed\" value changed to %i", selectedObject->groundSpeed));
		isDirty = true;
	}
}

void MainWindow::OnLightColorChanged(wxColourPickerEvent & event)
{
	if (selectedObject)
	{
		// TODO:
		// event.GetColour();
		isDirty = true;
	}
}

void MainWindow::OnLightIntensityChanged(wxCommandEvent & event)
{
	if (selectedObject)
	{
		selectedObject->lightIntensity = wxAtoi(lightIntensityInput->GetValue());
		statusBar->SetStatusText(wxString::Format("Object \"Light intensity\" value changed to %i", selectedObject->lightIntensity));
		isDirty = true;
	}
}

void MainWindow::OnOffsetXYChanged(wxCommandEvent & event)
{
	if (selectedObject)
	{
		int id = event.GetId();
		if (id == ID_OFFSET_X_INPUT)
		{
			selectedObject->displacementX = wxAtoi(offsetXInput->GetValue());
			statusBar->SetStatusText(wxString::Format("Object \"Offset X\" value changed to %i", selectedObject->displacementX));
		}
		else if (id == ID_OFFSET_Y_INPUT)
		{
			selectedObject->displacementY = wxAtoi(offsetYInput->GetValue());
			statusBar->SetStatusText(wxString::Format("Object \"Offset Y\" value changed to %i", selectedObject->displacementY));
		}
		isDirty = true;
	}
}

void MainWindow::OnElevationChanged(wxCommandEvent & event)
{
	if (selectedObject)
	{
		selectedObject->elevation = wxAtoi(elevationInput->GetValue());
		statusBar->SetStatusText(wxString::Format("Object \"Elevation\" value changed to %i", selectedObject->elevation));
		isDirty = true;
	}
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
	Close();
}

void MainWindow::OnClose(wxCloseEvent & event)
{
	if (event.CanVeto() && !checkDirty())
	{
		event.Veto();
	}
	else
	{
		Destroy();
	}
}

void MainWindow::OnGenerateRMEDialog(wxCommandEvent & event)
{
	auto generateRMEDialog = new GenerateRMEDialog(this);
	generateRMEDialog->ShowModal();
}

void MainWindow::OnQuickGuide(wxCommandEvent & event)
{
	auto quickGuideDialog = new QuickGuideDialog(this);
	quickGuideDialog->ShowModal();
}

void MainWindow::OnAbout(wxCommandEvent & event)
{
	auto aboutDialog = new AboutDialog(this);
	aboutDialog->ShowModal();
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

		result = mainWindow->isDirty = true;
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

		result = mainWindow->isDirty = true;
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
