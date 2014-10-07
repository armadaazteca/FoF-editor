#include <wx/wxprec.h>
#ifndef WX_PRECOMP
#include <wx/wx.h>
#endif
#include "QuickGuideDialog.h"
#include "Config.h"

QuickGuideDialog::QuickGuideDialog(wxWindow * parent) : wxDialog(parent, wxID_ANY, "Quick guide", wxDefaultPosition, wxSize(640, 480))
{
	auto vbox = new wxBoxSizer(wxVERTICAL);

  const char * guideText = "To create new files from scratch use \"File -> New\" menu or Ctrl+N hotkey.\n\n"
	                         "To open existing files use \"File -> New\" menu or Ctrl+O hotkey.\n\n"
	                         "To save files use \"File -> Save\" menu or Ctrl+S hotkey.\n\n"
                           "For opening and saving you will need to provide paths to files. If at least one of the sprites "
                           "currently being edited contains alpha-transparency, you must check corresponding "
                           "checkbox in the save dialog and provide path to .alp file.\n\n"
	                         "After new files have been created or existing have been loaded, "
                           "you can switch objects category using dropdown in the top-left corner. "
	                         "Then you can select existing object in the listbox under the dropdown.\n\n"
	                         "To create new object in the current category press \"New object\" button "
                           "in the bottom-left corner.\n\n"
                           "If existing object is selected and it has some sprites, they will be displayed on the right side in the "
                           "\"Object sprites\" tab.\n\n"
	                         "To import new sprites in the editor, select \"New sprites\" tab next to \"Object sprites\", then "
                           "press \"Import sprites\" button at the bottom right corner. You'll be able to select one or multiple "
                           "image files which must be in PNG format.\n\n"
                           "In the middle area of the editor, you can manage object sprites layout and animations.\n\n"
                           "Main thing here is the sprites grid, surrounded by the orientation buttons. By default, new object will have "
                           "only one grid cell. Amount of cells can be changed by modifying \"Width\" and \"Height\" parameters.\n\n"
                           "You can drag & drop sprites from \"Object sprites\" / \"New sprites\" tabs to these grid cells. "
                           "Imported sprites, that are larger than 32x32 pixels, will be automatically layout into current grid.\n\n"
                           "By right-clicking sprite cell you will erase it, thus that partucular sprite will be disassociated with "
                           "curret object, but that sprite will still persist in .spr file.\n\n"
                           "By pressing green arrow buttons you can switch orientation of animation, mostly used for character "
                           "animations.\n\n"
                           "\"Amounf of frames\" field is used to set animation frames number. Current frame can be switched using "
                           "\"< previous\" and \"next >\" buttons, and current frame index is displayed in the corresponding field.\n\n"
                           "\"Always animated\" checkbox is the attribute, used to indicate that object animation must be always "
                           "played in the game.\n\n"
                           "Below the animation area is the attributes area. It is separated into \"Boolean attributes\" and "
                           "\"Value attributes\". Boolean attributes are simply values of \"true / false\" and value attributes, "
                           "though also have boolean toggle, have other (mostly numeric) values as well. When attribute checkbox "
                           "is not checked, its corresponding value fields are disabled.";
	auto text = new wxTextCtrl(this, wxID_ANY, guideText, wxDefaultPosition, wxDefaultSize,
	                           wxTE_MULTILINE | wxTE_READONLY | wxNO_BORDER);
	text->SetBackgroundColour(GetDefaultAttributes().colBg);
	vbox->Add(text, 1, wxALL | wxEXPAND, 10);

	SetSizer(vbox);
	Center();
}
