#include <wx/wxprec.h>
#ifndef WX_PRECOMP
#include <wx/wx.h>
#endif
#include "QuickGuideDialog.h"
#include "Config.h"

QuickGuideDialog::QuickGuideDialog(wxWindow * parent) : wxDialog(parent, wxID_ANY, "Quick guide", wxDefaultPosition, wxSize(640, 480))
{
	auto vbox = new wxBoxSizer(wxVERTICAL);

  const char * guideText = "QUICK GUIDE - OVERVIEW OF EDITOR FUNCTIONALITY\n"
	                         "===============================================\n\n"
	                         "To create new files from scratch use \"File -> New\" menu or Ctrl+N hotkey.\n\n"
	                         "To open existing files use \"File -> Open\" menu or Ctrl+O hotkey.\n\n"
	                         "To save files use \"File -> Save\" menu or Ctrl+S hotkey.\n\n"
                           "For opening and saving you will need to provide paths to files. If at least one of the sprites "
                           "currently being edited contains alpha-transparency, you must check corresponding "
                           "checkbox in the save dialog and provide path to .alp file. If you edited some advanced object attributes "
													 "(described below) you also must check corresponding checkbox and provide path to .aoa file.\n\n"
	                         "After new files have been created or existing have been loaded, "
                           "you can switch objects category using dropdown in the top-left corner. "
	                         "Then you can select existing object in the listbox under the dropdown.\n\n"
	                         "To create new object in the current category press \"New object\" button "
                           "in the bottom-left corner.\n\n"
                           "If existing object is selected and it has some sprites, they will be displayed on the right side in the "
                           "\"Object sprites\" tab.\n\n"
	                         "To import new sprites in the editor, select \"New sprites\" tab next to \"Object sprites\", then "
                           "press \"Import sprites\" button at the bottom right corner. You'll be able to select one or multiple "
                           "image files, which must be in PNG format.\n\n"
                           "In the middle area of the editor, you can manage object sprites layout and animations.\n\n"
                           "Main thing here is the sprites grid, surrounded by the orientation buttons. By default, new object will have "
                           "only one grid cell. Amount of cells can be changed by modifying \"Width\" and \"Height\" parameters.\n\n"
                           "You can drag & drop sprites from \"Object sprites\" / \"New sprites\" tabs to these grid cells. "
                           "Imported sprites, that are larger than 32x32 pixels, will be automatically layout into current grid.\n\n"
                           "Right-click on the sprite cell will bring popup menu, which will allow you to export this particular 32x32 sprite, "
													 "stored in current cell, or export whole composed frame for object with width or height more than 1. "
													 "Thus you can export whole multi-layer up to 4x4 size animation frame into one PNG file. This can be useful if "
													 "you somehow lost original image, used for animation.\n\n"
													 "You can also export or delete sprite by right-clicking it in \"Object sprites\" list.\n\n"
													 "If you've deleted sprite either in animation grid or in sprites list, it will be disassociated with "
                           "curret object, but it will still persist in .spr file. To completely remove unused sprites you will need "
													 "to use \"Tools\" menu (described below) \n\n"
													 "Next to other animation controls.\n\n"
                           "By pressing green arrow buttons you can switch orientation of animation, mostly used for character "
                           "animations.\n\n"
													 "\"Pattern width\" / \"Pattern height\" are used for repeating tiled objects, like grounds or walls, "
													 "so you can make variations of such tiles which will be placed depending on their in-game drawing coordinates. "
													 "\"Pattern width\" is also in connection with orientation, because orientation is in fact certain pattern iteration.\n\n"
													 "You can control current iteration across pattern width or height using \"xDiv\" / \"yDiv\" "
													 "arrows accordingly. Iterating \"xDiv\" is in fact the same thing as pressing some orintation arrow. "
													 "But orientation arrows are useful for characters, while \"xDiv\" arrows are useful for tiled objects "
													 "(grounds, walls etc.)\n\n"
													 "\"Layers count\" field modifies amount of layers in the animation and you can switch between layers using "
													 "\"lr #\" arrows. Layers can be drawn separately or blended (merged). This is controlled by \"Blend layers\" "
													 "checkbox.\n\n"
                           "\"Amounf of frames\" field is used to set animation frames number. Current frame can be switched using "
                           "\"fr #\" arrows.\n\n"
                           "\"Always animated\" checkbox is the attribute, used to indicate that object animation must be always "
                           "played in the game.\n\n"
													 "You can preview animation using \"Preview animation\" button. By modifying \"Preview FPS\" value you can "
													 "control how fast animation will be played.\n\n"
                           "Below the animation area is the attributes area. It is separated into \"Boolean attributes\" and "
                           "\"Value attributes\". Boolean attributes are simply values of \"true / false\" and value attributes, "
                           "though also have boolean toggle, have other (mostly numeric) values as well. When attribute checkbox "
                           "is not checked, its corresponding value fields are disabled.\n\n"
													 "The \"Tools\" menu contains some helpful tools.\n\n"
													 "\"Edit advanced attributes...\" (Ctrl+E hotkey) brings-up dialog, where you can edit some advanced attributes, "
													 "which will be associated with selected object. What are these attributes exactly depends on object category, "
													 "i.e. items and creatures have separate attributes set. Attributes need to be saved separately into .aoa file "
													 "(selected in save dialog).\n\n"
													 "\"Generate RME resources...\" (Ctrl+G hotkey) allows you to automatically generate some resources for "
													 "Remere's Map Editor. For now these resources are: items.otb, items.xml, creatures.xml and materials.xml. "
													 "Data for these files is taken partially from .dat file and partially from .aoa file, so you have to "
													 "properly set advanced object attributes for these files to be generated properly. For example, both items.xml "
													 "and creatures.xml require \"name\" advanced attribute to be set.\n\n"
													 "\"Clean-up unused sprites...\" tool can be helpful if you've deleted some sprites from the animation(s). "
													 "These sprites remain and memory and got saved into .spr file even if they're disassociated with their objects. "
													 "To get rid of them, you can just run this tool and it will perform required clean-up. Don't forget to save "
													 ".spr file after it has finished working.\n\n"
													 "You can undo / redo most operations using standard Ctrl+Z / Ctrl+Y hotkeys.";
	auto text = new wxTextCtrl(this, wxID_ANY, guideText, wxDefaultPosition, wxDefaultSize,
	                           wxTE_MULTILINE | wxTE_READONLY | wxNO_BORDER);
	text->SetBackgroundColour(GetDefaultAttributes().colBg);
	vbox->Add(text, 1, wxALL | wxEXPAND, 10);

	SetSizer(vbox);
	Center();
}
