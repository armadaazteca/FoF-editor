#ifndef _CONFIG_H_
#define _CONFIG_H_

#include <wx/string.h>

class Config
{
public:
	static const wxString & APP_NAME;
	static const int MAJOR_VERSION = 0;
	static const int MINOR_VERSION = 9;
	static const int BUILD_VERSION = 4;
	static const wxString & VERSION_STRING;
	static const int SPRITE_SIZE = 32;
	static const int MAIN_WINDOW_WIDTH = 1280;
	static const int MAIN_WINDOW_HEIGHT = 800;
	static const int MAX_OBJECT_WIDTH = 16;
	static const int MAX_OBJECT_HEIGHT = 16;
	static const int MAX_XY_DIV = 8;
	static const int MAX_LAYERS = 4;
	static const int MAX_ANIM_FRAMES = 128;
	static const int MAX_PREVIEW_FPS = 30;
	static const int OBJECTS_LIST_BOX_WHEEL_SCROLL_LINES = 10;
	static const wxString & ERROR_TITLE;
	static const wxString & COMMON_READ_ERROR, & COMMON_WRITE_ERROR;
	static const int COMMON_NUM_FIELD_WIDTH = 55;
	static const int SMALL_SQUARE_BUTTON_SIZE = 30;
	static const int MAX_UNDO_RECORDS = 100;
	static const int BLOCKING_MARK_X = 21;
	static const int BLOCKING_MARK_Y = 2;
};

#endif // _CONFIG_H_
