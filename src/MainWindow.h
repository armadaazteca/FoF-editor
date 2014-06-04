#ifndef _MAIN_WINDOW_H_
#define _MAIN_WINDOW_H_

#include <memory>
#include "DatSprStructs.h"

using namespace std;

class MainWindow : public wxFrame
{
public:
	MainWindow(const wxString & title, const wxPoint & pos, const wxSize & size);

private:
	static const wxString CATEGORIES[];
	enum ControlIds
	{
		ID_CATEGORIES_COMBOBOX = 1,
		ID_OBJECTS_LISTBOX
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

	wxComboBox * categoryComboBox = nullptr;
	wxListBox * objectsListBox = nullptr;
	wxCheckBox * attrCheckboxes[ID_ATTR_LAST];
	DatObjectCategory currentCategory = CategoryItem;

	wxScrolledWindow * spritesPanel = nullptr;
	wxFlexGridSizer * spritesPanelSizer = nullptr;

	void OnOpenDatSprDialog(wxCommandEvent & event);
	void OnDatSprLoaded(wxCommandEvent & event);
	void OnObjectCategoryChanged(wxCommandEvent & event);
	void OnObjectSelected(wxCommandEvent & event);
	void OnExit(wxCommandEvent & event);
	void OnAbout(wxCommandEvent & event);
	void fillObjectsListBox();
	void setAttributeValues(shared_ptr <DatObject> object);
	void fillObjectSprites(shared_ptr <DatObject> object);

	wxDECLARE_EVENT_TABLE();
};

#endif // _MAIN_WINDOW_H_
