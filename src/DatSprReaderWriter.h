#ifndef _DAT_SPR_READER_WRITER_H_
#define _DAT_SPR_READER_WRITER_H_

#include <vector>
#include <map>
#include <memory>
#include <fstream>
#include "Interfaces.h"
#include "DatSprStructs.h"

using namespace std;

class DatSprReaderWriter
{
public:
	enum Error
	{
		ERROR_INVALID_SIGNATURE,
		ERROR_UNKNOWN
	};

	const static unsigned int DEFAULT_DAT_SIGNATURE = 0x4FFA74CC; // signatures for
	const static unsigned int DEFAULT_SPR_SIGNATURE = 0x4FFA74F9; // Tibia 9.60 format
	const static unsigned int ITEM_FIRST_ID = 100;
	typedef vector <shared_ptr <DatObject>> DatObjectList;
	typedef map <unsigned int, shared_ptr <Sprite>> SpriteMap;

	static DatSprReaderWriter & getInstance();
	void initNewData();
	bool readDat(const wxString & filename, ProgressUpdatable * progressUpdatable);
	bool readSpr(const wxString & filename, ProgressUpdatable * progressUpdatable);
	bool readBlockingStates(const wxString & filename, ProgressUpdatable * progressUpdatable);
	bool readAlpha(const wxString & filename, ProgressUpdatable * progressUpdatable);
	bool writeDat(const wxString & filename, ProgressUpdatable * progressUpdatable);
	bool writeSpr(const wxString & filename, ProgressUpdatable * progressUpdatable);
	bool writeBlockingStates(const wxString & filename, ProgressUpdatable * progressUpdatable);
	bool writeAlpha(const wxString & filename, ProgressUpdatable * progressUpdatable);
	shared_ptr <DatObjectList> getObjects(DatObjectCategory category);
	inline shared_ptr <SpriteMap> getSprites() { return sprites; }
	inline void setSprites(shared_ptr <SpriteMap> sprites) { this->sprites = sprites; }
	inline unsigned int getMaxSpriteId() { return maxSpriteId; }
	inline unsigned int incrementMaxSpriteId() { return maxSpriteId++; } // return first, then increment
	inline Error getLastError() { return lastError; }

private:
	DatSprReaderWriter() {}

	unsigned char readByte();
	unsigned short readU16();
	unsigned int readU32();
	char * readString();

	void writeByte(unsigned char byte);
	void writeU16(unsigned short u16);
	void writeU32(unsigned int u32);
	void writeString(char * str);

	bool hasData = false;
	fstream file;
	unsigned int datSignature = 0, sprSignature = 0;
	unsigned short itemsCount = 0, creaturesCount = 0, effectsCount = 0, projectilesCount = 0;
	shared_ptr <DatObjectList> items = nullptr, creatures = nullptr, effects = nullptr, projectiles = nullptr;
	shared_ptr <SpriteMap> sprites = nullptr;
	unsigned int maxSpriteId = 0;
	Error lastError = ERROR_UNKNOWN;
};

#endif // _DAT_SPR_READER_WRITER_H_
