#ifndef _MAIN_WINDOW_H_
#define _MAIN_WINDOW_H_

#include <memory>
#include <vector>
#include <stack>
#include <wx/generic/statbmpg.h>
#include <wx/dnd.h>
#include <wx/clrpicker.h>
#include <wx/spinctrl.h>
#include "DatSprStructs.h"

using namespace std;

class MainWindow : public wxFrame
{
public:
	static const wxString & DIRECTION_QUESTION, & DIRECTION_QUESTION_TITLE;

	MainWindow(const wxString & title, const wxPoint & pos, const wxSize & size);

	class EditorSpriteIDs : public wxCustomDataObject
	{
	public:
		enum Type { EXISTING = 0, IMPORTED = 1 };
		inline Type getType() { return type; }
		inline void setType(Type type) { this->type = type; }
		inline vector <unsigned int> & getSpriteIDs() { return spriteIDs; }
		inline unsigned int getObjectSpriteIndex() { return objectSpriteIndex; }
		inline void setObjectSpriteIndex(unsigned int index) { objectSpriteIndex = index; }
		inline unsigned int getIndexInVector() { return indexInVector; }
		inline void setIndexInVector(unsigned int index) { indexInVector = index; }
		inline unsigned int getBigSpriteWidth() { return bigSpriteWidth; }
		inline unsigned int getBigSpriteHeight() { return bigSpriteHeight; }
		inline void setBigSpriteSize(unsigned int width, unsigned int height)
		{
			bigSpriteWidth = width;
			bigSpriteHeight = height;
		}
	private:
		Type type = EXISTING;
		vector <unsigned int> spriteIDs;
		unsigned int objectSpriteIndex = 0;
		unsigned int indexInVector = 0;
		unsigned int bigSpriteWidth = 1, bigSpriteHeight = 1;
	};

	class AnimationSpriteDropTarget : public wxTextDropTarget
	{
	public:
		virtual bool OnDropText(wxCoord x, wxCoord y, const wxString & data);
		virtual wxDragResult OnEnter(wxCoord x, wxCoord y, wxDragResult defResult);
		virtual void OnLeave();
		inline unsigned int getSpriteHolderIndex() { return spriteHolderIndex; }
		inline void setSpriteHolderIndex(unsigned int index) { spriteHolderIndex = index; }
		inline void setMainWindow(MainWindow * mainWindow) { this->mainWindow = mainWindow; };
		inline virtual ~AnimationSpriteDropTarget() {}
	private:
		MainWindow * mainWindow = nullptr;
		unsigned int spriteHolderIndex = 0;
	};

private:
	static const wxString CATEGORIES[];

	enum ControlIds
	{
		ID_CATEGORIES_COMBOBOX = 1,
		ID_OBJECTS_LISTBOX,
		ID_DIR_LEFT_BUTTON,
		ID_DIR_TOP_LEFT_BUTTON,
		ID_DIR_TOP_BUTTON,
		ID_DIR_TOP_RIGHT_BUTTON,
		ID_DIR_RIGHT_BUTTON,
		ID_DIR_BOTTOM_RIGHT_BUTTON,
		ID_DIR_BOTTOM_BUTTON,
		ID_DIR_BOTTOM_LEFT_BUTTON,
		ID_PREV_FRAME_BUTTON,
		ID_NEXT_FRAME_BUTTON,
		ID_ANIM_WIDTH_INPUT,
		ID_ANIM_HEIGHT_INPUT,
		ID_PATTERN_WIDTH_INPUT,
		ID_PATTERN_HEIGHT_INPUT,
		ID_PREV_XDIV_BUTTON,
		ID_NEXT_XDIV_BUTTON,
		ID_PREV_YDIV_BUTTON,
		ID_NEXT_YDIV_BUTTON,
		ID_LAYERS_COUNT_INPUT,
		ID_PREV_LAYER_BUTTON,
		ID_NEXT_LAYER_BUTTON,
		ID_FRAMES_AMOUNT_INPUT,
		ID_ALWAYS_ANIMATED_CHECKBOX,
		ID_BLEND_LAYERS_CHECKBOX,
		ID_PREVIEW_ANIMATION_BUTTON,
		ID_PREVIEW_TIMER,
		ID_PREVIEW_FPS_INPUT,
		ID_NEW_OBJECT_BUTTON,
		ID_DELETE_OBJECT_BUTTON,
		ID_IMPORT_SPRITES_BUTTON,
		ID_IMPORTED_SPRITE,
		ID_OBJ_SPRITES_GSB,
		ID_GRID_GSB
	};

	enum BooleanAttrCheckboxIds
	{
		ID_ATTR_IS_CONTAINER = 100, ID_ATTR_IS_STACKABLE, ID_ATTR_IS_MULTI_USE,	ID_ATTR_IS_WALKABLE,
		ID_ATTR_IS_PATHABLE, ID_ATTR_IS_MOVABLE, ID_ATTR_BLOCKS_PROJECTILES, ID_ATTR_IS_PICKUPABLE,
		ID_ATTR_IGNORE_LOOK, ID_ATTR_IS_HANGABLE,	ID_ATTR_IS_LYING_CORPSE, ID_ATTR_HAS_MOUNT, ID_ATTR_BOOLEAN_LAST
	};

	enum ValueAttrIds
	{
		ID_ATTR_IS_FULL_GROUND = 200, ID_ATTR_HAS_LIGHT, ID_ATTR_HAS_OFFSET, ID_ATTR_HAS_ELEVATION,
		ID_GROUND_SPEED_INPUT, ID_LIGHT_COLOR_PICKER, ID_LIGHT_INTENSITY_INPUT, ID_OFFSET_X_INPUT, ID_OFFSET_Y_INPUT,
		ID_ELEVATION_INPUT
	};

	enum MenuIDs
	{
		// main menu IDs
		ID_MENU_EDIT_ADVANCED_ATTRS = 300,
		ID_MENU_GENERATE_RME,
		ID_MENU_CLEANUP_SPRITES,
		ID_MENU_QUICK_GUIDE,
		// context menu IDs
		ID_MENU_EXPORT_SPRITE,
		ID_MENU_EXPORT_COMPOSED,
		ID_MENU_DELETE_SPRITE
	};

	enum OrientationToXDiv
	{
		ORIENT_NORTH = 0,
		ORIENT_EAST = 1,
		ORIENT_SOUTH = 2,
		ORIENT_WEST = 3,
		ORIENT_NORTH_WEST = 4,
		ORIENT_NORTH_EAST = 5,
		ORIENT_SOUTH_EAST = 6,
		ORIENT_SOUTH_WEST = 7
	};

	enum OperationID
	{
		ANIM_WIDTH_CHANGE, ANIM_HEIGHT_CHANGE, PATTERN_WIDTH_CHANGE, PATTERN_HEIGHT_CHANGE,
		LAYERS_COUNT_CHANGE, AMOUNT_OF_FRAMES_CHANGE, ALWAYS_ANIMATED_TOGGLE, OPERATION_NONE
	};

	struct OperationInfo
	{
		OperationID operationID = OPERATION_NONE;
		int oldIntValue = 0, newIntValue = 0;
		wxString oldStrValue, newStrValue;
		bool chained = false; // whether this operation chained with other undos to rollback several at once
	};

	bool isDirty = false; // whether editor data has been modified

	shared_ptr <DatObject> selectedObject = nullptr;

	wxMenu * menuFile = nullptr, * menuEdit = nullptr, * menuTools = nullptr, * menuHelp = nullptr;
	wxPanel * mainPanel = nullptr;
	wxStatusBar * statusBar = nullptr;
	wxComboBox * categoryComboBox = nullptr;
	wxListBox * objectsListBox = nullptr;
	wxCheckBox * booleanAttrCheckboxes[ID_ATTR_BOOLEAN_LAST], * valueAttrCheckboxes[ID_GROUND_SPEED_INPUT];
	DatObjectCategory currentCategory = CategoryItem;

	wxScrolledWindow * objectSpritesPanel = nullptr, * newSpritesPanel = nullptr;
	wxFlexGridSizer * objectSpritesPanelSizer = nullptr, * newSpritesPanelSizer = nullptr;

	wxPanel * booleanAttrsPanel = nullptr, * valueAttrsPanel = nullptr;
	wxPanel * animationPanel = nullptr, * animationSpritesPanel = nullptr;
	wxFlexGridSizer * animationBoxExpandSizer = nullptr, * animationMainGridSizer = nullptr;
	wxBoxSizer * animationPanelSizer = nullptr;
	wxGridSizer * animationSpritesSizer = nullptr;
	wxGenericStaticBitmap * animationSpriteBitmaps[16];
	wxStaticText * currentFrameNumber = nullptr;
	wxSpinCtrl * animationWidthInput = nullptr, * animationHeightInput = nullptr;
	wxSpinCtrl * patternWidthInput = nullptr, * patternHeightInput = nullptr;
	wxStaticText * currentXDivLabel = nullptr, * currentYDivLabel = nullptr;
	wxSpinCtrl * layersCountInput = nullptr;
	wxStaticText * currentLayerNumber = nullptr;
	wxSpinCtrl * amountOfFramesInput = nullptr;
	wxCheckBox * alwaysAnimatedCheckbox = nullptr, * blendLayersCheckbox = nullptr;
	wxSpinCtrl * previewFpsInput = nullptr;
	unique_ptr <wxTimer> previewTimer = nullptr;
	wxStaticText * groundSpeedLabel = nullptr, * lightColorLabel = nullptr, * lightIntensityLabel = nullptr;
	wxStaticText * offsetXLabel = nullptr, * offsetYLabel = nullptr, * elevationLabel = nullptr;
	wxTextCtrl * groundSpeedInput = nullptr, * lightIntensityInput = nullptr;
	wxTextCtrl * offsetXInput = nullptr, * offsetYInput = nullptr, * elevationInput = nullptr;
	wxColourPickerCtrl * lightColorPicker = nullptr;
	wxGenericStaticBitmap * spriteContextMenuTarget = nullptr;
	unsigned int currentFrame = 0, lastCurrentFrame = 0, currentXDiv = 0, currentYDiv = 0, currentLayer = 0;
	bool doBlendLayers = false;
	shared_ptr <wxImage> stubImage = nullptr;
	unique_ptr <unsigned char[]> stubImageRgb = nullptr;
	unique_ptr <unsigned char[]> stubImageAlpha = nullptr;
	int amountOfFrames = 0;
	bool isPreviewAnimationOn = false;

	vector <shared_ptr <wxImage>> importedSprites;
	vector <EditorSpriteIDs *> editorSpriteIDs;
	vector <wxWindow *> controlsToDisableOnPreview;
	stack <OperationInfo> undoStack, redoStack;

	void OnCreateNewFiles(wxCommandEvent & event);
	void OnOpenDatSprDialog(wxCommandEvent & event);
	void OnDatSprLoaded(wxCommandEvent & event);
	void OnDatSprSaved(wxCommandEvent & event);
	void OnObjectCategoryChanged(wxCommandEvent & event);
	void OnObjectSelected(wxCommandEvent & event);
	void OnToggleAttrCheckbox(wxCommandEvent & event);
	void OnAnimWidthChanged(wxSpinEvent & event);
	void OnAnimHeightChanged(wxSpinEvent & event);
	void OnFramesAmountChanged(wxSpinEvent & event);
	void OnClickOrientationButton(wxCommandEvent & event);
	void OnPatternWidthChanged(wxSpinEvent & event);
	void OnPatternHeightChanged(wxSpinEvent & event);
	void OnClickPrevXDivButton(wxCommandEvent & event);
	void OnClickNextXDivButton(wxCommandEvent & event);
	void OnClickPrevYDivButton(wxCommandEvent & event);
	void OnClickNextYDivButton(wxCommandEvent & event);
	void OnLayersCountChanged(wxSpinEvent & event);
	void OnClickPrevLayerButton(wxCommandEvent & event);
	void OnClickNextLayerButton(wxCommandEvent & event);
	void OnClickPrevFrameButton(wxCommandEvent & event);
	void OnClickNextFrameButton(wxCommandEvent & event);
	void OnPreviewFPSChanged(wxSpinEvent & event);
	void OnClickPreviewAnimationButton(wxCommandEvent & event);
	void OnPreviewTimerEvent(wxTimerEvent & event);
	void OnClickNewObjectButton(wxCommandEvent & event);
	void OnClickDeleteObjectButton(wxCommandEvent & event);
	void OnClickImportSpriteButton(wxCommandEvent & event);
	void OnClickImportedOrObjectSprite(wxMouseEvent & event);
	void OnRightClickObjectSprite(wxMouseEvent & event);
	void OnToggleAlwaysAnimatedAttr(wxCommandEvent & event);
	void OnToggleBlendLayersCheckbox(wxCommandEvent & event);
	void OnToggleIsFullGroundAttr(wxCommandEvent & event);
	void OnToggleHasLightAttr(wxCommandEvent & event);
	void OnToggleHasOffsetAttr(wxCommandEvent & event);
	void OnToggleHasElevationAttr(wxCommandEvent & event);
	void OnGroundSpeedChanged(wxCommandEvent & event);
	void OnLightColorChanged(wxColourPickerEvent & event);
	void OnLightIntensityChanged(wxCommandEvent & event);
	void OnOffsetXYChanged(wxCommandEvent & event);
	void OnElevationChanged(wxCommandEvent & event);
	void OnExit(wxCommandEvent & event);
	void OnClose(wxCloseEvent & event);
	void OnUndo(wxCommandEvent & event);
	void OnRedo(wxCommandEvent & event);
	void OnAdvancedAttributesDialog(wxCommandEvent & event);
	void OnAdvancedAttributesChanged(wxCommandEvent & event);
	void OnGenerateRMEDialog(wxCommandEvent & event);
	void OnRMEResourcesGenerated(wxCommandEvent & event);
	void OnSpritesCleanupDialog(wxCommandEvent & event);
	void OnSpritesCleanedUp(wxCommandEvent & event);
	void OnQuickGuide(wxCommandEvent & event);
	void OnAbout(wxCommandEvent & event);
	void OnExportSpriteMenu(wxCommandEvent & event);
	void OnExportComposedPictureMenu(wxCommandEvent & event);
	void OnDeleteSpriteMenu(wxCommandEvent & event);
	bool checkDirty();
	void fillObjectsListBox(unsigned int selectedIndex = 0);
	void setAttributeValues(bool isNewObject = false);
	void fillObjectSprites();
	void fillAnimationSection(bool resetIterators = true);
	void fillAnimationSprites();
	void buildAnimationSpriteHolders();
	void drawAnimationSpriteSelection(wxGenericStaticBitmap * staticBitmap);
	void clearAnimationSpriteSelection(wxGenericStaticBitmap * staticBitmap);
	void resizeObjectSpriteIDsArray(shared_ptr <DatObject> object);
	void deleteSelectedObject();
	void startAnimationPreview();
	void stopAnimationPreview();
	void addOperationInfo(OperationID operationID, int oldValue, int newValue, bool chained = false);
	void addOperationInfo(OperationID operationID, wxString oldValue, wxString newValue, bool chained = false);

	wxDECLARE_EVENT_TABLE();

	~MainWindow();
};

#endif // _MAIN_WINDOW_H_
