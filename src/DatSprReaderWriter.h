#ifndef _DAT_SPR_READER_WRITER_H_
#define _DAT_SPR_READER_WRITER_H_

#include <vector>
#include <map>
#include <memory>
#include <iostream>
#include <fstream>
#include "DatSprStructs.h"

using namespace std;

class DatSprReaderWriter
{
public:
	const static unsigned int ITEM_FIRST_ID = 100;
	typedef vector <shared_ptr <DatObject>> DatObjectList;
	typedef map <unsigned int, shared_ptr <Sprite>> SpriteMap;
	static DatSprReaderWriter & getInstance();
	bool readDat(const wxString & filename, ProgressUpdatable * progressUpdatable);
	bool readSpr(const wxString & filename, ProgressUpdatable * progressUpdatable);
	shared_ptr <DatObjectList> getObjects(DatObjectCategory category);
	inline shared_ptr <SpriteMap> getSprites() { return sprites; }
private:
	ifstream file;
	char buffer[8192];
	unsigned int datSignature = 0, sprSignature = 0;
	unsigned short itemsCount = 0, creaturesCount = 0, effectsCount = 0, projectilesCount = 0;
	shared_ptr <DatObjectList> items = nullptr, creatures = nullptr, effects = nullptr, projectiles = nullptr;
	shared_ptr <SpriteMap> sprites = nullptr;

	unsigned char readByte();
	unsigned short readU16();
	unsigned int readU32();
	char * readString();
};

#endif // _DAT_SPR_READER_WRITER_H_
