#include <math.h>
#include <fstream>
#include <algorithm>
#include <wx/wxprec.h>
#ifndef WX_PRECOMP
#include <wx/wx.h>
#endif
#include <wx/filename.h>
#include <wx/notebook.h>
#include <wx/rawbmp.h>
#include <wx/valnum.h>
#include <wx/dcgraph.h>
#include "Events.h"
#include "Utils.h"
#include "Settings.h"
#include "MainWindow.h"
#include "DatSprOpenSaveDialog.h"
#include "AdvancedAttributesDialog.h"
#include "AdvancedAttributesManager.h"
#include "GenerateRMEDialog.h"
#include "SpritesCleanupDialog.h"
#include "PreferencesDialog.h"
#include "AutoBackupDialog.h"
#include "QuickGuideDialog.h"
#include "AboutDialog.h"
#include "DatSprReaderWriter.h"

const wxString & MainWindow::DIRECTION_QUESTION = "This object doesn't have \"%s\" direction yet, create it?";
const wxString & MainWindow::DIRECTION_QUESTION_TITLE = "Create direction?";
const wxString MainWindow::CATEGORIES[] = { "Items", "Creatures", "Effects", "Projectiles" };
const wxString & STR_ZERO = "0";
const wxString & OBJ_ATTR_CHANGE_STATUS_MSG = "Object \"%s\" attribute set to %s";
const wxString & UNDONE_MSG = "Undone %s";
const wxString & REDONE_MSG = "Redone %s";

wxBEGIN_EVENT_TABLE(MainWindow, wxFrame)
	EVT_CLOSE(MainWindow::OnClose)
	EVT_MENU(wxID_NEW, MainWindow::OnCreateNewFiles)
	EVT_MENU(wxID_OPEN, MainWindow::OnOpenDatSprDialog)
	EVT_MENU(wxID_SAVE, MainWindow::OnOpenDatSprDialog)
	EVT_MENU(wxID_EXIT, MainWindow::OnExit)
	EVT_MENU(wxID_UNDO, MainWindow::OnUndo)
	EVT_MENU(wxID_REDO, MainWindow::OnRedo)
	EVT_MENU(wxID_PREFERENCES, MainWindow::OnPreferencesDialog)
	EVT_MENU(ID_MENU_EDIT_ADVANCED_ATTRS, MainWindow::OnAdvancedAttributesDialog)
	EVT_MENU(ID_MENU_GENERATE_RME, MainWindow::OnGenerateRMEDialog)
	EVT_MENU(ID_MENU_CLEANUP_SPRITES, MainWindow::OnSpritesCleanupDialog)
	EVT_MENU(ID_MENU_QUICK_GUIDE, MainWindow::OnQuickGuide)
	EVT_MENU(wxID_ABOUT, MainWindow::OnAbout)
	EVT_MENU(ID_MENU_EXPORT_SPRITE, MainWindow::OnExportSpriteMenu)
	EVT_MENU(ID_MENU_EXPORT_COMPOSED, MainWindow::OnExportComposedFrameMenu)
	EVT_MENU(ID_MENU_DELETE_SPRITE, MainWindow::OnDeleteSpriteMenu)
	EVT_MENU(ID_MENU_TOGGLE_SPRITE_BLOCKING, MainWindow::OnToggleSpriteBlockingMenu)
	EVT_MENU(ID_MENU_TOGGLE_SPRITE_BLOCKING_ALL_FRAMES, MainWindow::OnToggleSpriteBlockingAllFramesMenu)
	EVT_COMMAND(wxID_ANY, DAT_SPR_LOADED, MainWindow::OnDatSprLoaded)
	EVT_COMMAND(wxID_ANY, DAT_SPR_SAVED, MainWindow::OnDatSprSaved)
	EVT_COMMAND(wxID_ANY, RME_RES_GENERATED, MainWindow::OnRMEResourcesGenerated)
	EVT_COMMAND(wxID_ANY, ADV_ATTRS_CHANGED, MainWindow::OnAdvancedAttributesChanged)
	EVT_COMMAND(wxID_ANY, SPRITES_CLEANED_UP, MainWindow::OnSpritesCleanedUp)
	EVT_COMMAND(wxID_ANY, PREFERENCES_SAVED, MainWindow::OnPreferencesSaved)
	EVT_COMMAND(wxID_ANY, AUTOBACKUP_PROCESSED, MainWindow::OnAutobackupProcessed)
	EVT_COMBOBOX(ID_CATEGORIES_COMBOBOX, MainWindow::OnObjectCategoryChanged)
	EVT_LISTBOX(ID_OBJECTS_LISTBOX, MainWindow::OnObjectSelected)
	EVT_SPINCTRL(ID_ANIM_WIDTH_INPUT, MainWindow::OnAnimWidthChanged)
	EVT_SPINCTRL(ID_ANIM_HEIGHT_INPUT, MainWindow::OnAnimHeightChanged)
	EVT_SPINCTRL(ID_LAYERS_COUNT_INPUT, MainWindow::OnLayersCountChanged)
	EVT_SPINCTRL(ID_FRAMES_AMOUNT_INPUT, MainWindow::OnFramesAmountChanged)
	EVT_BUTTON(ID_DIR_TOP_LEFT_BUTTON, MainWindow::OnClickOrientationButton)
	EVT_BUTTON(ID_DIR_TOP_BUTTON, MainWindow::OnClickOrientationButton)
	EVT_BUTTON(ID_DIR_TOP_RIGHT_BUTTON, MainWindow::OnClickOrientationButton)
	EVT_BUTTON(ID_DIR_LEFT_BUTTON, MainWindow::OnClickOrientationButton)
	EVT_BUTTON(ID_DIR_RIGHT_BUTTON, MainWindow::OnClickOrientationButton)
	EVT_BUTTON(ID_DIR_BOTTOM_LEFT_BUTTON, MainWindow::OnClickOrientationButton)
	EVT_BUTTON(ID_DIR_BOTTOM_BUTTON, MainWindow::OnClickOrientationButton)
	EVT_BUTTON(ID_DIR_BOTTOM_RIGHT_BUTTON, MainWindow::OnClickOrientationButton)
	EVT_SPINCTRL(ID_PATTERN_WIDTH_INPUT, MainWindow::OnPatternWidthChanged)
	EVT_SPINCTRL(ID_PATTERN_HEIGHT_INPUT, MainWindow::OnPatternHeightChanged)
	EVT_BUTTON(ID_PREV_XDIV_BUTTON, MainWindow::OnClickPrevXDivButton)
	EVT_BUTTON(ID_NEXT_XDIV_BUTTON, MainWindow::OnClickNextXDivButton)
	EVT_BUTTON(ID_PREV_YDIV_BUTTON, MainWindow::OnClickPrevYDivButton)
	EVT_BUTTON(ID_NEXT_YDIV_BUTTON, MainWindow::OnClickNextYDivButton)
	EVT_BUTTON(ID_PREV_LAYER_BUTTON, MainWindow::OnClickPrevLayerButton)
	EVT_BUTTON(ID_NEXT_LAYER_BUTTON, MainWindow::OnClickNextLayerButton)
	EVT_BUTTON(ID_PREV_FRAME_BUTTON, MainWindow::OnClickPrevFrameButton)
	EVT_BUTTON(ID_NEXT_FRAME_BUTTON, MainWindow::OnClickNextFrameButton)
	EVT_SPINCTRL(ID_PREVIEW_FPS_INPUT, MainWindow::OnPreviewFPSChanged)
	EVT_BUTTON(ID_PREVIEW_ANIMATION_BUTTON, MainWindow::OnClickPreviewAnimationButton)
	EVT_TIMER(ID_PREVIEW_TIMER, MainWindow::OnPreviewTimerEvent)
	EVT_TIMER(ID_AUTO_BACKUP_TIMER, MainWindow::OnAutoBackupTimerEvent)
	EVT_BUTTON(ID_NEW_OBJECT_BUTTON, MainWindow::OnClickNewObjectButton)
	EVT_BUTTON(ID_DELETE_OBJECT_BUTTON, MainWindow::OnClickDeleteObjectButton)
	EVT_BUTTON(ID_IMPORT_SPRITES_BUTTON, MainWindow::OnClickImportSpriteButton)
	EVT_CHECKBOX(ID_ALWAYS_ANIMATED_CHECKBOX, MainWindow::OnToggleAlwaysAnimatedAttr)
	EVT_CHECKBOX(ID_BLEND_LAYERS_CHECKBOX, MainWindow::OnToggleBlendLayersCheckbox)
	EVT_CHECKBOX(ID_DRAW_BLOCKING_MARKS_CHECKBOX, MainWindow::OnToggleDrawBlockingMarksCheckbox)
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
	menuFile = new wxMenu();
	menuFile->Append(wxID_NEW);
	menuFile->Append(wxID_OPEN);
	menuFile->Append(wxID_SAVE);
	menuFile->FindChildItem(wxID_SAVE)->Enable(false);
	menuFile->AppendSeparator();
	menuFile->Append(wxID_EXIT);
	menuEdit = new wxMenu();
	menuEdit->Append(wxID_UNDO);
	menuEdit->Append(wxID_REDO, "Redo\tCtrl+Y");
	menuEdit->FindChildItem(wxID_UNDO)->Enable(false);
	menuEdit->FindChildItem(wxID_REDO)->Enable(false);
	menuEdit->AppendSeparator();
	menuEdit->Append(wxID_PREFERENCES);
	menuTools = new wxMenu();
	menuTools->Append(ID_MENU_EDIT_ADVANCED_ATTRS, "Edit advanced attributes...\tCtrl+E");
	menuTools->Append(ID_MENU_GENERATE_RME, "Generate RME resources...\tCtrl+G");
	menuTools->Append(ID_MENU_CLEANUP_SPRITES, "Clean-up unused sprites...\tCtrl+U");
	menuTools->FindChildItem(ID_MENU_EDIT_ADVANCED_ATTRS)->Enable(false);
	menuTools->FindChildItem(ID_MENU_GENERATE_RME)->Enable(false);
	menuTools->FindChildItem(ID_MENU_CLEANUP_SPRITES)->Enable(false);
	menuHelp = new wxMenu();
	menuHelp->Append(ID_MENU_QUICK_GUIDE, "Quick guide");
	menuHelp->Append(wxID_ABOUT);
	auto menuBar = new wxMenuBar();
	menuBar->Append(menuFile, "&File");
	menuBar->Append(menuEdit, "&Edit");
	menuBar->Append(menuTools, "&Tools");
	menuBar->Append(menuHelp, "&Help");
	SetMenuBar(menuBar);

	// constructing main window interface
	mainPanel = new wxPanel(this, -1);
	auto vbox = new wxBoxSizer(wxVERTICAL);
	auto fgs = new wxFlexGridSizer(1, 3, 10, 10);

	auto leftColumnGrid = new wxFlexGridSizer(4, 1, 5, 5);
	categoryComboBox = new wxComboBox(mainPanel, ID_CATEGORIES_COMBOBOX, CATEGORIES[0],
			wxDefaultPosition, wxDefaultSize, 4, CATEGORIES, wxCB_READONLY);
	leftColumnGrid->Add(categoryComboBox, 1, wxEXPAND);
	objectsListBox = new wxListBox(mainPanel, ID_OBJECTS_LISTBOX, wxDefaultPosition, wxSize(220, -1), 0, NULL, wxLB_SINGLE);
	auto onKeyDown = [&](wxKeyEvent & event)
	{
		if (event.GetKeyCode() == WXK_DELETE) deleteSelectedObject();
		else event.Skip();
	};
	objectsListBox->Bind(wxEVT_KEY_DOWN, onKeyDown);
	leftColumnGrid->Add(objectsListBox, 1, wxEXPAND);
	leftColumnGrid->AddGrowableRow(1, 1);
	auto newObjectButton = new wxButton(mainPanel, ID_NEW_OBJECT_BUTTON, "New object");
	leftColumnGrid->Add(newObjectButton, 1, wxEXPAND);
	auto deleteObjectButton = new wxButton(mainPanel, ID_DELETE_OBJECT_BUTTON, "Delete object");
	leftColumnGrid->Add(deleteObjectButton, 1, wxEXPAND);

	auto onMouseWheel = [&](wxMouseEvent & event)
	{
		objectsListBox->ScrollLines((event.GetWheelRotation() > 0 ? -1 : 1) * Config::OBJECTS_LIST_BOX_WHEEL_SCROLL_LINES);
	};
	objectsListBox->Bind(wxEVT_MOUSEWHEEL, onMouseWheel);

	auto midColumnGrid = new wxFlexGridSizer(3, 1, 5, 5);

	// animation block
	auto animationBox = new wxStaticBox(mainPanel, wxID_ANY, "Animation");
	animationBoxSizer = new wxFlexGridSizer(1, 1, 0, 0);
	animationBox->SetSizer(animationBoxSizer);
	animationBoxSizer->AddGrowableRow(0, 1);
	animationBoxSizer->AddGrowableCol(0, 1);
	animationPanel = new wxScrolledWindow(animationBox, wxID_ANY);
	animationPanel->SetScrollRate(0, 20);
	animationPanelSizer = new wxFlexGridSizer(1, 1, 0, 0);
	animationPanel->SetSizer(animationPanelSizer);
	animationPanelSizer->AddGrowableRow(0, 1);
	animationPanelSizer->AddGrowableCol(0, 1);
	auto animationPanelVAlignSizer = new wxBoxSizer(wxVERTICAL);

	wxImage arrowIconImage("res/icons/green_arrow_left.png", wxBITMAP_TYPE_PNG);
	wxImage arrowIconDiagonalImage("res/icons/green_arrow_top_left.png", wxBITMAP_TYPE_PNG);
	animationMainGridSizer = new wxFlexGridSizer(3, 3, 5, 5);

	auto dirTopLeftButton = new wxButton(animationPanel, ID_DIR_TOP_LEFT_BUTTON, "", wxDefaultPosition, wxSize(32, 32));
	wxBitmap dirTopLeftButtonIcon(arrowIconDiagonalImage);
	dirTopLeftButton->SetBitmap(dirTopLeftButtonIcon, wxLEFT);
	animationMainGridSizer->Add(dirTopLeftButton, 0, wxALIGN_CENTER);
	controlsToDisableOnPreview.push_back(dirTopLeftButton);

	auto dirTopButton = new wxButton(animationPanel, ID_DIR_TOP_BUTTON, "", wxDefaultPosition, wxSize(32, 32));
	arrowIconImage = arrowIconImage.Rotate90();
	wxBitmap dirTopButtonIcon(arrowIconImage);
	dirTopButton->SetBitmap(dirTopButtonIcon, wxLEFT);
	animationMainGridSizer->Add(dirTopButton, 0, wxALIGN_CENTER);
	controlsToDisableOnPreview.push_back(dirTopButton);

	auto dirTopRightButton = new wxButton(animationPanel, ID_DIR_TOP_RIGHT_BUTTON, "", wxDefaultPosition, wxSize(32, 32));
	arrowIconDiagonalImage = arrowIconDiagonalImage.Rotate90();
	wxBitmap dirTopRightButtonIcon(arrowIconDiagonalImage);
	dirTopRightButton->SetBitmap(dirTopRightButtonIcon, wxLEFT);
	animationMainGridSizer->Add(dirTopRightButton, 0, wxALIGN_CENTER);
	controlsToDisableOnPreview.push_back(dirTopRightButton);

	auto dirLeftButton = new wxButton(animationPanel, ID_DIR_LEFT_BUTTON, "", wxDefaultPosition, wxSize(32, 32));
	arrowIconImage = arrowIconImage.Rotate90(false);
	wxBitmap dirLeftButtonIcon(arrowIconImage);
	dirLeftButton->SetBitmap(dirLeftButtonIcon, wxLEFT);
	animationMainGridSizer->Add(dirLeftButton, 0, wxALIGN_CENTER);
	controlsToDisableOnPreview.push_back(dirLeftButton);

	animationMainGridSizer->Add(32, 32); // placeholder for sprite holders

	auto dirRightButton = new wxButton(animationPanel, ID_DIR_RIGHT_BUTTON, "", wxDefaultPosition, wxSize(32, 32));
	arrowIconImage = arrowIconImage.Rotate180();
	wxBitmap dirRightButtonIcon(arrowIconImage);
	dirRightButton->SetBitmap(dirRightButtonIcon, wxLEFT);
	animationMainGridSizer->Add(dirRightButton, 0, wxALIGN_CENTER);
	controlsToDisableOnPreview.push_back(dirRightButton);

	auto dirBottomLeftButton = new wxButton(animationPanel, ID_DIR_BOTTOM_LEFT_BUTTON, "", wxDefaultPosition, wxSize(32, 32));
	arrowIconDiagonalImage = arrowIconDiagonalImage.Rotate180();
	wxBitmap dirBottomLeftButtonIcon(arrowIconDiagonalImage);
	dirBottomLeftButton->SetBitmap(dirBottomLeftButtonIcon, wxLEFT);
	animationMainGridSizer->Add(dirBottomLeftButton, 0, wxALIGN_CENTER);
	controlsToDisableOnPreview.push_back(dirBottomLeftButton);

	auto dirBottomButton = new wxButton(animationPanel, ID_DIR_BOTTOM_BUTTON, "", wxDefaultPosition, wxSize(32, 32));
	arrowIconImage = arrowIconImage.Rotate90();
	wxBitmap dirBottomButtonIcon(arrowIconImage);
	dirBottomButton->SetBitmap(dirBottomButtonIcon, wxLEFT);
	animationMainGridSizer->Add(dirBottomButton, 0, wxALIGN_CENTER);
	controlsToDisableOnPreview.push_back(dirBottomButton);

	auto dirBottomRightButton = new wxButton(animationPanel, ID_DIR_BOTTOM_RIGHT_BUTTON, "", wxDefaultPosition, wxSize(32, 32));
	arrowIconDiagonalImage = arrowIconDiagonalImage.Rotate90(false);
	wxBitmap dirBottomRightButtonIcon(arrowIconDiagonalImage);
	dirBottomRightButton->SetBitmap(dirBottomRightButtonIcon, wxLEFT);
	animationMainGridSizer->Add(dirBottomRightButton, 0, wxALIGN_CENTER);
	controlsToDisableOnPreview.push_back(dirBottomRightButton);

	animationPanelVAlignSizer->Add(animationMainGridSizer, 0, wxALIGN_CENTER);

	animationSpritesPanel = nullptr; // initializing
	animationSpritesSizer = nullptr; // to prevent warnings
	unsigned char emptyBitmapRGB[Sprite::RGB_SIZE];
	unsigned char emptyBitmapAlpha[Sprite::ALPHA_SIZE];
	memset(emptyBitmapRGB, 0, Sprite::RGB_SIZE);
	memset(emptyBitmapAlpha, 0, Sprite::ALPHA_SIZE);
	wxImage emptyImage(32, 32, emptyBitmapRGB, emptyBitmapAlpha, true);
	emptyBitmap = wxBitmap(emptyImage);

	// animation settings
	auto animationSettingsGridSizer = new wxGridSizer(2, 5, 5);

	// width and height settings
	animationPanelVAlignSizer->Add(0, 5); // a little spacer
	auto widthSizer = new wxBoxSizer(wxHORIZONTAL);
	auto widthLabel = new wxStaticText(animationPanel, -1, "Width:");
	widthSizer->Add(widthLabel, 0, wxALIGN_CENTER_VERTICAL);
	controlsToDisableOnPreview.push_back(widthLabel);
	animationWidthInput = new wxSpinCtrl(animationPanel, ID_ANIM_WIDTH_INPUT, "1", wxDefaultPosition,
	                                     wxSize(Config::COMMON_NUM_FIELD_WIDTH, -1), wxTE_RIGHT);
	animationWidthInput->SetRange(1, Config::MAX_OBJECT_WIDTH);
	widthSizer->Add(animationWidthInput, 0, wxLEFT | wxRIGHT, 5);
	controlsToDisableOnPreview.push_back(animationWidthInput);
	animationSettingsGridSizer->Add(widthSizer, 0, wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL);

	auto heightSizer = new wxBoxSizer(wxHORIZONTAL);
	auto heightLabel = new wxStaticText(animationPanel, -1, "Height:");
	controlsToDisableOnPreview.push_back(heightLabel);
	heightSizer->Add(heightLabel, 0, wxLEFT | wxALIGN_CENTER_VERTICAL, 5);
	animationHeightInput = new wxSpinCtrl(animationPanel, ID_ANIM_HEIGHT_INPUT, "1", wxDefaultPosition,
	                                      wxSize(Config::COMMON_NUM_FIELD_WIDTH, -1), wxTE_RIGHT);
	animationHeightInput->SetRange(1, Config::MAX_OBJECT_HEIGHT);
	heightSizer->Add(animationHeightInput, 0, wxLEFT, 5);
	controlsToDisableOnPreview.push_back(animationHeightInput);
	animationSettingsGridSizer->Add(heightSizer, 0, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL);

	// pattern width / height and xDiv / yDiv controls
	auto patternWidthBox = new wxBoxSizer(wxHORIZONTAL);
	auto patternWidthLabel = new wxStaticText(animationPanel, wxID_ANY, "Pattern width:");
	patternWidthBox->Add(patternWidthLabel, 0, wxALIGN_CENTER_VERTICAL, 5);
	controlsToDisableOnPreview.push_back(patternWidthLabel);
	patternWidthInput = new wxSpinCtrl(animationPanel, ID_PATTERN_WIDTH_INPUT, "1", wxDefaultPosition,
                                     wxSize(Config::COMMON_NUM_FIELD_WIDTH, -1), wxTE_RIGHT);
	patternWidthInput->SetRange(1, Config::MAX_XY_DIV);
	patternWidthBox->Add(patternWidthInput, 0, wxLEFT | wxRIGHT, 5);
	controlsToDisableOnPreview.push_back(patternWidthInput);
	animationSettingsGridSizer->Add(patternWidthBox, 0, wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL);

	wxFont monospaceFont = wxSystemSettings::GetFont(wxSYS_SYSTEM_FONT);
	monospaceFont.SetFamily(wxFONTFAMILY_TELETYPE);

	auto xDivBox = new wxBoxSizer(wxHORIZONTAL);
	auto prevXDivButton = new wxButton(animationPanel, ID_PREV_XDIV_BUTTON, "<", wxDefaultPosition,
	                                   wxSize(Config::SMALL_SQUARE_BUTTON_SIZE, Config::SMALL_SQUARE_BUTTON_SIZE));
	xDivBox->Add(prevXDivButton, 0, wxLEFT, 5);
	controlsToDisableOnPreview.push_back(prevXDivButton);
	auto xDivLabel = new wxStaticText(animationPanel, wxID_ANY, "xDiv:");
	xDivLabel->SetFont(monospaceFont);
	xDivBox->Add(xDivLabel, 0, wxLEFT | wxALIGN_CENTER_VERTICAL, 5);
	controlsToDisableOnPreview.push_back(xDivLabel);
	currentXDivLabel = new wxStaticText(animationPanel, wxID_ANY, STR_ZERO);
	currentXDivLabel->SetFont(monospaceFont);
	xDivBox->Add(currentXDivLabel, 0, wxLEFT | wxRIGHT | wxALIGN_CENTER_VERTICAL, 5);
	controlsToDisableOnPreview.push_back(currentXDivLabel);
	auto nextXDivButton = new wxButton(animationPanel, ID_NEXT_XDIV_BUTTON, ">", wxDefaultPosition,
	                                   wxSize(Config::SMALL_SQUARE_BUTTON_SIZE, Config::SMALL_SQUARE_BUTTON_SIZE));
	xDivBox->Add(nextXDivButton);
	controlsToDisableOnPreview.push_back(nextXDivButton);
	animationSettingsGridSizer->Add(xDivBox, 0, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL);

	auto patternHeightBox = new wxBoxSizer(wxHORIZONTAL);
	auto patternHeightLabel = new wxStaticText(animationPanel, wxID_ANY, "Pattern height:");
	patternHeightBox->Add(patternHeightLabel, 0, wxALIGN_CENTER_VERTICAL);
	controlsToDisableOnPreview.push_back(patternHeightLabel);
	patternHeightInput = new wxSpinCtrl(animationPanel, ID_PATTERN_HEIGHT_INPUT, "1", wxDefaultPosition,
																		 wxSize(Config::COMMON_NUM_FIELD_WIDTH, -1), wxTE_RIGHT);
	patternHeightInput->SetRange(1, Config::MAX_XY_DIV);
	patternHeightBox->Add(patternHeightInput, 0, wxLEFT | wxRIGHT, 5);
	controlsToDisableOnPreview.push_back(patternHeightInput);
	animationSettingsGridSizer->Add(patternHeightBox, 0, wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL);

	auto yDivBox = new wxBoxSizer(wxHORIZONTAL);
	auto prevYDivButton = new wxButton(animationPanel, ID_PREV_YDIV_BUTTON, "<", wxDefaultPosition,
																		 wxSize(Config::SMALL_SQUARE_BUTTON_SIZE, Config::SMALL_SQUARE_BUTTON_SIZE));
	yDivBox->Add(prevYDivButton, 0, wxLEFT, 5);
	controlsToDisableOnPreview.push_back(prevYDivButton);
	auto yDivLabel = new wxStaticText(animationPanel, wxID_ANY, "yDiv:");
	yDivLabel->SetFont(monospaceFont);
	yDivBox->Add(yDivLabel, 0, wxLEFT | wxALIGN_CENTER_VERTICAL, 5);
	controlsToDisableOnPreview.push_back(yDivLabel);
	currentYDivLabel = new wxStaticText(animationPanel, wxID_ANY, STR_ZERO);
	currentYDivLabel->SetFont(monospaceFont);
	yDivBox->Add(currentYDivLabel, 0, wxLEFT | wxRIGHT | wxALIGN_CENTER_VERTICAL, 5);
	controlsToDisableOnPreview.push_back(currentYDivLabel);
	auto nextYDivButton = new wxButton(animationPanel, ID_NEXT_YDIV_BUTTON, ">", wxDefaultPosition,
																		 wxSize(Config::SMALL_SQUARE_BUTTON_SIZE, Config::SMALL_SQUARE_BUTTON_SIZE));
	yDivBox->Add(nextYDivButton);
	controlsToDisableOnPreview.push_back(nextYDivButton);
	animationSettingsGridSizer->Add(yDivBox, 0, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL);

	// layer controls
	auto layersCountSizer = new wxBoxSizer(wxHORIZONTAL);
	auto layersCountLabel = new wxStaticText(animationPanel, -1, "Layers count:");
	layersCountSizer->Add(layersCountLabel, 0, wxALIGN_CENTER_VERTICAL);
	controlsToDisableOnPreview.push_back(layersCountLabel);
	layersCountInput = new wxSpinCtrl(animationPanel, ID_LAYERS_COUNT_INPUT, "1", wxDefaultPosition,
	                                  wxSize(Config::COMMON_NUM_FIELD_WIDTH, -1), wxTE_RIGHT);
	layersCountInput->SetRange(1, Config::MAX_LAYERS);
	layersCountSizer->Add(layersCountInput, 0, wxLEFT | wxRIGHT, 5);
	controlsToDisableOnPreview.push_back(layersCountInput);
	animationSettingsGridSizer->Add(layersCountSizer, 0, wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL);

	currentLayer = 0;
	auto layerControlsSizer = new wxBoxSizer(wxHORIZONTAL);
	auto prevLayerButton = new wxButton(animationPanel, ID_PREV_LAYER_BUTTON, "<", wxDefaultPosition,
																			wxSize(Config::SMALL_SQUARE_BUTTON_SIZE, Config::SMALL_SQUARE_BUTTON_SIZE));
	layerControlsSizer->Add(prevLayerButton, 0, wxLEFT, 5);
	controlsToDisableOnPreview.push_back(prevLayerButton);
	auto currentLayerLabel = new wxStaticText(animationPanel, -1, "lr #:");
	currentLayerLabel->SetFont(monospaceFont);
	layerControlsSizer->Add(currentLayerLabel, 0, wxLEFT | wxALIGN_CENTER_VERTICAL, 5);
	controlsToDisableOnPreview.push_back(currentLayerLabel);
	currentLayerNumber = new wxStaticText(animationPanel, -1, STR_ZERO);
	currentLayerNumber->SetFont(monospaceFont);
	layerControlsSizer->Add(currentLayerNumber, 0, wxLEFT | wxRIGHT | wxALIGN_CENTER_VERTICAL, 5);
	controlsToDisableOnPreview.push_back(currentLayerNumber);
	auto nextLayerButton = new wxButton(animationPanel, ID_NEXT_LAYER_BUTTON, ">", wxDefaultPosition,
																			wxSize(Config::SMALL_SQUARE_BUTTON_SIZE, Config::SMALL_SQUARE_BUTTON_SIZE));
	layerControlsSizer->Add(nextLayerButton);
	controlsToDisableOnPreview.push_back(nextLayerButton);
	animationSettingsGridSizer->Add(layerControlsSizer, 0, wxALIGN_LEFT);

	// amount of frames setting
	auto amountOfFramesSizer = new wxBoxSizer(wxHORIZONTAL);
	auto amountOfFramesLabel = new wxStaticText(animationPanel, -1, "Amount of frames:");
	amountOfFramesSizer->Add(amountOfFramesLabel, 0, wxALIGN_CENTER_VERTICAL);
	controlsToDisableOnPreview.push_back(amountOfFramesLabel);
	amountOfFramesInput = new wxSpinCtrl(animationPanel, ID_FRAMES_AMOUNT_INPUT, "1", wxDefaultPosition,
																			 wxSize(Config::COMMON_NUM_FIELD_WIDTH, -1), wxTE_RIGHT);
	amountOfFramesInput->SetRange(1, Config::MAX_ANIM_FRAMES);
	amountOfFramesSizer->Add(amountOfFramesInput, 0, wxLEFT | wxRIGHT, 5);
	controlsToDisableOnPreview.push_back(amountOfFramesInput);
	animationSettingsGridSizer->Add(amountOfFramesSizer, 0, wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL);

	// frame controls
	currentFrame = 0;
	auto frameControlsSizer = new wxBoxSizer(wxHORIZONTAL);
	auto prevFrameButton = new wxButton(animationPanel, ID_PREV_FRAME_BUTTON, "<", wxDefaultPosition,
	                                    wxSize(Config::SMALL_SQUARE_BUTTON_SIZE, Config::SMALL_SQUARE_BUTTON_SIZE));
	frameControlsSizer->Add(prevFrameButton, 0, wxLEFT, 5);
	controlsToDisableOnPreview.push_back(prevFrameButton);
	auto currentFrameLabel = new wxStaticText(animationPanel, -1, "fr #:");
	currentFrameLabel->SetFont(monospaceFont);
	frameControlsSizer->Add(currentFrameLabel, 0, wxLEFT | wxALIGN_CENTER_VERTICAL, 5);
	controlsToDisableOnPreview.push_back(currentFrameLabel);
	currentFrameNumber = new wxStaticText(animationPanel, -1, STR_ZERO);
	currentFrameNumber->SetFont(monospaceFont);
	frameControlsSizer->Add(currentFrameNumber, 0, wxLEFT | wxRIGHT | wxALIGN_CENTER_VERTICAL, 5);
	controlsToDisableOnPreview.push_back(currentFrameNumber);
	auto nextFrameButton = new wxButton(animationPanel, ID_NEXT_FRAME_BUTTON, ">", wxDefaultPosition,
	                                    wxSize(Config::SMALL_SQUARE_BUTTON_SIZE, Config::SMALL_SQUARE_BUTTON_SIZE));
	frameControlsSizer->Add(nextFrameButton);
	controlsToDisableOnPreview.push_back(nextFrameButton);
	animationSettingsGridSizer->Add(frameControlsSizer, 0, wxALIGN_LEFT);

	// always animated setting
	alwaysAnimatedCheckbox = new wxCheckBox(animationPanel, ID_ALWAYS_ANIMATED_CHECKBOX, "Always animated");
	animationSettingsGridSizer->Add(alwaysAnimatedCheckbox, 0, wxALIGN_CENTER);
	controlsToDisableOnPreview.push_back(alwaysAnimatedCheckbox);

	// always animated setting
	doBlendLayers = true;
	blendLayersCheckbox = new wxCheckBox(animationPanel, ID_BLEND_LAYERS_CHECKBOX, "Blend layers");
	blendLayersCheckbox->SetValue(doBlendLayers);
	animationSettingsGridSizer->Add(blendLayersCheckbox, 0, wxALIGN_CENTER);
	controlsToDisableOnPreview.push_back(blendLayersCheckbox);

	// animation preview
	auto fpsSizer = new wxBoxSizer(wxHORIZONTAL);
	auto fpsLabel = new wxStaticText(animationPanel, -1, "Preview FPS:");
	fpsSizer->Add(fpsLabel, 0, wxALIGN_CENTER_VERTICAL);
	previewFpsInput = new wxSpinCtrl(animationPanel, ID_PREVIEW_FPS_INPUT, "1", wxDefaultPosition,
	                                 wxSize(Config::COMMON_NUM_FIELD_WIDTH, -1), wxTE_RIGHT);
	previewFpsInput->SetRange(1, Config::MAX_PREVIEW_FPS);
	auto & fpsSetting = Settings::getInstance().get("previewFPS");
	if (fpsSetting.Length()) previewFpsInput->SetValue(wxAtoi(fpsSetting));
	fpsSizer->Add(previewFpsInput, 0, wxLEFT | wxRIGHT, 5);
	animationSettingsGridSizer->Add(fpsSizer, 0, wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL);

	auto previewButton = new wxButton(animationPanel, ID_PREVIEW_ANIMATION_BUTTON, "Preview animation");
	animationSettingsGridSizer->Add(previewButton, 0, wxALIGN_CENTER);

	animationPanelVAlignSizer->Add(animationSettingsGridSizer, 0, wxALIGN_CENTER);

	// always animated setting
	doDrawBlockingMarks = true;
	drawBlockingMarksCheckbox = new wxCheckBox(animationPanel, ID_DRAW_BLOCKING_MARKS_CHECKBOX,
	                                           "Draw 'blocking' state marks on sprites");
	drawBlockingMarksCheckbox->SetValue(doDrawBlockingMarks);

	animationPanelVAlignSizer->Add(drawBlockingMarksCheckbox, 0, wxALIGN_CENTER | wxTOP, 5);

	animationPanelSizer->Add(animationPanelVAlignSizer, 1, wxALIGN_CENTER);
	animationBoxSizer->Add(animationPanel, 1, wxEXPAND | wxRIGHT | wxBOTTOM, 5);
	midColumnGrid->Add(animationBox, 1, wxEXPAND);

	previewTimer = unique_ptr <wxTimer> (new wxTimer(this, ID_PREVIEW_TIMER));
	autoBackupTimer = unique_ptr <wxTimer> (new wxTimer(this, ID_AUTO_BACKUP_TIMER));

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
	lastLightColorValue = *wxWHITE;
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

	// 'blocking' mark icon; loading it and converting into byte arrays
	blockingMarkImage.LoadFile("res/icons/blocking_mark.png", wxBITMAP_TYPE_PNG);
	if (!blockingMarkImage.HasAlpha()) blockingMarkImage.InitAlpha();
	wxBitmap bmiBmp(blockingMarkImage);
	wxAlphaPixelData apd(bmiBmp);
	auto it = apd.GetPixels();
	int bmiW = blockingMarkImage.GetWidth(), bmiX = 0, bmiY = 0;
	auto pdlen = bmiW * blockingMarkImage.GetHeight();
	blockingMarkRGB = unique_ptr <unsigned char[]> (new unsigned char[pdlen * 3]);
	blockingMarkAlpha = unique_ptr <unsigned char[]> (new unsigned char[pdlen]);
	auto bmiRGB = blockingMarkRGB.get(), bmiAlpha = blockingMarkAlpha.get();
	for (int i = 0, j = 0; j < pdlen; i += 3, ++j)
	{
		it.MoveTo(apd, bmiX, bmiY);
		bmiRGB[i] = it.Red();
		bmiRGB[i + 1] = it.Green();
		bmiRGB[i + 2] = it.Blue();
		bmiAlpha[j] = it.Alpha();
		++bmiX;
		if (bmiX >= bmiW)
		{
			bmiX = 0;
			++bmiY;
		}
	}

	// variables
	currentXDiv = currentYDiv = 0;

	// disabling main panel and all nested controls until correct .dat / .spr files will be loaded or created
	mainPanel->Disable();
	animationPanel->Disable();
	booleanAttrsPanel->Disable();
	valueAttrsPanel->Disable();

	// auto-backup setup
	Settings & settings = Settings::getInstance();
	wxString autoBackupEnabledValue = settings.get("autoBackupEnabled");
	if (!autoBackupEnabledValue)
	{
		// enabling auto-backup by default
		autoBackupEnabledValue = "yes";
		settings.set("autoBackupEnabled", autoBackupEnabledValue);
		settings.set("autoBackupInterval", "10"); // every 10 minutes
		settings.save();
	}
	if (autoBackupEnabledValue.IsSameAs("yes"))
	{
		const wxString & autoBackupIntervalValue = settings.get("autoBackupInterval");
		autoBackupTimer->Start(wxAtoi(autoBackupIntervalValue) * 60000);
	}
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

	menuFile->FindChildItem(wxID_SAVE)->Enable();
	menuTools->FindChildItem(ID_MENU_EDIT_ADVANCED_ATTRS)->Enable();
	menuTools->FindChildItem(ID_MENU_GENERATE_RME)->Enable();
	menuTools->FindChildItem(ID_MENU_CLEANUP_SPRITES)->Enable();

	undoStack = stack <OperationInfo> (); // clearing
	redoStack = stack <OperationInfo> (); // stacks
	menuEdit->FindChildItem(wxID_UNDO)->Enable(false);
	menuEdit->FindChildItem(wxID_REDO)->Enable(false);

	DatSprReaderWriter::getInstance().initNewData();
	mainPanel->Enable();

	isDirty = false;

	statusBar->SetStatusText("New files initialized");
}

void MainWindow::OnOpenDatSprDialog(wxCommandEvent & event)
{
	if (isPreviewAnimationOn) stopAnimationPreview();

	bool isModeOpen = event.GetId() == wxID_OPEN;
	if (isModeOpen && !checkDirty()) return;

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
			wxMessageBox("There are no objects to save", Config::ERROR_TITLE, wxOK | wxICON_ERROR);
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

	menuFile->FindChildItem(wxID_SAVE)->Enable();
	menuTools->FindChildItem(ID_MENU_EDIT_ADVANCED_ATTRS)->Enable();
	menuTools->FindChildItem(ID_MENU_GENERATE_RME)->Enable();
	menuTools->FindChildItem(ID_MENU_CLEANUP_SPRITES)->Enable();

	undoStack = stack <OperationInfo> (); // clearing
	redoStack = stack <OperationInfo> (); // stacks
	menuEdit->FindChildItem(wxID_UNDO)->Enable(false);
	menuEdit->FindChildItem(wxID_REDO)->Enable(false);

	statusBar->SetStatusText("Files have been loaded successfully");
}

void MainWindow::OnDatSprSaved(wxCommandEvent & event)
{
	isDirty = false;
	statusBar->SetStatusText("Files have been saved successfully");
}

void MainWindow::OnObjectCategoryChanged(wxCommandEvent & event)
{
	if (isPreviewAnimationOn) stopAnimationPreview();

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

void MainWindow::fillObjectsListBox(unsigned int selectedIndex)
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
		objectsListBox->SetSelection(selectedIndex);
	}
}

void MainWindow::OnObjectSelected(wxCommandEvent & event)
{
	if (isPreviewAnimationOn) stopAnimationPreview();

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
	lastLightColorValue = *wxWHITE;
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
		lightColorPicker->Enable();
		// some weird formulas to convert 8-bit compressed RGB color into usual 24-bit
		unsigned short c = selectedObject->lightColor;
		int r = int(floor(c / 36)) % 6 * 0x33, g = int(floor(c / 6)) % 6 * 0x33, b = c % 6 * 0x33;
		wxColour color(r, g, b);
		lightColorPicker->SetColour(color);
		lastLightColorValue = color;
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
		lastLightColorValue = *wxWHITE;
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
	bool oldValue = changeBooleanAttribute(event.GetId(), value);
	auto & opInfo = addOperationInfo(BOOLEAN_ATTR_TOGGLE, oldValue, value);
	opInfo.controlID = event.GetId();
	isDirty = true;
}

bool MainWindow::changeBooleanAttribute(int id, bool value)
{
	bool oldValue = false;
	switch (id)
	{
		case ID_ATTR_IS_CONTAINER:
			oldValue = selectedObject->isContainer;
			selectedObject->isContainer = value;
			selectedObject->allAttrs[AttrContainer] = value;
			statusBar->SetStatusText(wxString::Format(OBJ_ATTR_CHANGE_STATUS_MSG, "Is container", value ? "on" : "off"));
		break;
		case ID_ATTR_IS_STACKABLE:
			oldValue = selectedObject->isStackable;
			selectedObject->isStackable = value;
			selectedObject->allAttrs[AttrStackable] = value;
			statusBar->SetStatusText(wxString::Format(OBJ_ATTR_CHANGE_STATUS_MSG, "Is stackable", value ? "on" : "off"));
		break;
		case ID_ATTR_IS_MULTI_USE:
			oldValue = selectedObject->isMultiUse;
			selectedObject->isMultiUse = value;
			selectedObject->allAttrs[AttrMultiUse] = value;
			statusBar->SetStatusText(wxString::Format(OBJ_ATTR_CHANGE_STATUS_MSG, "Multi-use", value ? "on" : "off"));
		break;
		case ID_ATTR_IS_WALKABLE:
			oldValue = selectedObject->isWalkable;
			selectedObject->isWalkable = value;
			selectedObject->allAttrs[AttrNotWalkable] = !value;
			statusBar->SetStatusText(wxString::Format(OBJ_ATTR_CHANGE_STATUS_MSG, "Is walkable", value ? "on" : "off"));
		break;
		case ID_ATTR_IS_PATHABLE:
			oldValue = selectedObject->isPathable;
			selectedObject->isPathable = value;
			selectedObject->allAttrs[AttrNotPathable] = !value;
			statusBar->SetStatusText(wxString::Format(OBJ_ATTR_CHANGE_STATUS_MSG, "Is pathable", value ? "on" : "off"));
		break;
		case ID_ATTR_IS_MOVABLE:
			oldValue = selectedObject->isMovable;
			selectedObject->isMovable = value;
			selectedObject->allAttrs[AttrNotMoveable] = !value;
			statusBar->SetStatusText(wxString::Format(OBJ_ATTR_CHANGE_STATUS_MSG, "Is movable", value ? "on" : "off"));
		break;
		case ID_ATTR_BLOCKS_PROJECTILES:
			oldValue = selectedObject->blocksProjectiles;
			selectedObject->blocksProjectiles = value;
			selectedObject->allAttrs[AttrBlockProjectile] = value;
			statusBar->SetStatusText(wxString::Format(OBJ_ATTR_CHANGE_STATUS_MSG, "Block projectiles", value ? "on" : "off"));
		break;
		case ID_ATTR_IS_PICKUPABLE:
			oldValue = selectedObject->isPickupable;
			selectedObject->isPickupable = value;
			selectedObject->allAttrs[AttrPickupable] = value;
			statusBar->SetStatusText(wxString::Format(OBJ_ATTR_CHANGE_STATUS_MSG, "Is pickupable", value ? "on" : "off"));
		break;
		case ID_ATTR_IGNORE_LOOK:
			oldValue = selectedObject->ignoreLook;
			selectedObject->ignoreLook = value;
			selectedObject->allAttrs[AttrLook] = value;
			statusBar->SetStatusText(wxString::Format(OBJ_ATTR_CHANGE_STATUS_MSG, "Ignore look", value ? "on" : "off"));
		break;
		case ID_ATTR_IS_HANGABLE:
			oldValue = selectedObject->isHangable;
			selectedObject->isHangable = value;
			selectedObject->allAttrs[AttrHangable] = value;
			statusBar->SetStatusText(wxString::Format(OBJ_ATTR_CHANGE_STATUS_MSG, "Is hangable", value ? "on" : "off"));
		break;
		case ID_ATTR_IS_LYING_CORPSE:
			oldValue = selectedObject->isLyingCorpse;
			selectedObject->isLyingCorpse = value;
			selectedObject->allAttrs[AttrLyingCorpse] = value;
			statusBar->SetStatusText(wxString::Format(OBJ_ATTR_CHANGE_STATUS_MSG, "Is lying corpse", value ? "on" : "off"));
		break;
		case ID_ATTR_HAS_MOUNT:
			oldValue = selectedObject->hasMount;
			selectedObject->hasMount = value;
			selectedObject->allAttrs[AttrMount] = value;
			statusBar->SetStatusText(wxString::Format(OBJ_ATTR_CHANGE_STATUS_MSG, "Has mount", value ? "on" : "off"));
		break;
	}
	return oldValue;
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
				staticBitmap = new wxGenericStaticBitmap(objectSpritesPanel, ID_OBJ_SPRITES_GSB, bitmap);

				auto esi = new EditorSpriteIDs();
				esi->setType(EditorSpriteIDs::EXISTING);
				esi->setObjectSpriteIndex(i);
				esi->getSpriteIDs().push_back(sprite->id);
				esi->setIndexInVector(editorSpriteIDs.size());
				editorSpriteIDs.push_back(esi);
				staticBitmap->SetClientData(esi);

				staticBitmap->Bind(wxEVT_LEFT_DOWN, &MainWindow::OnClickImportedOrObjectSprite, this);
				staticBitmap->Bind(wxEVT_RIGHT_UP, &MainWindow::OnRightClickObjectSprite, this);

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

void MainWindow::fillAnimationSection(bool resetIterators)
{
	if (selectedObject)
	{
		if (resetIterators)
		{
			currentXDiv = currentYDiv = currentLayer = currentFrame = 0;
		}

		animationWidthInput->SetValue(selectedObject->width);
		animationHeightInput->SetValue(selectedObject->height);

		patternWidthInput->SetValue(selectedObject->patternWidth);
		patternHeightInput->SetValue(selectedObject->patternHeight);
		currentXDivLabel->SetLabelText(wxString::Format("%i", currentXDiv));
		currentYDivLabel->SetLabelText(wxString::Format("%i", currentYDiv));

		layersCountInput->SetValue(selectedObject->layersCount);
		currentLayerNumber->SetLabelText(wxString::Format("%i", currentLayer));

		currentFrameNumber->SetLabelText(wxString::Format("%i", currentFrame));
		amountOfFramesInput->SetValue(selectedObject->phasesCount);

		alwaysAnimatedCheckbox->SetValue(selectedObject->isAlwaysAnimated);

		fillAnimationSprites();
	}
	else
	{
		if (resetIterators)
		{
			currentXDiv = currentYDiv = currentLayer = currentFrame = 0;
		}

		animationWidthInput->SetValue(0);
		animationHeightInput->SetValue(0);

		patternWidthInput->SetValue(0);
		patternHeightInput->SetValue(0);
		currentXDivLabel->SetLabelText(STR_ZERO);
		currentYDivLabel->SetLabelText(STR_ZERO);

		layersCountInput->SetValue(0);

		currentFrameNumber->SetLabelText(STR_ZERO);
		amountOfFramesInput->SetValue(0);

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
		animationGridBitmaps[i] = new wxGenericStaticBitmap(animationSpritesPanel, ID_ANIM_GRID_BITMAP,
				emptyBitmap, wxDefaultPosition, wxSize(Config::SPRITE_SIZE, Config::SPRITE_SIZE));
		animationSpritesSizer->Add(animationGridBitmaps[i]);

		// for sprites drag & drop
		auto dropTarget = new AnimationSpriteDropTarget();
		dropTarget->setMainWindow(this);
		dropTarget->setSpriteHolderIndex(i);
		animationGridBitmaps[i]->SetDropTarget(dropTarget);

		auto onEnter = [&](wxMouseEvent & event)
		{
			if (isPreviewAnimationOn) return;
			auto animGridBitmap = dynamic_cast <wxGenericStaticBitmap *> (event.GetEventObject());
			drawAnimationSpriteSelection(animGridBitmap);
		};
		auto onLeave = [&](wxMouseEvent & event)
		{
			if (isPreviewAnimationOn) return;
			auto animGridBitmap = dynamic_cast <wxGenericStaticBitmap *> (event.GetEventObject());
			clearAnimationSpriteSelection(animGridBitmap);
		};
		animationGridBitmaps[i]->Bind(wxEVT_ENTER_WINDOW, onEnter);
		animationGridBitmaps[i]->Bind(wxEVT_LEAVE_WINDOW, onLeave);
		animationGridBitmaps[i]->Bind(wxEVT_RIGHT_UP, &MainWindow::OnRightClickAnimGridCell, this);
	}

	animationSpritesPanel->SetSizer(animationSpritesSizer, true);
	animationMainGridSizer->Insert(4, animationSpritesPanel);
	animationBoxSizer->Layout();
}

void MainWindow::fillAnimationSprites()
{
	int frameSize = selectedObject->width * selectedObject->height;
	// these buffers will be used for manual blending
	unsigned char ** bitmapsRGB = new unsigned char * [frameSize];
	unsigned char ** bitmapsAlpha = new unsigned char * [frameSize];

	fillBitmapBuffers(bitmapsRGB, bitmapsAlpha);

	// now copying processed (and probably blended) buffers into wxGenericStaticBitmaps
	for (int i = 0; i < frameSize; ++i)
	{
		wxImage image(32, 32, bitmapsRGB[i], bitmapsAlpha[i], true);
		animationGridBitmaps[i]->SetBitmap(wxBitmap(image));
		delete [] bitmapsRGB[i];
		delete [] bitmapsAlpha[i];
	}
	delete [] bitmapsRGB;
	delete [] bitmapsAlpha;
}

void MainWindow::fillBitmapBuffers(unsigned char ** bitmapsRGB, unsigned char ** bitmapsAlpha)
{
	auto sprites = DatSprReaderWriter::getInstance().getSprites();
	shared_ptr <Sprite> sprite = nullptr;
	unsigned int spriteId = 0;
	int frameSize = selectedObject->width * selectedObject->height;

	for (int i = 0; i < frameSize; ++i)
	{
		bitmapsRGB[i] = new unsigned char[Sprite::RGB_SIZE];
		bitmapsAlpha[i] = new unsigned char[Sprite::ALPHA_SIZE];
		memset(bitmapsRGB[i], 0, Sprite::RGB_SIZE);
		memset(bitmapsAlpha[i], 0, Sprite::ALPHA_SIZE);
	}

	int patternSize = selectedObject->patternWidth * selectedObject->patternHeight;
	int startLayer = doBlendLayers ? 0 : currentLayer;
	int endLayer = doBlendLayers ? selectedObject->layersCount - 1 : startLayer;
	for (int layer = startLayer; layer <= endLayer; ++layer)
	{
		// sprites are grouped hierarchically, at first they're grouped into animation frames
		// then they're grouped in 'patterns', for creatures xDiv pattern mostly used to provide different
		// 'orientations' of the creature
		// finally all that grouped into layers, thus layers are on top of hierarchy
		int xyDivIndex = currentYDiv * selectedObject->patternWidth + currentXDiv;
		int frameIndex = currentFrame * patternSize * selectedObject->layersCount;
		int startSprite = (layer + xyDivIndex  + frameIndex) * frameSize;
		int endSprite = startSprite + frameSize;
		for (int i = endSprite - 1, j = 0; i >= startSprite; --i, ++j)
		{
			spriteId = selectedObject->spriteIDs[i];
			if (spriteId != 0 && sprites->find(spriteId) != sprites->end())
			{
				sprite = sprites->at(spriteId);
				if (sprite->valid)
				{
					if (!doBlendLayers || layer == 0) // filling bottom layer (or single layer if not blending)
					{
						// for first layer just copying data
						memcpy(bitmapsRGB[j], sprite->rgb.get(), Sprite::RGB_SIZE);
						memcpy(bitmapsAlpha[j], sprite->alpha.get(), Sprite::ALPHA_SIZE);
					}
					else
					{
						// for other layers doing manual alpha-blending
						blendBitmapBuffers(bitmapsRGB[j], bitmapsAlpha[j], sprite->rgb.get(), sprite->alpha.get());
					}
				}

				if (doDrawBlockingMarks && layer == endLayer && sprite->isBlocking)
				{
					// blending 'blocking' mark into resulting image
					blendBitmapBuffers(bitmapsRGB[j], bitmapsAlpha[j], blockingMarkRGB.get(), blockingMarkAlpha.get(),
					                   Config::SPRITE_SIZE, Config::SPRITE_SIZE,
					                   blockingMarkImage.GetWidth(), blockingMarkImage.GetHeight(),
					                   Config::BLOCKING_MARK_X, Config::BLOCKING_MARK_Y);
				}
			}
		}
	}
}

void MainWindow::blendBitmapBuffers(unsigned char * destRGB, unsigned char * destAlpha,
																		unsigned char * srcRGB, unsigned char * srcAlpha,
																		unsigned int destWidth, unsigned int destHeight,
																		unsigned int srcWidth, unsigned int srcHeight,
																		unsigned int xOffset, unsigned int yOffset)
{
	unsigned int dap = yOffset * destWidth + xOffset; // destination alpha pointer
	unsigned int drp = dap * 3; // destination RGB pointer
	unsigned int sap = 0, srp = 0; // no offsets in source for now
	for (unsigned int y = 0; y < srcHeight; ++y)
	{
		for (unsigned int x = 0; x < srcWidth; ++x)
		{
			float srca = srcAlpha[sap] / 255.0f, dsta = destAlpha[dap] / 255.0f;
			float msrca = 1.0f - srca;
			destRGB[drp] = srcRGB[srp] * srca + destRGB[drp] * dsta * msrca;
			destRGB[drp + 1] = srcRGB[srp + 1] * srca + destRGB[drp + 1]  * dsta * msrca;
			destRGB[drp + 2] = srcRGB[srp + 2] * srca + destRGB[drp + 2]  * dsta * msrca;
			destAlpha[dap] = (srca + dsta * msrca) * 255.0f;
			drp += 3;
			srp += 3;
			dap++;
			sap++;
		}
		drp += (destWidth - srcWidth) * 3;
		dap += (destWidth - srcWidth);
	}
}

void MainWindow::OnAnimWidthChanged(wxSpinEvent & event)
{
	if (!animationWidthInput) return;
	
	unsigned int val = animationWidthInput->GetValue();
	addOperationInfo(ANIM_WIDTH_CHANGE, selectedObject->width, val);
	
	selectedObject->width = val;
	resizeObjectSpriteIDsArray(selectedObject);
	buildAnimationSpriteHolders();
	fillAnimationSprites();

	isDirty = true;

	statusBar->SetStatusText(wxString::Format("Object width changed to %i", val));
}

void MainWindow::OnAnimHeightChanged(wxSpinEvent & event)
{
	if (!animationHeightInput) return;

	unsigned int val = animationHeightInput->GetValue();
	addOperationInfo(ANIM_HEIGHT_CHANGE, selectedObject->height, val);
	
	selectedObject->height = val;
	resizeObjectSpriteIDsArray(selectedObject);
	buildAnimationSpriteHolders();
	fillAnimationSprites();

	isDirty = true;

	statusBar->SetStatusText(wxString::Format("Object height changed to %i", val));
}

void MainWindow::OnFramesAmountChanged(wxSpinEvent & event)
{
	if (!amountOfFramesInput) return;

	unsigned int val = amountOfFramesInput->GetValue();
	addOperationInfo(AMOUNT_OF_FRAMES_CHANGE, selectedObject->phasesCount, val);

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
				patternWidthInput->SetValue(selectedObject->patternWidth);
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

	currentXDiv = xDiv;
	currentXDivLabel->SetLabelText(wxString::Format("%i", currentXDiv));

	// resetting frame when switching orientation
	currentFrame = 0;
	currentFrameNumber->SetLabelText(wxString::Format("%i", currentFrame));

	fillAnimationSprites();
}

void MainWindow::OnPatternWidthChanged(wxSpinEvent & event)
{
	unsigned int val = event.GetInt();
	addOperationInfo(PATTERN_WIDTH_CHANGE, selectedObject->patternWidth, val);

	selectedObject->patternWidth = val;
	resizeObjectSpriteIDsArray(selectedObject);
	if (currentXDiv >= selectedObject->patternWidth)
	{
		currentXDiv = selectedObject->patternWidth - 1;
		currentXDivLabel->SetLabelText(wxString::Format("%i", currentXDiv));
		fillAnimationSprites();
	}

	isDirty = true;

	statusBar->SetStatusText(wxString::Format("Animation pattern width changed to %i", val));
}

void MainWindow::OnPatternHeightChanged(wxSpinEvent & event)
{
	unsigned int val = event.GetInt();
	addOperationInfo(PATTERN_HEIGHT_CHANGE, selectedObject->patternHeight, val);

	selectedObject->patternHeight = val;
	resizeObjectSpriteIDsArray(selectedObject);
	if (currentYDiv >= selectedObject->patternHeight)
	{
		currentYDiv = selectedObject->patternHeight - 1;
		currentYDivLabel->SetLabelText(wxString::Format("%i", currentYDiv));
		fillAnimationSprites();
	}

	isDirty = true;

	statusBar->SetStatusText(wxString::Format("Animation pattern height changed to %i", val));
}

void MainWindow::OnClickPrevXDivButton(wxCommandEvent & event)
{
	if (currentXDiv == 0) currentXDiv = selectedObject->patternWidth - 1;
	else currentXDiv--;
	currentXDivLabel->SetLabelText(wxString::Format("%i", currentXDiv));
	fillAnimationSprites();
}

void MainWindow::OnClickNextXDivButton(wxCommandEvent & event)
{
	currentXDiv++;
	if (currentXDiv >= selectedObject->patternWidth) currentXDiv = 0;
	currentXDivLabel->SetLabelText(wxString::Format("%i", currentXDiv));
	fillAnimationSprites();
}

void MainWindow::OnClickPrevYDivButton(wxCommandEvent & event)
{
	if (currentYDiv == 0) currentYDiv = selectedObject->patternHeight - 1;
	else currentYDiv--;
	currentYDivLabel->SetLabelText(wxString::Format("%i", currentYDiv));
	fillAnimationSprites();
}

void MainWindow::OnClickNextYDivButton(wxCommandEvent & event)
{
	currentYDiv++;
	if (currentYDiv >= selectedObject->patternHeight) currentYDiv = 0;
	currentYDivLabel->SetLabelText(wxString::Format("%i", currentYDiv));
	fillAnimationSprites();
}

void MainWindow::OnLayersCountChanged(wxSpinEvent & event)
{
	unsigned int val = event.GetInt();
	addOperationInfo(LAYERS_COUNT_CHANGE, selectedObject->layersCount, val);

	selectedObject->layersCount = val;
	resizeObjectSpriteIDsArray(selectedObject);
	if (currentLayer >= selectedObject->layersCount)
	{
		currentLayer = selectedObject->patternWidth - 1;
		currentLayerNumber->SetLabelText(wxString::Format("%i", currentLayer));
		fillAnimationSprites();
	}

	isDirty = true;

	statusBar->SetStatusText(wxString::Format("Animation layers count changed to %i", val));
}

void MainWindow::OnClickPrevLayerButton(wxCommandEvent & event)
{
	if (currentLayer == 0) currentLayer = selectedObject->layersCount - 1;
	else currentLayer--;
	currentLayerNumber->SetLabelText(wxString::Format("%i", currentLayer));
	fillAnimationSprites();
}

void MainWindow::OnClickNextLayerButton(wxCommandEvent & event)
{
	currentLayer++;
	if (currentLayer >= selectedObject->layersCount) currentLayer = 0;
	currentLayerNumber->SetLabelText(wxString::Format("%i", currentLayer));
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

void MainWindow::OnPreviewFPSChanged(wxSpinEvent & event)
{
	if (isPreviewAnimationOn)
	{
		int val = previewFpsInput->GetValue();
		auto & settings = Settings::getInstance();
		settings.set("previewFPS", wxString::Format("%i", val));
		settings.save();
		previewTimer->Stop();
		previewTimer->Start(1000 / val);
	}
}

void MainWindow::OnClickPreviewAnimationButton(wxCommandEvent & event)
{
	if (!isPreviewAnimationOn)
	{
		startAnimationPreview();
	}
	else
	{
		stopAnimationPreview();
	}
}

void MainWindow::startAnimationPreview()
{
	isPreviewAnimationOn = true;
	lastCurrentFrame = currentFrame;
	previewTimer->Start(1000 / previewFpsInput->GetValue());
	dynamic_cast <wxButton *> (FindWindow(ID_PREVIEW_ANIMATION_BUTTON))->SetLabelText("Stop preview");
	statusBar->SetStatusText("Animation preview started");
	for (auto elem : controlsToDisableOnPreview)
	{
		elem->Enable(false);
	}
}

void MainWindow::stopAnimationPreview()
{
	isPreviewAnimationOn = false;
	previewTimer->Stop();
	currentFrame = lastCurrentFrame;
	fillAnimationSprites();
	dynamic_cast <wxButton *> (FindWindow(ID_PREVIEW_ANIMATION_BUTTON))->SetLabelText("Preview animation");
	statusBar->SetStatusText("Animation preview stopped");
	for (auto elem : controlsToDisableOnPreview)
	{
		elem->Enable(true);
	}
}

void MainWindow::OnPreviewTimerEvent(wxTimerEvent & event)
{
	currentFrame++;
	if (currentFrame >= selectedObject->phasesCount) currentFrame = 0;
	fillAnimationSprites();
}

void MainWindow::OnAutoBackupTimerEvent(wxTimerEvent & event)
{
	if (isPreviewAnimationOn) stopAnimationPreview();
	AutoBackupDialog autoBackupDialog(this);
	autoBackupDialog.ShowModal();
}

void MainWindow::OnClickNewObjectButton(wxCommandEvent & event)
{
	if (isPreviewAnimationOn) stopAnimationPreview();

	auto objects = DatSprReaderWriter::getInstance().getObjects(currentCategory);
	auto newObject = make_shared <DatObject> ();
	newObject->id = objects->size() + (currentCategory == CategoryItem ? 100 : 1);
	newObject->width = newObject->height = 1;
	newObject->patternWidth = newObject->patternHeight = newObject->patternDepth = 1;
	newObject->layersCount = newObject->phasesCount = newObject->spriteCount = 1;
	newObject->spriteIDs = unique_ptr <unsigned int[]> (new unsigned int[1] { 0 });
	for (int i = 0; i < AttrLast; ++i)
	{
		newObject->allAttrs[i] = false;
	}
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

void MainWindow::OnClickDeleteObjectButton(wxCommandEvent & event)
{
	deleteSelectedObject();
}

void MainWindow::deleteSelectedObject()
{
	const wxString & msg = wxString::Format("Are you sure you want to delete object <%i> from \"%s\"?",
		                                        selectedObject->id, CATEGORIES[currentCategory]);
	if (selectedObject && wxMessageBox(msg, "Confirm delete", wxYES_NO | wxCANCEL) == wxYES)
	{
		unsigned int id = selectedObject->id;
		auto ret = processObjectDeletion(currentCategory, id);
		deletedObjectsByCatAndId[currentCategory][id] = ret.first;
		deletedAttrsByCatAndId[currentCategory][id] = ret.second;

		// putting currentCategory instead of newIntValue
		addOperationInfo(OBJECT_DELETION, id, currentCategory);

		statusBar->SetStatusText(wxString::Format("Deleted object <%i>", id));

		isDirty = true;
	}
}

pair <shared_ptr <DatObject>, shared_ptr <AdvancedObjectAttributes>>
MainWindow::processObjectDeletion(DatObjectCategory category, unsigned int id)
{
	if (isPreviewAnimationOn) stopAnimationPreview();

	auto objects = DatSprReaderWriter::getInstance().getObjects(category);
	auto & attrs = AdvancedAttributesManager::getInstance().getCategoryAttributes(category);
	unsigned int index = id - (category == CategoryItem ? 100 : 1);

	// remapping advanced attributes first
	unsigned int attrsID = id, endID = id + (objects->size() - index) - 1;
	auto deletedAttrs = attrs[attrsID];
	attrs.erase(attrs.find(attrsID));
	for (; attrsID < endID; ++attrsID)
	{
		attrs[attrsID] = attrs[attrsID + 1];
	}

	// then erasing object itself
	auto deletedObject = objects->at(index);
	objects->erase(objects->begin() + index);
	if (index < objects->size())
	{
		selectedObject = objects->at(index); // moving to next object
		while (index < objects->size())
		{
			objects->at(index++)->id = id++; // reindexing IDs
		}
		index--;
	}
	else if (index > 0)
	{
		selectedObject = objects->at(--index); // moving to previous object
	}
	else
	{
		selectedObject = nullptr;
		animationPanel->Disable();
		booleanAttrsPanel->Disable();
		valueAttrsPanel->Disable();
	}

	fillObjectsListBox(index);
	setAttributeValues();
	fillObjectSprites();
	buildAnimationSpriteHolders();
	fillAnimationSection();

	return pair <shared_ptr <DatObject>, shared_ptr <AdvancedObjectAttributes>> (deletedObject, deletedAttrs);
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

		unsigned int imageID = 0;
		shared_ptr <wxImage> image = nullptr;
		wxGenericStaticBitmap * staticBitmap = nullptr;
		wxStaticText * spriteLabel = nullptr;
		wxFileName filename;
		wxString basename;
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

			imageID = importedSprites.size();
			image = make_shared <wxImage> (p, wxBITMAP_TYPE_PNG);
			if (image->HasMask()) image->InitAlpha(); // converting mask to alpha

			if (!image->IsOk())
			{
				errors.Append(wxString::Format("%s has not been imported, because loading file has failed\n\n", basename));
				continue;
			}
			if (image->GetWidth() % 32 != 0 || image->GetHeight() % 32 != 0)
			{
				errors.Append(wxString::Format("%s has not been imported, because its width and height must be multiplier of 32\n\n", basename));
				continue;
			}
			importedSprites.push_back(image);
			staticBitmap = new wxGenericStaticBitmap(newSpritesPanel, -1, wxBitmap(*image));

			auto esi = new EditorSpriteIDs();
			esi->setType(EditorSpriteIDs::IMPORTED);
			esi->getSpriteIDs().push_back(imageID);
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
			wxMessageDialog errorMsg(this, errors, Config::ERROR_TITLE, wxOK | wxICON_ERROR);
			errorMsg.SetMinSize(wxSize(300, -1));
			errorMsg.ShowModal();
			statusBar->SetStatusText("Errors occured while importing new files");
		}
		else if (paths.size() > 0)
		{
			if (paths.size() == 1)
			{
				filename.Assign(paths[0]);
				statusBar->SetStatusText(wxString::Format("New sprite, named \"%s\", has been imported", filename.GetName()));
			}
			else
			{
				statusBar->SetStatusText(wxString::Format("%li new sprites have been imported", paths.size()));
			}
		}
	}
}

void MainWindow::OnClickImportedOrObjectSprite(wxMouseEvent & event)
{
	auto staticBitmap = dynamic_cast <wxGenericStaticBitmap *> (event.GetEventObject());
	wxDropSource dropSource(staticBitmap);
	auto esi = (EditorSpriteIDs *) staticBitmap->GetClientData();
	char strEsiIndex[5];
	sprintf(strEsiIndex, "%i", esi->getIndexInVector());
	wxTextDataObject data(strEsiIndex);
	dropSource.SetData(data);
	dropSource.DoDragDrop(true);
}

void MainWindow::OnRightClickAnimGridCell(wxMouseEvent & event)
{
	if (isPreviewAnimationOn) return;
	contextMenuTargetBitmap = dynamic_cast <wxGenericStaticBitmap *> (event.GetEventObject());
	wxMenu contextMenu;
	contextMenu.Append(ID_MENU_EXPORT_SPRITE, "Export this sprite into PNG-file");
	if (selectedObject->width > 1 || selectedObject->height > 1)
	{
		contextMenu.Append(ID_MENU_EXPORT_COMPOSED, "Export composed frame into PNG-file");
	}
	contextMenu.Append(ID_MENU_DELETE_SPRITE, "Delete this sprite");
	contextMenu.AppendSeparator();
	contextMenu.Append(ID_MENU_TOGGLE_SPRITE_BLOCKING, "Toggle sprite 'blocking' state");
	contextMenu.Append(ID_MENU_TOGGLE_SPRITE_BLOCKING_ALL_FRAMES, "Toggle spite 'blocking' state on all frames");
	auto onMenuClose = [&](wxMenuEvent & event)
	{
		clearAnimationSpriteSelection(contextMenuTargetBitmap);
	};
	contextMenu.Bind(wxEVT_MENU_CLOSE, onMenuClose);
	PopupMenu(&contextMenu);
}

void MainWindow::OnRightClickObjectSprite(wxMouseEvent & event)
{
	if (isPreviewAnimationOn) return;
	contextMenuTargetBitmap = dynamic_cast <wxGenericStaticBitmap *> (event.GetEventObject());
	wxMenu contextMenu;
	contextMenu.Append(ID_MENU_EXPORT_SPRITE, "Export this sprite into PNG-file");
	contextMenu.Append(ID_MENU_DELETE_SPRITE, "Delete this sprite");
	PopupMenu(&contextMenu);
}

void MainWindow::OnToggleAlwaysAnimatedAttr(wxCommandEvent & event)
{
	bool value = event.GetInt() != 0;
	addOperationInfo(ALWAYS_ANIMATED_TOGGLE, selectedObject->isAlwaysAnimated, value);

	selectedObject->isAlwaysAnimated = value;
	selectedObject->allAttrs[AttrAnimateAlways] = value;

	isDirty = true;

	statusBar->SetStatusText(wxString::Format(OBJ_ATTR_CHANGE_STATUS_MSG, "Always animated", value ? "on" : "off"));
}

void MainWindow::OnToggleBlendLayersCheckbox(wxCommandEvent & event)
{
	doBlendLayers = event.GetInt() != 0;
	fillAnimationSection(false);
	statusBar->SetStatusText(wxString::Format("Layers blending is %s", doBlendLayers ? "on" : "off"));
}

void MainWindow::OnToggleDrawBlockingMarksCheckbox(wxCommandEvent & event)
{
	doDrawBlockingMarks = event.GetInt() != 0;
	fillAnimationSection(false);
	statusBar->SetStatusText(wxString::Format("Drawing 'blocking' states on sprites is %s",
																						doDrawBlockingMarks ? "on" : "off"));
}

void MainWindow::OnToggleIsFullGroundAttr(wxCommandEvent & event)
{
	bool value = event.GetInt() != 0, oldValue = selectedObject->isFullGround;
	groundSpeedLabel->Enable(value);
	groundSpeedInput->Enable(value);
	selectedObject->isGround = selectedObject->isFullGround = value;
	selectedObject->allAttrs[AttrGround] = selectedObject->allAttrs[AttrFullGround] = value;
	selectedObject->groundSpeed = value ? wxAtoi(groundSpeedInput->GetValue()) : 0;

	auto & opInfo = addOperationInfo(FULL_GROUND_TOGGLE, oldValue, value);
	opInfo.controlID = ID_ATTR_IS_FULL_GROUND;

	isDirty = true;

	statusBar->SetStatusText(wxString::Format(OBJ_ATTR_CHANGE_STATUS_MSG, "Is full ground", value ? "on" : "off"));
}

void MainWindow::OnToggleHasLightAttr(wxCommandEvent & event)
{
	bool value = event.GetInt() != 0, oldValue = selectedObject->isLightSource;
	lightColorLabel->Enable(value);
	lightColorPicker->Enable(value);
	lightIntensityLabel->Enable(value);
	lightIntensityInput->Enable(value);
	selectedObject->isLightSource = value;
	selectedObject->allAttrs[AttrLight] = value;
	const wxColour & color = lightColorPicker->GetColour();
	if (value)
	{
		selectedObject->lightColor = floor(color.Red() / 0x33) * 36 + floor(color.Green() / 0x33) * 6
															 + floor(color.Blue() / 0x33);
		selectedObject->lightIntensity = wxAtoi(lightIntensityInput->GetValue());
	}
	else
	{
		selectedObject->lightColor = selectedObject->lightIntensity = 0;
	}

	auto & opInfo = addOperationInfo(HAS_LIGHT_TOGGLE, oldValue, value);
	opInfo.controlID = ID_ATTR_HAS_LIGHT;

	isDirty = true;

	statusBar->SetStatusText(wxString::Format(OBJ_ATTR_CHANGE_STATUS_MSG, "Has light", value ? "on" : "off"));
}

void MainWindow::OnToggleHasOffsetAttr(wxCommandEvent & event)
{
	bool value = event.GetInt() != 0, oldValue = selectedObject->hasDisplacement;
	offsetXLabel->Enable(value);
	offsetXInput->Enable(value);
	offsetYLabel->Enable(value);
	offsetYInput->Enable(value);
	selectedObject->hasDisplacement = value;
	selectedObject->allAttrs[AttrDisplacement] = value;
	if (value)
	{
		selectedObject->displacementX = wxAtoi(offsetXInput->GetValue());
		selectedObject->displacementY = wxAtoi(offsetYInput->GetValue());
	}
	else
	{
		selectedObject->displacementX = selectedObject->displacementY = 0;
	}

	auto & opInfo = addOperationInfo(HAS_OFFSET_TOGGLE, oldValue, value);
	opInfo.controlID = ID_ATTR_HAS_OFFSET;

	isDirty = true;

	statusBar->SetStatusText(wxString::Format(OBJ_ATTR_CHANGE_STATUS_MSG, "Has offset", value ? "on" : "off"));
}

void MainWindow::OnToggleHasElevationAttr(wxCommandEvent & event)
{
	bool value = event.GetInt() != 0, oldValue = selectedObject->isRaised;
	elevationLabel->Enable(value);
	elevationInput->Enable(value);
	selectedObject->isRaised = value;
	selectedObject->allAttrs[AttrElevation] = value;
	selectedObject->elevation = value ? wxAtoi(elevationInput->GetValue()) : 0;

	auto & opInfo = addOperationInfo(HAS_ELEVATION_TOGGLE, oldValue, value);
	opInfo.controlID = ID_ATTR_HAS_ELEVATION;

	isDirty = true;

	statusBar->SetStatusText(wxString::Format(OBJ_ATTR_CHANGE_STATUS_MSG, "Has elevation", value ? "on" : "off"));
}

void MainWindow::OnGroundSpeedChanged(wxCommandEvent & event)
{
	if (selectedObject)
	{
		unsigned int val = wxAtoi(groundSpeedInput->GetValue());
		addOperationInfo(GROUND_SPEED_CHANGE, selectedObject->groundSpeed, val);
		selectedObject->groundSpeed = val;
		statusBar->SetStatusText(wxString::Format("Object \"Ground speed\" value changed to %i", selectedObject->groundSpeed));
		isDirty = true;
	}
}

void MainWindow::OnLightColorChanged(wxColourPickerEvent & event)
{
	if (selectedObject)
	{
		const wxColour & color = event.GetColour();
		addOperationInfo(LIGHT_COLOR_CHANGE, lastLightColorValue.GetRGB(), color.GetRGB());
		selectedObject->lightColor = floor(color.Red() / 0x33) * 36 + floor(color.Green() / 0x33) * 6
		                           + floor(color.Blue() / 0x33);
		lastLightColorValue = color;
		isDirty = true;
	}
}

void MainWindow::OnLightIntensityChanged(wxCommandEvent & event)
{
	if (selectedObject)
	{
		unsigned int val = wxAtoi(lightIntensityInput->GetValue());
		addOperationInfo(LIGHT_INTENSITY_CHANGE, selectedObject->lightIntensity, val);
		selectedObject->lightIntensity = val;
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
			unsigned int val = wxAtoi(offsetXInput->GetValue());
			auto & opInfo = addOperationInfo(OFFSET_XY_CHANGE, selectedObject->displacementX, val);
			opInfo.controlID = ID_OFFSET_X_INPUT;
			selectedObject->displacementX = val;
			statusBar->SetStatusText(wxString::Format("Object \"Offset X\" value changed to %i", selectedObject->displacementX));
		}
		else if (id == ID_OFFSET_Y_INPUT)
		{
			unsigned int val = wxAtoi(offsetYInput->GetValue());
			auto & opInfo = addOperationInfo(OFFSET_XY_CHANGE, selectedObject->displacementY, val);
			opInfo.controlID = ID_OFFSET_Y_INPUT;
			selectedObject->displacementY = val;
			statusBar->SetStatusText(wxString::Format("Object \"Offset Y\" value changed to %i", selectedObject->displacementY));
		}
		isDirty = true;
	}
}

void MainWindow::OnElevationChanged(wxCommandEvent & event)
{
	if (selectedObject)
	{
		unsigned int val = wxAtoi(elevationInput->GetValue());
		addOperationInfo(ELEVATION_CHANGE, selectedObject->elevation, val);
		selectedObject->elevation = val;
		statusBar->SetStatusText(wxString::Format("Object \"Elevation\" value changed to %i", selectedObject->elevation));
		isDirty = true;
	}
}

void MainWindow::drawAnimationSpriteSelection(wxGenericStaticBitmap * animGridBitmap)
{
	auto currentBitmap = new wxBitmap(animGridBitmap->GetBitmap().GetSubBitmap(
			wxRect(0, 0, Config::SPRITE_SIZE, Config::SPRITE_SIZE) // getting copy of original bitmap
	));
	animGridBitmap->SetClientData(currentBitmap);
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
	animGridBitmap->SetBitmap(tmpBmp);
}

void MainWindow::clearAnimationSpriteSelection(wxGenericStaticBitmap * animGridBitmap)
{
	if (animGridBitmap->GetClientData())
	{
		auto bmp = reinterpret_cast <wxBitmap *> (animGridBitmap->GetClientData());
		animGridBitmap->SetBitmap(*bmp);
		animGridBitmap->SetClientData(nullptr);
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
	unsigned int minSpriteCount = std::min(object->spriteCount, oldSpriteCount);
	for (unsigned int i = 0; i < minSpriteCount; ++i)
	{
		object->spriteIDs[i] = oldSpriteIDs[i];
	}
	for (unsigned int i = minSpriteCount; i < object->spriteCount; ++i)
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

void MainWindow::OnAdvancedAttributesDialog(wxCommandEvent & event)
{
	if (currentCategory == CategoryItem || currentCategory == CategoryCreature)
	{
		if (selectedObject)
		{
			if (isPreviewAnimationOn) stopAnimationPreview();
			unsigned int id = selectedObject->id;
			auto & aam = AdvancedAttributesManager::getInstance();
			savedAttrsByCatAndId[currentCategory][id] = aam.getAttributes(currentCategory, id);
			AdvancedAttributesDialog dialog(this, currentCategory, id);
			dialog.ShowModal();
		}
		else
		{
			wxMessageBox("No object selected to edit", Config::ERROR_TITLE, wxOK | wxICON_ERROR);
		}
	}
	else
	{
		wxMessageBox("Advanced attributes can only be edited for objects in \"Items\" and \"Creatures\" categories",
		             Config::ERROR_TITLE, wxOK | wxICON_ERROR);
	}
}

void MainWindow::OnAdvancedAttributesChanged(wxCommandEvent & event)
{
	isDirty = true;
	const wchar_t * msg = wxT("Advanced attributes of the object <%i> have been modified");

	unsigned int id = selectedObject->id;
	auto & aam = AdvancedAttributesManager::getInstance();
	changedAttrsByCatAndId[currentCategory][id] = aam.getAttributes(currentCategory, id);
	// putting currentCategory instead of newIntValue
	addOperationInfo(ADVANCED_ATTRS_CHANGE, id, currentCategory);

	isDirty = true;

	statusBar->SetStatusText(wxString::Format(msg, selectedObject->id));
}

void MainWindow::OnGenerateRMEDialog(wxCommandEvent & event)
{
	if (isPreviewAnimationOn) stopAnimationPreview();
	GenerateRMEDialog generateRMEDialog(this);
	generateRMEDialog.ShowModal();
}

void MainWindow::OnRMEResourcesGenerated(wxCommandEvent & event)
{
	statusBar->SetStatusText("RME resources have been generated successfully");
}

void MainWindow::OnSpritesCleanupDialog(wxCommandEvent & event)
{
	if (isPreviewAnimationOn) stopAnimationPreview();
	if (DatSprReaderWriter::getInstance().getSprites()->size() > 0)
	{
		SpritesCleanupDialog spritesCleanupDialog(this);
		spritesCleanupDialog.ShowModal();
	}
	else
	{
		wxMessageBox("There are no sprites to clean-up", Config::ERROR_TITLE, wxOK | wxICON_ERROR);
	}
}

void MainWindow::OnSpritesCleanedUp(wxCommandEvent & event)
{
	fillObjectSprites();
	fillAnimationSprites();
	isDirty = true;
	statusBar->SetStatusText("Cleaned-up some sprites. Don't forget to save.");
}

void MainWindow::OnPreferencesSaved(wxCommandEvent & event)
{
	Settings & settings = Settings::getInstance();
	const wxString & autoBackupEnabledValue = settings.get("autoBackupEnabled");
	const wxString & autoBackupIntervalValue = settings.get("autoBackupInterval");
	if (autoBackupEnabledValue.IsSameAs("yes"))
	{
		autoBackupTimer->Stop();
		autoBackupTimer->Start(wxAtoi(autoBackupIntervalValue) * 60000);
	}

	statusBar->SetStatusText("Preferences saved");
}

void MainWindow::OnQuickGuide(wxCommandEvent & event)
{
	if (isPreviewAnimationOn) stopAnimationPreview();
	QuickGuideDialog quickGuideDialog(this);
	quickGuideDialog.ShowModal();
}

void MainWindow::OnAbout(wxCommandEvent & event)
{
	if (isPreviewAnimationOn) stopAnimationPreview();
	AboutDialog aboutDialog(this);
	aboutDialog.ShowModal();
}

void MainWindow::OnExportSpriteMenu(wxCommandEvent & event)
{
	Settings & settings = Settings::getInstance();
	const wxString & browseDir = settings.get("spriteExportDir");
	wxFileDialog browseDialog(this, "Export sprite", browseDir, "sprite.png", "*.png|*.png|All files|*", wxFD_SAVE | wxFD_OVERWRITE_PROMPT);
	if (browseDialog.ShowModal() == wxID_OK)
	{
		const wxString & path = browseDialog.GetPath();
		wxFileName filename(path);
		settings.set("spriteExportDir", filename.GetPath());
		settings.save();

		auto sprites = DatSprReaderWriter::getInstance().getSprites();
		unsigned int spriteID = selectedObject->spriteIDs[getSpriteIdIndexOfAnimGridBitmap(contextMenuTargetBitmap)];
		if (spriteID && sprites->find(spriteID) != sprites->end())
		{
			auto sprite = sprites->at(spriteID);
			wxImage image(32, 32, sprite->rgb.get(), sprite->alpha.get(), true);
			if (image.SaveFile(path, wxBITMAP_TYPE_PNG))
			{
				statusBar->SetStatusText("Sprite successfully exported");
			}
			else
			{
				wxMessageBox("Sprite export failed, error occured", Config::ERROR_TITLE, wxOK | wxICON_ERROR);
			}
		}
	}
}

void MainWindow::OnExportComposedFrameMenu(wxCommandEvent & event)
{
	Settings & settings = Settings::getInstance();
	const wxString & browseDir = settings.get("spriteExportDir");
	wxFileDialog browseDialog(this, "Export sprite", browseDir, "sprite.png", "*.png|*.png|All files|*", wxFD_SAVE | wxFD_OVERWRITE_PROMPT);
	if (browseDialog.ShowModal() == wxID_OK)
	{
		const wxString & path = browseDialog.GetPath();
		wxFileName filename(path);
		settings.set("spriteExportDir", filename.GetPath());
		settings.save();

		int frameSize = selectedObject->width * selectedObject->height;
		// these buffers will be used for manual blending
		unsigned char ** bitmapsRGB = new unsigned char * [frameSize];
		unsigned char ** bitmapsAlpha = new unsigned char * [frameSize];

		// these are buffers for composed frame
		int rgbLen = Sprite::RGB_SIZE * frameSize, alphaLen = Sprite::ALPHA_SIZE * frameSize;
		unsigned char * resultRGB = new unsigned char[rgbLen];
		unsigned char * resultAlpha = new unsigned char[alphaLen];
		memset(resultRGB, 0, rgbLen);
		memset(resultAlpha, 0, alphaLen);

		bool ddbmValue = doDrawBlockingMarks;
		doDrawBlockingMarks = false; // turning of blocking marks, because we don't need them in resulting image
		fillBitmapBuffers(bitmapsRGB, bitmapsAlpha);
		doDrawBlockingMarks = ddbmValue;

		// now copying processed (and probably blended) sprite buffers into composed frame buffer
		int destWidth = selectedObject->width * Config::SPRITE_SIZE;
		int destHeight = selectedObject->height * Config::SPRITE_SIZE;
		int x = 0, y = 0;
		for (int i = 0; i < frameSize; ++i)
		{
			blendBitmapBuffers(resultRGB, resultAlpha, bitmapsRGB[i], bitmapsAlpha[i], destWidth, destHeight,
			                   Config::SPRITE_SIZE, Config::SPRITE_SIZE, x, y);
			x += Config::SPRITE_SIZE;
			if (x >= destWidth)
			{
				x = 0;
				y += Config::SPRITE_SIZE;
			}
			delete [] bitmapsRGB[i];
			delete [] bitmapsAlpha[i];
		}
		delete [] bitmapsRGB;
		delete [] bitmapsAlpha;

		wxImage composed(destWidth, destHeight, resultRGB, resultAlpha, true);
		if (composed.SaveFile(path, wxBITMAP_TYPE_PNG))
		{
			statusBar->SetStatusText("Composed frame successfully exported");
		}
		else
		{
			wxMessageBox("Composed frame export failed, error occured", Config::ERROR_TITLE, wxOK | wxICON_ERROR);
		}
		delete [] resultRGB;
		delete [] resultAlpha;
	}
}

void MainWindow::OnDeleteSpriteMenu(wxCommandEvent & event)
{
	contextMenuTargetBitmap->SetBitmap(emptyBitmap);
	unsigned int spriteIndex = 0, oldSpriteID = 0;
	if (contextMenuTargetBitmap->GetId() == ID_ANIM_GRID_BITMAP)
	{
		spriteIndex = getSpriteIdIndexOfAnimGridBitmap(contextMenuTargetBitmap);
		oldSpriteID = selectedObject->spriteIDs[spriteIndex];
		selectedObject->spriteIDs[spriteIndex] = 0;

	}
	else if (contextMenuTargetBitmap->GetId() == ID_OBJ_SPRITES_GSB)
	{
		auto esi = reinterpret_cast <EditorSpriteIDs *> (contextMenuTargetBitmap->GetClientData());
		spriteIndex = esi->getObjectSpriteIndex();
		oldSpriteID = selectedObject->spriteIDs[spriteIndex];
		selectedObject->spriteIDs[spriteIndex] = 0;
		fillAnimationSprites();
	}

	fillObjectSprites();

	auto & opInfo = addOperationInfo(ANIM_SPRITE_CHANGE, oldSpriteID, 0);
	opInfo.categoryID = currentCategory;
	opInfo.objectID = selectedObject->id;
	opInfo.spriteID = spriteIndex; // saving spriteIndex as spriteID

	statusBar->SetStatusText(wxString::Format("Deleted sprite <%i> from animation", oldSpriteID));

	isDirty = true;
}

void MainWindow::OnToggleSpriteBlockingMenu(wxCommandEvent & event)
{
	auto sprites = DatSprReaderWriter::getInstance().getSprites();
	unsigned int spriteIdIndex = getSpriteIdIndexOfAnimGridBitmap(contextMenuTargetBitmap);
	unsigned int spriteID = selectedObject->spriteIDs[spriteIdIndex];
	if (spriteID && sprites->find(spriteID) != sprites->end())
	{
		auto sprite = sprites->at(spriteID);
		bool newBlockingValue = !sprite->isBlocking;
		if (selectedObject->layersCount > 1)
		{
			// if we have several layers, we need to go through them and update each sprite 'blocking' state
			// which are on the same position in the grid
			unsigned int curLayer = currentLayer;
			bool first = true;
			for (currentLayer = 0; currentLayer < selectedObject->layersCount; ++currentLayer)
			{
				spriteIdIndex = getSpriteIdIndexOfAnimGridBitmap(contextMenuTargetBitmap);
				spriteID = selectedObject->spriteIDs[spriteIdIndex];
				if (spriteID && sprites->find(spriteID) != sprites->end())
				{
					sprite = sprites->at(spriteID);

					auto & opInfo = addOperationInfo(SPRITE_BLOCKING_CHANGE, sprite->isBlocking, newBlockingValue, !first);
					opInfo.categoryID = currentCategory;
					opInfo.objectID = selectedObject->id;
					opInfo.spriteID = spriteID;

					sprite->isBlocking = newBlockingValue;

					first = false;
				}
			}
			currentLayer = curLayer;
		}
		else
		{
			auto & opInfo = addOperationInfo(SPRITE_BLOCKING_CHANGE, sprite->isBlocking, newBlockingValue);
			opInfo.categoryID = currentCategory;
			opInfo.objectID = selectedObject->id;
			opInfo.spriteID = spriteID;
			sprite->isBlocking = newBlockingValue;
		}
	}

	fillAnimationSprites();

	statusBar->SetStatusText(wxString::Format("Changed 'blocking' state of sprite <%i>", spriteID));

	isDirty = true;
}

void MainWindow::OnToggleSpriteBlockingAllFramesMenu(wxCommandEvent & event)
{
	// TODO
	auto sprites = DatSprReaderWriter::getInstance().getSprites();
}

unsigned int MainWindow::getSpriteIdIndexOfAnimGridBitmap(wxGenericStaticBitmap * animGridBitmap)
{
	AnimationSpriteDropTarget * dropTarget = dynamic_cast <AnimationSpriteDropTarget *> (
			animGridBitmap->GetDropTarget()
	);
	unsigned int spriteHolderIndex = dropTarget->getSpriteHolderIndex();
	unsigned int frameSize = selectedObject->width * selectedObject->height;
	unsigned int patternSize = selectedObject->patternWidth * selectedObject->patternHeight;
	unsigned int xyDivIndex = currentYDiv * selectedObject->patternWidth + currentXDiv;
	unsigned int frameIndex = currentFrame * patternSize * selectedObject->layersCount;
	unsigned int spriteIdIndex = (currentLayer + xyDivIndex  + frameIndex) * frameSize
	                           + (frameSize - 1 - spriteHolderIndex);
	return spriteIdIndex;
}

MainWindow::OperationInfo & MainWindow::addOperationInfo(OperationID operationID, int oldValue, int newValue, bool chained)
{
	OperationInfo info;
	info.operationID = operationID;
	info.oldIntValue = oldValue;
	info.newIntValue = newValue;
	info.chainedUndo = chained;
	if (chained)
	{
		undoStack.top().chainedRedo = true;
	}
	undoStack.push(info);
	redoStack = stack <OperationInfo> (); // clearing redos
	menuEdit->FindChildItem(wxID_UNDO)->Enable();
	menuEdit->FindChildItem(wxID_REDO)->Enable(false);
	return undoStack.top();
}

MainWindow::OperationInfo & MainWindow::addOperationInfo(OperationID operationID, wxString oldValue, wxString newValue, bool chained)
{
	OperationInfo info;
	info.operationID = operationID;
	info.oldStrValue = oldValue;
	info.newStrValue = newValue;
	info.chainedUndo = chained;
	if (chained)
	{
		undoStack.top().chainedRedo = true;
	}
	undoStack.push(info);
	redoStack = stack <OperationInfo> (); // clearing redos
	menuEdit->FindChildItem(wxID_UNDO)->Enable();
	menuEdit->FindChildItem(wxID_REDO)->Enable(false);
	return undoStack.top();
}

void MainWindow::OnUndo(wxCommandEvent & event)
{
	OperationInfo info = undoStack.top();

	switch (info.operationID)
	{
		case ANIM_WIDTH_CHANGE:
		{
			animationWidthInput->SetValue(info.oldIntValue);
			selectedObject->width = (unsigned char) info.oldIntValue;
			resizeObjectSpriteIDsArray(selectedObject);
			buildAnimationSpriteHolders();
			fillAnimationSprites();
			statusBar->SetStatusText(wxString::Format(UNDONE_MSG, "animation width change"));
		}
		break;

		case ANIM_HEIGHT_CHANGE:
		{
			animationHeightInput->SetValue(info.oldIntValue);
			selectedObject->height = (unsigned char) info.oldIntValue;
			resizeObjectSpriteIDsArray(selectedObject);
			buildAnimationSpriteHolders();
			fillAnimationSprites();
			statusBar->SetStatusText(wxString::Format(UNDONE_MSG, "animation height change"));
		}
		break;

		case PATTERN_WIDTH_CHANGE:
		{
			patternWidthInput->SetValue(info.oldIntValue);
			selectedObject->patternWidth = (unsigned char) info.oldIntValue;
			resizeObjectSpriteIDsArray(selectedObject);
			buildAnimationSpriteHolders();
			fillAnimationSprites();
			statusBar->SetStatusText(wxString::Format(UNDONE_MSG, "pattern width change"));
		}
		break;

		case PATTERN_HEIGHT_CHANGE:
		{
			patternHeightInput->SetValue(info.oldIntValue);
			selectedObject->patternHeight = (unsigned char) info.oldIntValue;
			resizeObjectSpriteIDsArray(selectedObject);
			buildAnimationSpriteHolders();
			fillAnimationSprites();
			statusBar->SetStatusText(wxString::Format(UNDONE_MSG, "pattern height change"));
		}
		break;

		case LAYERS_COUNT_CHANGE:
		{
			layersCountInput->SetValue(info.oldIntValue);
			selectedObject->layersCount = (unsigned char) info.oldIntValue;
			resizeObjectSpriteIDsArray(selectedObject);
			buildAnimationSpriteHolders();
			fillAnimationSprites();
			statusBar->SetStatusText(wxString::Format(UNDONE_MSG, "layers count change"));
		}
		break;

		case AMOUNT_OF_FRAMES_CHANGE:
		{
			amountOfFramesInput->SetValue(info.oldIntValue);
			selectedObject->phasesCount = (unsigned char) info.oldIntValue;
			resizeObjectSpriteIDsArray(selectedObject);
			buildAnimationSpriteHolders();
			fillAnimationSprites();
			statusBar->SetStatusText(wxString::Format(UNDONE_MSG, "amount of frames change"));
		}
		break;

		case ALWAYS_ANIMATED_TOGGLE:
		{
			bool value = info.oldIntValue != 0;
			alwaysAnimatedCheckbox->SetValue(value);
			selectedObject->isAlwaysAnimated = value;
			statusBar->SetStatusText(wxString::Format(UNDONE_MSG, "\"Always animated\" attribute toggle"));
		}
		break;

		case BOOLEAN_ATTR_TOGGLE:
		{
			bool value = info.oldIntValue != 0;
			booleanAttrCheckboxes[info.controlID]->SetValue(value);
			changeBooleanAttribute(info.controlID, value);
			statusBar->SetStatusText(wxString::Format(UNDONE_MSG, wxString::Format("\"%s\" attribute toggle",
			                         booleanAttrCheckboxes[info.controlID]->GetLabel())));
		}
		break;

		case FULL_GROUND_TOGGLE:
		{
			bool value = info.oldIntValue != 0;
			valueAttrCheckboxes[info.controlID]->SetValue(value);
			groundSpeedLabel->Enable(value);
			groundSpeedInput->Enable(value);
			selectedObject->isGround = selectedObject->isFullGround = value;
			selectedObject->allAttrs[AttrGround] = selectedObject->allAttrs[AttrFullGround] = value;
			selectedObject->groundSpeed = value ? wxAtoi(groundSpeedInput->GetValue()) : 0;
			statusBar->SetStatusText(wxString::Format(UNDONE_MSG, wxString::Format("\"%s\" attribute toggle",
			                         valueAttrCheckboxes[info.controlID]->GetLabel())));
		}
		break;

		case HAS_LIGHT_TOGGLE:
		{
			bool value = info.oldIntValue != 0;
			valueAttrCheckboxes[info.controlID]->SetValue(value);
			lightColorLabel->Enable(value);
			lightColorPicker->Enable(value);
			lightIntensityLabel->Enable(value);
			lightIntensityInput->Enable(value);
			selectedObject->isLightSource = value;
			selectedObject->allAttrs[AttrLight] = value;
			const wxColour & color = lightColorPicker->GetColour();
			if (value)
			{
				selectedObject->lightColor = floor(color.Red() / 0x33) * 36 + floor(color.Green() / 0x33) * 6
																	 + floor(color.Blue() / 0x33);
				selectedObject->lightIntensity = wxAtoi(lightIntensityInput->GetValue());
			}
			else
			{
				selectedObject->lightColor = selectedObject->lightIntensity = 0;
			}
			statusBar->SetStatusText(wxString::Format(UNDONE_MSG, wxString::Format("\"%s\" attribute toggle",
			                         valueAttrCheckboxes[info.controlID]->GetLabel())));
		}
		break;

		case HAS_OFFSET_TOGGLE:
		{
			bool value = info.oldIntValue != 0;
			valueAttrCheckboxes[info.controlID]->SetValue(value);
			offsetXLabel->Enable(value);
			offsetXInput->Enable(value);
			offsetYLabel->Enable(value);
			offsetYInput->Enable(value);
			selectedObject->hasDisplacement = value;
			selectedObject->allAttrs[AttrDisplacement] = value;
			if (value)
			{
				selectedObject->displacementX = wxAtoi(offsetXInput->GetValue());
				selectedObject->displacementY = wxAtoi(offsetYInput->GetValue());
			}
			else
			{
				selectedObject->displacementX = selectedObject->displacementY = 0;
			}
			statusBar->SetStatusText(wxString::Format(UNDONE_MSG, wxString::Format("\"%s\" attribute toggle",
			                         valueAttrCheckboxes[info.controlID]->GetLabel())));
		}
		break;

		case HAS_ELEVATION_TOGGLE:
		{
			bool value = info.oldIntValue != 0;
			valueAttrCheckboxes[info.controlID]->SetValue(value);
			elevationLabel->Enable(value);
			elevationInput->Enable(value);
			selectedObject->isRaised = value;
			selectedObject->allAttrs[AttrElevation] = value;
			selectedObject->elevation = value ? wxAtoi(elevationInput->GetValue()) : 0;
			statusBar->SetStatusText(wxString::Format(UNDONE_MSG, wxString::Format("\"%s\" attribute toggle",
			                         valueAttrCheckboxes[info.controlID]->GetLabel())));
		}
		break;

		case GROUND_SPEED_CHANGE:
		{
			groundSpeedInput->ChangeValue(wxString::Format("%i", info.oldIntValue));
			selectedObject->groundSpeed = (unsigned char) info.oldIntValue;
			statusBar->SetStatusText(wxString::Format(UNDONE_MSG, "\"Ground speed\" change"));
		}
		break;

		case LIGHT_COLOR_CHANGE:
		{
			wxColour color(info.oldIntValue);
			lightColorPicker->SetColour(color);
			lastLightColorValue = color;
			selectedObject->lightColor = floor(color.Red() / 0x33) * 36 + floor(color.Green() / 0x33) * 6
			                           + floor(color.Blue() / 0x33);
			statusBar->SetStatusText(wxString::Format(UNDONE_MSG, "\"Light color\" change"));
		}
		break;

		case LIGHT_INTENSITY_CHANGE:
		{
			lightIntensityInput->ChangeValue(wxString::Format("%i", info.oldIntValue));
			selectedObject->lightIntensity = (unsigned char) info.oldIntValue;
			statusBar->SetStatusText(wxString::Format(UNDONE_MSG, "\"Light intensity\" change"));
		}
		break;

		case OFFSET_XY_CHANGE:
		{
			int oldValue = info.oldIntValue;
			if (info.controlID == ID_OFFSET_X_INPUT)
			{
				offsetXInput->ChangeValue(wxString::Format("%i", oldValue));
				selectedObject->displacementX = (unsigned char) oldValue;
				statusBar->SetStatusText(wxString::Format(UNDONE_MSG, "\"Offset X\" change"));
			}
			else if (info.controlID == ID_OFFSET_Y_INPUT)
			{
				offsetYInput->ChangeValue(wxString::Format("%i", oldValue));
				selectedObject->displacementY = (unsigned char) oldValue;
				statusBar->SetStatusText(wxString::Format(UNDONE_MSG, "\"Offset Y\" change"));
			}
		}
		break;

		case ELEVATION_CHANGE:
		{
			elevationInput->ChangeValue(wxString::Format("%i", info.oldIntValue));
			selectedObject->elevation = (unsigned char) info.oldIntValue;
			statusBar->SetStatusText(wxString::Format(UNDONE_MSG, "\"Elevation\" change"));
		}
		break;

		case OBJECT_DELETION:
		{
			if (isPreviewAnimationOn) stopAnimationPreview();

			DatObjectCategory category = (DatObjectCategory) info.newIntValue;
			auto objects = DatSprReaderWriter::getInstance().getObjects(category);
			auto & attrs = AdvancedAttributesManager::getInstance().getCategoryAttributes(category);
			unsigned int id = info.oldIntValue;
			unsigned int index = id - (category == CategoryItem ? 100 : 1);

			// remapping advanced attributes first
			unsigned int attrsID = id, endID = id + (objects->size() - index) + 1;
			for (unsigned int aid = endID; aid > attrsID; --aid)
			{
				attrs[aid] = attrs[aid - 1];
			}
			attrs[attrsID] = deletedAttrsByCatAndId[category][attrsID];

			// then inserting deleted object back
			objects->insert(objects->begin() + index, deletedObjectsByCatAndId[category][id]);
			unsigned int idx = index;
			while (idx < objects->size())
			{
				objects->at(idx++)->id = id++; // reindexing IDs
			}

			// selecting restored object if we're still in same category
			if (category == currentCategory)
			{
				selectedObject = objects->at(index);

				animationPanel->Enable();
				booleanAttrsPanel->Enable();
				valueAttrsPanel->Enable();

				fillObjectsListBox(index);
				setAttributeValues();
				fillObjectSprites();
				buildAnimationSpriteHolders();
				fillAnimationSection();
			}

			statusBar->SetStatusText(wxString::Format(UNDONE_MSG, "object deletion"));
		}
		break;

		case ADVANCED_ATTRS_CHANGE:
		{
			DatObjectCategory category = (DatObjectCategory) info.newIntValue;
			unsigned int id = info.oldIntValue;
			AdvancedAttributesManager::getInstance().setAttributes(category, id, savedAttrsByCatAndId[category][id]);
			statusBar->SetStatusText(wxString::Format(UNDONE_MSG, "advanced attributes modification"));
		}
		break;

		case SPRITE_INSERTION:
		{
			auto sprites = DatSprReaderWriter::getInstance().getSprites();
			deletedSprites[info.newIntValue] = addedSprites[info.newIntValue];
			sprites->erase(sprites->find(addedSprites[info.newIntValue]->id));

			DatObjectCategory category = (DatObjectCategory) info.categoryID;
			unsigned int id = info.objectID;
			if (category == currentCategory && selectedObject && id == selectedObject->id)
			{
				fillObjectSprites();
				fillAnimationSprites();
			}

			statusBar->SetStatusText(wxString::Format(UNDONE_MSG, "sprite insertion"));
		}
		break;

		case ANIM_SPRITE_CHANGE:
		{
			DatObjectCategory category = (DatObjectCategory) info.categoryID;
			auto objects = DatSprReaderWriter::getInstance().getObjects(category);
			unsigned int id = info.objectID;
			unsigned int index = id - (info.categoryID == CategoryItem ? 100 : 1);
			auto obj = objects->at(index);
			obj->spriteIDs[info.spriteID] = info.oldIntValue;
			if (category == currentCategory && selectedObject && id == selectedObject->id)
			{
				fillObjectSprites();
				fillAnimationSprites();
			}

			statusBar->SetStatusText(wxString::Format(UNDONE_MSG, "animation sprite change"));
		}
		break;

		case SPRITE_BLOCKING_CHANGE:
		{
			DatSprReaderWriter::getInstance().getSprites()->at(info.spriteID)->isBlocking = info.oldIntValue != 0;
			if ((unsigned int) info.objectID == selectedObject->id)
			{
				fillAnimationSprites();
			}
		}
		break;

		default:
		break;
	}

	undoStack.pop();
	redoStack.push(info);
	menuEdit->FindChildItem(wxID_REDO)->Enable();
	if (undoStack.empty())
	{
		menuEdit->FindChildItem(wxID_UNDO)->Enable(false);
	}
	else if (info.chainedUndo)
	{
		OnUndo(event);
	}
}

void MainWindow::OnRedo(wxCommandEvent & event)
{
	OperationInfo info = redoStack.top();
	
	switch (info.operationID)
	{
		case ANIM_WIDTH_CHANGE:
		{
			animationWidthInput->SetValue(info.newIntValue);
			selectedObject->width = (unsigned char) info.newIntValue;
			resizeObjectSpriteIDsArray(selectedObject);
			buildAnimationSpriteHolders();
			fillAnimationSprites();
			statusBar->SetStatusText(wxString::Format(REDONE_MSG, "animation width change"));
		}
		break;

		case ANIM_HEIGHT_CHANGE:
		{
			animationHeightInput->SetValue(info.newIntValue);
			selectedObject->height = (unsigned char) info.newIntValue;
			resizeObjectSpriteIDsArray(selectedObject);
			buildAnimationSpriteHolders();
			fillAnimationSprites();
			statusBar->SetStatusText(wxString::Format(REDONE_MSG, "animation height change"));
		}
		break;

		case PATTERN_WIDTH_CHANGE:
		{
			patternWidthInput->SetValue(info.newIntValue);
			selectedObject->patternWidth = (unsigned char) info.newIntValue;
			resizeObjectSpriteIDsArray(selectedObject);
			buildAnimationSpriteHolders();
			fillAnimationSprites();
			statusBar->SetStatusText(wxString::Format(REDONE_MSG, "pattern width change"));
		}
		break;

		case PATTERN_HEIGHT_CHANGE:
		{
			patternHeightInput->SetValue(info.newIntValue);
			selectedObject->patternHeight = (unsigned char) info.newIntValue;
			resizeObjectSpriteIDsArray(selectedObject);
			buildAnimationSpriteHolders();
			fillAnimationSprites();
			statusBar->SetStatusText(wxString::Format(REDONE_MSG, "pattern height change"));
		}
		break;

		case LAYERS_COUNT_CHANGE:
		{
			layersCountInput->SetValue(info.newIntValue);
			selectedObject->layersCount = (unsigned char) info.newIntValue;
			resizeObjectSpriteIDsArray(selectedObject);
			buildAnimationSpriteHolders();
			fillAnimationSprites();
			statusBar->SetStatusText(wxString::Format(REDONE_MSG, "layers count change"));
		}
		break;

		case AMOUNT_OF_FRAMES_CHANGE:
		{
			amountOfFramesInput->SetValue(info.newIntValue);
			selectedObject->phasesCount = (unsigned char) info.newIntValue;
			resizeObjectSpriteIDsArray(selectedObject);
			buildAnimationSpriteHolders();
			fillAnimationSprites();
			statusBar->SetStatusText(wxString::Format(REDONE_MSG, "amount of frames change"));
		}
		break;

		case ALWAYS_ANIMATED_TOGGLE:
		{
			bool value = info.newIntValue != 0;
			alwaysAnimatedCheckbox->SetValue(value);
			selectedObject->isAlwaysAnimated = value;
			statusBar->SetStatusText(wxString::Format(REDONE_MSG, "\"Always animated\" attribute toggle"));
		}
		break;

		case BOOLEAN_ATTR_TOGGLE:
		{
			bool value = info.newIntValue != 0;
			booleanAttrCheckboxes[info.controlID]->SetValue(value);
			changeBooleanAttribute(info.controlID, value);
			statusBar->SetStatusText(wxString::Format(REDONE_MSG, wxString::Format("\"%s\" attribute toggle",
			                         booleanAttrCheckboxes[info.controlID]->GetLabel())));
		}
		break;

		case FULL_GROUND_TOGGLE:
		{
			bool value = info.newIntValue != 0;
			valueAttrCheckboxes[info.controlID]->SetValue(value);
			groundSpeedLabel->Enable(value);
			groundSpeedInput->Enable(value);
			selectedObject->isGround = selectedObject->isFullGround = value;
			selectedObject->allAttrs[AttrGround] = selectedObject->allAttrs[AttrFullGround] = value;
			selectedObject->groundSpeed = value ? wxAtoi(groundSpeedInput->GetValue()) : 0;
			statusBar->SetStatusText(wxString::Format(REDONE_MSG, wxString::Format("\"%s\" attribute toggle",
			                         valueAttrCheckboxes[info.controlID]->GetLabel())));
		}
		break;

		case HAS_LIGHT_TOGGLE:
		{
			bool value = info.newIntValue != 0;
			valueAttrCheckboxes[info.controlID]->SetValue(value);
			lightColorLabel->Enable(value);
			lightColorPicker->Enable(value);
			lightIntensityLabel->Enable(value);
			lightIntensityInput->Enable(value);
			selectedObject->isLightSource = value;
			selectedObject->allAttrs[AttrLight] = value;
			const wxColour & color = lightColorPicker->GetColour();
			if (value)
			{
				selectedObject->lightColor = floor(color.Red() / 0x33) * 36 + floor(color.Green() / 0x33) * 6
																	 + floor(color.Blue() / 0x33);
				selectedObject->lightIntensity = wxAtoi(lightIntensityInput->GetValue());
			}
			else
			{
				selectedObject->lightColor = selectedObject->lightIntensity = 0;
			}
			statusBar->SetStatusText(wxString::Format(REDONE_MSG, wxString::Format("\"%s\" attribute toggle",
			                         valueAttrCheckboxes[info.controlID]->GetLabel())));
		}
		break;

		case HAS_OFFSET_TOGGLE:
		{
			bool value = info.newIntValue != 0;
			valueAttrCheckboxes[info.controlID]->SetValue(value);
			offsetXLabel->Enable(value);
			offsetXInput->Enable(value);
			offsetYLabel->Enable(value);
			offsetYInput->Enable(value);
			selectedObject->hasDisplacement = value;
			selectedObject->allAttrs[AttrDisplacement] = value;
			if (value)
			{
				selectedObject->displacementX = wxAtoi(offsetXInput->GetValue());
				selectedObject->displacementY = wxAtoi(offsetYInput->GetValue());
			}
			else
			{
				selectedObject->displacementX = selectedObject->displacementY = 0;
			}
			statusBar->SetStatusText(wxString::Format(REDONE_MSG, wxString::Format("\"%s\" attribute toggle",
			                         valueAttrCheckboxes[info.controlID]->GetLabel())));
		}
		break;

		case HAS_ELEVATION_TOGGLE:
		{
			bool value = info.newIntValue != 0;
			valueAttrCheckboxes[info.controlID]->SetValue(value);
			elevationLabel->Enable(value);
			elevationInput->Enable(value);
			selectedObject->isRaised = value;
			selectedObject->allAttrs[AttrElevation] = value;
			selectedObject->elevation = value ? wxAtoi(elevationInput->GetValue()) : 0;
			statusBar->SetStatusText(wxString::Format(REDONE_MSG, wxString::Format("\"%s\" attribute toggle",
			                         valueAttrCheckboxes[info.controlID]->GetLabel())));
		}
		break;

		case GROUND_SPEED_CHANGE:
		{
			groundSpeedInput->ChangeValue(wxString::Format("%i", info.newIntValue));
			selectedObject->groundSpeed = (unsigned char) info.newIntValue;
			statusBar->SetStatusText(wxString::Format(REDONE_MSG, "\"Ground speed\" change"));
		}
		break;

		case LIGHT_COLOR_CHANGE:
		{
			wxColour color(info.newIntValue);
			lightColorPicker->SetColour(color);
			lastLightColorValue = color;
			selectedObject->lightColor = floor(color.Red() / 0x33) * 36 + floor(color.Green() / 0x33) * 6
			                           + floor(color.Blue() / 0x33);
			statusBar->SetStatusText(wxString::Format(REDONE_MSG, "\"Light color\" change"));
		}
		break;

		case LIGHT_INTENSITY_CHANGE:
		{
			lightIntensityInput->ChangeValue(wxString::Format("%i", info.newIntValue));
			selectedObject->lightIntensity = (unsigned char) info.newIntValue;
			statusBar->SetStatusText(wxString::Format(REDONE_MSG, "\"Light intensity\" change"));
		}
		break;

		case OFFSET_XY_CHANGE:
		{
			int newValue = info.newIntValue;
			if (info.controlID == ID_OFFSET_X_INPUT)
			{
				offsetXInput->ChangeValue(wxString::Format("%i", newValue));
				selectedObject->displacementX = (unsigned char) newValue;
				statusBar->SetStatusText(wxString::Format(REDONE_MSG, "\"Offset X\" change"));
			}
			else if (info.controlID == ID_OFFSET_Y_INPUT)
			{
				offsetYInput->ChangeValue(wxString::Format("%i", newValue));
				selectedObject->displacementY = (unsigned char) newValue;
				statusBar->SetStatusText(wxString::Format(REDONE_MSG, "\"Offset Y\" change"));
			}
		}
		break;

		case ELEVATION_CHANGE:
		{
			elevationInput->ChangeValue(wxString::Format("%i", info.newIntValue));
			selectedObject->elevation = (unsigned char) info.newIntValue;
			statusBar->SetStatusText(wxString::Format(REDONE_MSG, "\"Elevation\" change"));
		}
		break;

		case OBJECT_DELETION:
		{
			DatObjectCategory category = (DatObjectCategory) info.newIntValue;
			unsigned int id = info.oldIntValue;
			processObjectDeletion(category, id);
			statusBar->SetStatusText(wxString::Format(REDONE_MSG, "object deletion"));
		}
		break;

		case ADVANCED_ATTRS_CHANGE:
		{
			DatObjectCategory category = (DatObjectCategory) info.newIntValue;
			unsigned int id = info.oldIntValue;
			AdvancedAttributesManager::getInstance().setAttributes(category, id, changedAttrsByCatAndId[category][id]);
			statusBar->SetStatusText(wxString::Format(REDONE_MSG, "advanced attributes modification"));
		}
		break;

		case SPRITE_INSERTION:
		{
			auto sprites = DatSprReaderWriter::getInstance().getSprites();
			(*sprites)[info.newIntValue] = deletedSprites[info.newIntValue];

			DatObjectCategory category = (DatObjectCategory) info.categoryID;
			unsigned int id = info.objectID;
			if (category == currentCategory && selectedObject && id == selectedObject->id)
			{
				fillObjectSprites();
				fillAnimationSprites();
			}

			statusBar->SetStatusText(wxString::Format(REDONE_MSG, "sprite insertion"));
		}
		break;

		case ANIM_SPRITE_CHANGE:
		{
			DatObjectCategory category = (DatObjectCategory) info.categoryID;
			auto objects = DatSprReaderWriter::getInstance().getObjects(category);
			unsigned int id = info.objectID;
			unsigned int index = id - (info.categoryID == CategoryItem ? 100 : 1);
			auto obj = objects->at(index);
			obj->spriteIDs[info.spriteID] = info.newIntValue;
			if (category == currentCategory && selectedObject && id == selectedObject->id)
			{
				fillObjectSprites();
				fillAnimationSprites();
			}

			statusBar->SetStatusText(wxString::Format(REDONE_MSG, "animation sprite change"));
		}
		break;

		case SPRITE_BLOCKING_CHANGE:
		{
			DatSprReaderWriter::getInstance().getSprites()->at(info.spriteID)->isBlocking = info.newIntValue != 0;
			if ((unsigned int) info.objectID == selectedObject->id)
			{
				fillAnimationSprites();
			}
		}
		break;

		default:
		break;
	}

	redoStack.pop();
	undoStack.push(info);
	menuEdit->FindChildItem(wxID_UNDO)->Enable();
	if (redoStack.empty())
	{
		menuEdit->FindChildItem(wxID_REDO)->Enable(false);
	}
	else if (info.chainedRedo)
	{
		OnRedo(event);
	}
}

void MainWindow::OnPreferencesDialog(wxCommandEvent & event)
{
	if (isPreviewAnimationOn) stopAnimationPreview();
	PreferencesDialog preferencesDialog(this);
	preferencesDialog.ShowModal();
}

void MainWindow::OnAutobackupProcessed(wxCommandEvent & event)
{
	statusBar->SetStatusText("Performed auto-backup");
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
	
	if (mainWindow->isPreviewAnimationOn) return result;
	
	unsigned int esiIndexInVector = wxAtoi(data);
	auto esi = mainWindow->editorSpriteIDs[esiIndexInVector];
	auto obj = mainWindow->selectedObject;
	unsigned int frameSize = obj->width * obj->height;
	unsigned int patternSize = obj->patternWidth * obj->patternHeight;
	int xyDivIndex = mainWindow->currentYDiv * obj->patternWidth + mainWindow->currentXDiv;
	// ^ it's like 2d grid of sprites, it also can have layers
	int frameIndex = mainWindow->currentFrame * patternSize * obj->layersCount;
	// ^ frames iterated over whole patterns + layers
	unsigned int spriteIdStartIndex = (xyDivIndex + mainWindow->currentLayer + frameIndex) * frameSize
	                                + (frameSize - 1 - spriteHolderIndex);

	if (esi->getType() == EditorSpriteIDs::IMPORTED) // imported sprite
	{
		auto sprites = DatSprReaderWriter::getInstance().getSprites();
		auto & esiSpriteIDs = esi->getSpriteIDs();
		auto image = mainWindow->importedSprites[esiSpriteIDs[0]];
		
		void * pdp = nullptr; // pixel data pointer
		bool hasAlpha = image->HasAlpha();
		if (hasAlpha)
		{
			wxBitmap bmp(*image);
			pdp = new wxAlphaPixelData(bmp);
		}
		else
		{
			wxBitmap bmp(*image);
			pdp = new wxNativePixelData(bmp);
		}

		int width = ceil(image->GetWidth() / (float) Config::SPRITE_SIZE);
		int height = ceil(image->GetHeight() / (float) Config::SPRITE_SIZE);
		esi->setType(MainWindow::EditorSpriteIDs::EXISTING);
		esi->setBigSpriteSize(width, height);
		esiSpriteIDs.clear();
		bool first = true;
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
					unsigned int oldSpriteID = obj->spriteIDs[spriteIndex];
					obj->spriteIDs[spriteIndex] = sprite->id;
					auto & opInfo = mainWindow->addOperationInfo(ANIM_SPRITE_CHANGE, oldSpriteID, sprite->id, !first);
					opInfo.categoryID = mainWindow->currentCategory;
					opInfo.objectID = obj->id;
					opInfo.spriteID = spriteIndex; // saving spriteIndex as spriteID
				}
				mainWindow->addedSprites[sprite->id] = sprite;
				auto & opInfo = mainWindow->addOperationInfo(SPRITE_INSERTION, 0, sprite->id, true);
				opInfo.categoryID = mainWindow->currentCategory;
				opInfo.objectID = obj->id;

				esiSpriteIDs.push_back(sprite->id);

				first = false;
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
		bool first = true;
		for (int h = 0; h < height; ++h)
		{
			for (int w = 0; w < width; ++w)
			{
				unsigned int spriteIndex = spriteIdStartIndex - h * obj->width - w;
				if (spriteIndex >= 0 && spriteIndex < obj->spriteCount)
				{
					unsigned int oldSpriteID = obj->spriteIDs[spriteIndex];
					obj->spriteIDs[spriteIndex] = esiSpriteIDs[spriteID];

					auto & opInfo = mainWindow->addOperationInfo(ANIM_SPRITE_CHANGE, oldSpriteID, spriteID, !first);
					opInfo.categoryID = mainWindow->currentCategory;
					opInfo.objectID = obj->id;
					opInfo.spriteID = spriteIndex; // saving spriteIndex as spriteID

					first = false;
					spriteID++;
				}
			}
		}
		mainWindow->fillObjectSprites();
		mainWindow->fillAnimationSprites();

		result = mainWindow->isDirty = true;
	}

	// resetting cached bitmap used for selection handling
	auto gridCell = mainWindow->animationGridBitmaps[spriteHolderIndex];
	gridCell->SetClientData(nullptr);

	return result;
}

wxDragResult MainWindow::AnimationSpriteDropTarget::OnEnter(wxCoord x, wxCoord y, wxDragResult defResult)
{
	if (mainWindow->isPreviewAnimationOn) return defResult;
	auto animGridBitmap = mainWindow->animationGridBitmaps[spriteHolderIndex];
	mainWindow->drawAnimationSpriteSelection(animGridBitmap);
	return defResult;
}

void MainWindow::AnimationSpriteDropTarget::OnLeave()
{
	if (mainWindow->isPreviewAnimationOn) return;
	auto animGridBitmap = mainWindow->animationGridBitmaps[spriteHolderIndex];
	mainWindow->clearAnimationSpriteSelection(animGridBitmap);
}
