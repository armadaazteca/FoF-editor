#ifndef _MAIN_WINDOW_H_
#define _MAIN_WINDOW_H_

#include <memory>
#include <vector>
#include <wx/generic/statbmpg.h>
#include <wx/dnd.h>
#include "DatSprStructs.h"

using namespace std;

class MainWindow : public wxFrame
{
public:
	MainWindow(const wxString & title, const wxPoint & pos, const wxSize & size);

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

	inline std::vector <shared_ptr <wxBitmap>> getImportedSprites() { return importedSprites; }
	inline wxGenericStaticBitmap ** getAnimationSpriteBitmaps() { return animationSpriteBitmaps; }

private:
	static const wxString CATEGORIES[];
	enum ControlIds
	{
		ID_CATEGORIES_COMBOBOX = 1,
		ID_OBJECTS_LISTBOX,
		ID_DIR_LEFT_BUTTON,
		ID_DIR_TOP_BUTTON,
		ID_DIR_RIGHT_BUTTON,
		ID_DIR_BOTTOM_BUTTON,
		ID_PREV_FRAME_BUTTON,
		ID_NEXT_FRAME_BUTTON,
		ID_ANIM_WIDTH_INPUT,
		ID_ANIM_HEIGHT_INPUT,
		ID_FRAMES_AMOUNT_INPUT,
		ID_ALWAYS_ANIMATED_CHECKBOX,
		ID_NEW_OBJECT_BUTTON,
		ID_IMPORT_SPRITES_BUTTON,
		ID_IMPORTED_SPRITE
	};
	enum AttrCheckboxIds
	{
		ID_ATTR_IS_GROUND_BORDER = 100, ID_ATTR_IS_ON_BOTTOM, ID_ATTR_IS_ON_TOP, ID_ATTR_IS_CONTAINER,
		ID_ATTR_IS_STACKABLE, ID_ATTR_IS_FORCE_USE, ID_ATTR_IS_MULTI_USE,	ID_ATTR_IS_FLUID_CONTAINER,
		ID_ATTR_IS_SPLASH, ID_ATTR_BLOCKS_PROJECTILES, ID_ATTR_IS_PICKUPABLE, ID_ATTR_IS_WALKABLE,
		ID_ATTR_IS_MOVABLE, ID_ATTR_IS_PATHABLE, ID_ATTR_CAN_BE_HIDDEN,	ID_ATTR_IS_HANGABLE,
		ID_ATTR_IS_HOOK_SOUTH, ID_ATTR_IS_HOOK_EAST, ID_ATTR_IS_ROTATABLE, ID_ATTR_IS_TRANSLUCENT,
		ID_ATTR_IS_LYING_CORPSE, ID_ATTR_IS_FULL_GROUND, ID_ATTR_IGNORE_LOOK,	ID_ATTR_IS_USABLE,
		ID_ATTR_LAST
		// TODO: ID_ATTR_IS_GROUND, ID_ATTR_IS_WRITABLE, ID_ATTR_IS_WRITABLE_ONCE, isAlwaysAnimated, isLightSource,
		// hasDisplacement, isRaised, isMinimap, isLensHelp, isCloth, isMarketable
	};
	enum OrientationToXDiv
	{
		ORIENT_NORTH = 0,
		ORIENT_EAST = 1,
		ORIENT_SOUTH = 2,
		ORIENT_WEST = 3
	};

	shared_ptr <DatObject> selectedObject = nullptr;

	wxPanel * mainPanel = nullptr;
	wxComboBox * categoryComboBox = nullptr;
	wxListBox * objectsListBox = nullptr;
	wxCheckBox * attrCheckboxes[ID_ATTR_LAST];
	DatObjectCategory currentCategory = CategoryItem;

	wxScrolledWindow * objectSpritesPanel = nullptr, * newSpritesPanel = nullptr;
	wxFlexGridSizer * objectSpritesPanelSizer = nullptr, * newSpritesPanelSizer = nullptr;

	wxPanel * animationPanel = nullptr, * animationSpritesPanel = nullptr;
	wxFlexGridSizer * animationBoxExpandSizer = nullptr, * animationMainGridSizer = nullptr;
	wxBoxSizer * animationPanelSizer = nullptr;
	wxGridSizer * animationSpritesSizer = nullptr;
	wxGenericStaticBitmap * animationSpriteBitmaps[64];
	wxStaticText * currentFrameNumber = nullptr;
	wxTextCtrl * animationWidthInput = nullptr, * animationHeightInput = nullptr;
	wxTextCtrl * amountOfFramesInput = nullptr;
	wxCheckBox * alwaysAnimatedCheckbox = nullptr;
	unsigned int currentFrame = 0, currentXDiv = 0, currentYDiv = 0;
	wxImage * stubImage = nullptr;
	wxBitmap * spritePlaceholderBitmap = nullptr;

	std::vector <shared_ptr <wxBitmap>> importedSprites;

	void OnOpenDatSprDialog(wxCommandEvent & event);
	void OnDatSprLoaded(wxCommandEvent & event);
	void OnObjectCategoryChanged(wxCommandEvent & event);
	void OnObjectSelected(wxCommandEvent & event);
	void OnAnimWidthChanged(wxCommandEvent & event);
	void OnAnimHeightChanged(wxCommandEvent & event);
	void OnClickOrientationButton(wxCommandEvent & event);
	void OnClickPrevFrameButton(wxCommandEvent & event);
	void OnClickNextFrameButton(wxCommandEvent & event);
	void OnClickNewObjectButton(wxCommandEvent & event);
	void OnClickImportSpriteButton(wxCommandEvent & event);
	void OnClickImportedSprite(wxMouseEvent & event);
	void OnExit(wxCommandEvent & event);
	void OnAbout(wxCommandEvent & event);
	void fillObjectsListBox();
	void setAttributeValues();
	void fillObjectSprites();
	void fillAnimationSection();
	void fillAnimationSprites();
	void buildAnimationSpriteHolders();
	void drawAnimationSpriteSelection(wxGenericStaticBitmap * staticBitmap);
	void clearAnimationSpriteSelection(wxGenericStaticBitmap * staticBitmap);

	wxDECLARE_EVENT_TABLE();
};

#endif // _MAIN_WINDOW_H_
