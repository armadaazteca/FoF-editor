#include <wx/wxprec.h>
#ifndef WX_PRECOMP
#include <wx/wx.h>
#include <wx/memory.h>
#endif
#include "ItemsOtbWriter.h"

const char * ItemsOtbWriter::OTB_VERSION_STR = "OTB 3.40.42-9.60";

ItemsOtbWriter & ItemsOtbWriter::getInstance()
{
	static ItemsOtbWriter instance;
	return instance;
}

bool ItemsOtbWriter::writeItemsOtb(DatObjectList items, const wxString & filename, ProgressUpdatable * progressUpdatable)
{
	file.open(filename.mb_str(), ios::in | ios::binary);
	if (file.is_open())
	{
		writeU32(0); // write 4 bytes for file version, they aren't used really, it seems
		writeByte(NODE_START);
		writeByte(0); // type info, seems to be unused
		writeU32(0); // some flags maybe, also looks unused
		writeByte(ROOT_ATTR_VERSION); // version attribute
		writeU16(DATA_LENGTH); // data length here should be constant
		// writing version, not sure why every version component is stored in U32 though, just waste of space...
		writeU32(V_MAJOR);
		writeU32(V_MINOR);
		writeU32(V_BUILD);
		// writing string version
		int versionLen = strlen(OTB_VERSION_STR), remainingLen = DATA_LENGTH - versionLen;
		file.write(OTB_VERSION_STR, versionLen);
		// filling up to 128 bytes (format requirement)
		char * zeros = new char[remainingLen];
		memset(zeros, 0, remainingLen);
		file.write(zeros, remainingLen);
		delete [] zeros;

		// writing actual items
		for (auto & item : items)
		{
			writeByte(NODE_START);

			writeByte(1); // TODO: here should be 'group' byte, 1 is for 'Ground'

			// writing flags
			unsigned int flags = 0;
			if (!item->isWalkable) flags |= FLAG_BLOCK_SOLID;
			if (item->blocksProjectiles) flags |= FLAG_BLOCK_PROJECTILE;
			if (!item->isPathable) flags |= FLAG_BLOCK_PATHFIND;
			if (item->isPickupable) flags |= FLAG_PICKUPABLE;
			if (item->isMovable) flags |= FLAG_MOVEABLE;
			if (item->isStackable) flags |= FLAG_STACKABLE;
			if (item->isHangable) flags |= FLAG_HANGABLE;
			if (item->isUsable) flags |= FLAG_USEABLE;
			writeU32(flags);

			// writing attributes
			const unsigned short datalen = 2; // this is constant 2 bytes

			// writing server ID, this will be simply item ID
			writeByte(ITEM_ATTR_SERVER_ID);
			writeU16(datalen);
			writeU16(item->id);

			// writing client ID, this will be SAME item ID, because we're making one-to-one mapping
			writeByte(ITEM_ATTR_CLIENT_ID);
			writeU16(datalen);
			writeU16(item->id);

			// if item 'isFullGround', writing ground speed, it's not used in RME, but just for compatibility
			if (item->isFullGround)
			{
				writeByte(ITEM_ATTR_SPEED);
				writeU16(datalen);
				writeU16(item->groundSpeed);
			}

			// if item 'isLightSource', writing light intensity and color, it's not used in RME, but just for compatibility
			if (item->isLightSource)
			{
				writeByte(ITEM_ATTR_LIGHT2);
				writeU16(datalen * 2); // need x2 datalen here
				writeU16(item->lightIntensity);
				writeU16(item->lightColor);
			}

			writeByte(NODE_END);
		}

		writeByte(NODE_END); // closing root node

		file.close();
		return true;
	}
	return false;
}

void ItemsOtbWriter::writeByte(unsigned char byte)
{
	file.write((char *) &byte, 1);
}

void ItemsOtbWriter::writeU16(unsigned short u16)
{
	file.write((char *) &u16, 2);
}

void ItemsOtbWriter::writeU32(unsigned int u32)
{
	file.write((char *) &u32, 4);
}
