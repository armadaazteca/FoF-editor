#ifndef _ITEMS_OTB_WRITER_H_
#define _ITEMS_OTB_WRITER_H_

#include <fstream>
#include <vector>
#include <memory>
#include "Interfaces.h"
#include "DatSprStructs.h"

using namespace std;

class ItemsOtbWriter
{
public:
	enum NodeType
	{
		NODE_START = 0xFE, NODE_END = 0xFF, ESCAPE_CHAR = 0xFD
	};

	enum ItemsOtbVersion
	{
		V_MAJOR = 3, V_MINOR = 40, V_BUILD = 42
	};

	enum ItemFlags
	{
		FLAG_BLOCK_SOLID = 1,
		FLAG_BLOCK_PROJECTILE = 2,
		FLAG_BLOCK_PATHFIND = 4,
		FLAG_USEABLE = 16,
		FLAG_PICKUPABLE = 32,
		FLAG_MOVEABLE = 64,
		FLAG_STACKABLE = 128,
		FLAG_HANGABLE = 65536
	};

	enum ItemAttributes
	{
		ITEM_ATTR_FIRST = 0x10,
		ITEM_ATTR_SERVER_ID = ITEM_ATTR_FIRST,
		ITEM_ATTR_CLIENT_ID,
		ITEM_ATTR_NAME,
		ITEM_ATTR_DESCR,
		ITEM_ATTR_SPEED,
		ITEM_ATTR_SLOT,
		ITEM_ATTR_MAXITEMS,
		ITEM_ATTR_WEIGHT,
		ITEM_ATTR_WEAPON,
		ITEM_ATTR_AMU,
		ITEM_ATTR_ARMOR,
		ITEM_ATTR_MAGLEVEL,
		ITEM_ATTR_MAGFIELDTYPE,
		ITEM_ATTR_WRITEABLE,
		ITEM_ATTR_ROTATETO,
		ITEM_ATTR_DECAY,
		ITEM_ATTR_SPRITEHASH,
		ITEM_ATTR_MINIMAPCOLOR,
		ITEM_ATTR_07,
		ITEM_ATTR_08,
		ITEM_ATTR_LIGHT,
		ITEM_ATTR_DECAY2,
		ITEM_ATTR_WEAPON2,
		ITEM_ATTR_AMU2,
		ITEM_ATTR_ARMOR2,
		ITEM_ATTR_WRITEABLE2,
		ITEM_ATTR_LIGHT2,
		ITEM_ATTR_TOPORDER,
		ITEM_ATTR_WRITEABLE3,
		ITEM_ATTR_LAST
	};

	static const unsigned char ROOT_ATTR_VERSION = 1;
	static const unsigned short DATA_LENGTH = 140, VERSION_STR_LENGTH = 128;
	static const char * OTB_VERSION_STR;

	typedef vector <shared_ptr <DatObject>> DatObjectList;

	static ItemsOtbWriter & getInstance();
	bool writeItemsOtb(shared_ptr <DatObjectList> items, const wxString & filename, ProgressUpdatable * progressUpdatable);

private:
	ItemsOtbWriter() {}
	void writeByte(unsigned char byte);
	void writeU16(unsigned short u16);
	void writeU32(unsigned int u32);

	fstream file;
};

#endif // _ITEMS_OTB_WRITER_H_
