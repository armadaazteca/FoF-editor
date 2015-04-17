#ifndef _ADVANCED_ATTRIBUTES_DIALOG_H_
#define _ADVANCED_ATTRIBUTES_DIALOG_H_

#include "Interfaces.h"
#include "DatSprStructs.h"

class AdvancedAttributesDialog : public wxDialog
{
public:
	enum ControlIDs { ID_BUTTON_ERASE };

	static const wxString GROUPS[], FLOOR_CHANGE[];

	AdvancedAttributesDialog(wxWindow * parent, DatObjectCategory objectCategory, unsigned int objectID);
	inline virtual ~AdvancedAttributesDialog() {}

private:
	void OnClickSaveButton(wxCommandEvent & event);
	void OnClickEraseButton(wxCommandEvent & event);
	wxDECLARE_EVENT_TABLE();

	DatObjectCategory objCat = CategoryItem;
	unsigned int objID = 0;

	wxComboBox * groupCombo = nullptr, * floorChangeCombo = nullptr;
	wxTextCtrl * nameText = nullptr, * descriptionText = nullptr;
	wxRadioButton * articleARb = nullptr, * articleAnRb = nullptr;
	wxRadioButton * typeMonsterRb = nullptr, * typeNPCRb = nullptr;
};

#endif // _ADVANCED_ATTRIBUTES_DIALOG_H_
