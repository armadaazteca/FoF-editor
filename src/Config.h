#ifndef _CONFIG_H_
#define _CONFIG_H_

#include <wx/string.h>

class Config
{
public:
	static const wxString & APP_NAME;
	static const int MAJOR_VERSION = 0;
	static const int MINOR_VERSION = 8;
	static const int BUILD_VERSION = 0;
	static const wxString & VERSION_STRING;
	static const int SPRITE_SIZE = 32;
	static const int MAX_OBJECT_WIDTH = 4;
	static const int MAX_OBJECT_HEIGHT = 4;
	static const int MAX_ANIM_FRAMES = 8;
	static const int OBJECTS_LIST_BOX_WHEEL_SCROLL_LINES = 10;
};

#endif // _CONFIG_H_
