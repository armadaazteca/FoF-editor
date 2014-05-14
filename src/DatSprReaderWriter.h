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
	bool readDat(const wxString & filename);
	bool readSpr(const wxString & filename);
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
