#ifndef _DAT_STRUCTS_H_
#define _DAT_STRUCTS_H_

enum DatObjectCategory : unsigned char
{
	CategoryItem = 0,
	CategoryCreature,
	CategoryEffect,
	CategoryProjectile,
	InvalidCategory,
	LastCategory = InvalidCategory
};

enum DatObjectAttr : unsigned char
{
	AttrGround = 0,
	AttrGroundBorder = 1,
	AttrOnBottom = 2,
	AttrOnTop = 3,
	AttrContainer = 4,
	AttrStackable = 5,
	AttrForceUse = 6,
	AttrMultiUse = 7,
	AttrWritable = 8,
	AttrWritableOnce = 9,
	AttrFluidContainer = 10,
	AttrSplash = 11,
	AttrNotWalkable = 12,
	AttrNotMoveable = 13,
	AttrBlockProjectile = 14,
	AttrNotPathable = 15,
	AttrPickupable = 16,
	AttrHangable = 17,
	AttrHookSouth = 18,
	AttrHookEast = 19,
	AttrRotateable = 20,
	AttrLight = 21,
	AttrDontHide = 22,
	AttrTranslucent = 23,
	AttrDisplacement = 24,
	AttrElevation = 25,
	AttrLyingCorpse = 26,
	AttrAnimateAlways = 27,
	AttrMinimapColor = 28,
	AttrLensHelp = 29,
	AttrFullGround = 30,
	AttrLook = 31,
	AttrCloth = 32,
	AttrMarket = 33,
	AttrUsable = 34,

	// TODO: maybe remove later
	/* additional
	AttrOpacity = 100,
	AttrNotPreWalkable = 101,

	AttrNoMoveAnimation = 253, // real value is 16, but we need to do this for backwards compatibility
	*/

	AttrLast = 255
};

struct DatObject
{
	// attributes
	unsigned int id = 0;
	bool isGround = false, isGroundBorder = false, isOnBottom = false, isOnTop = false, isContainer = false;
	bool isStackable = false, isForceUse = false, isMultiUse = false, isWritable = false, isWritableOnce = false;
	bool isFluidContainer = false, isSplash = false, blocksProjectiles = false, isPickupable = false;
	bool isWalkable = true, isMovable = true, isPathable = true, canBeHidden = true; // true by default attributes
	bool isHangable = false, isHookSouth = false, isHookEast = false, isRotatable = false, isLightSource = false;
	bool isTranslucent = false, hasDisplacement = false, isRaised = false, isLyingCorpse = false;
	bool isAlwaysAnimated = false, isMinimap = false, isLensHelp = false, isFullGround = false, ignoreLook = false;
	bool isCloth = false, isMarketable = false, isUsable = false;
	short speed = 0;
	unsigned short lightIntensity = 0;
	unsigned short lightColor = 0;
	short displacementX = 0, displacementY = 0;
	short marketCategory = 0;
	unsigned short marketTradeAs = 0, marketShowAs = 0;
	char * marketName = nullptr;
	unsigned short marketRestrictVocation = 0;
	unsigned short marketRequiredLevel = 0;
	short elevation = 0;
	unsigned short charsToWrite = 0;
	unsigned short minimapColor = 0;
	unsigned short clothSlot = 0;
	unsigned short lensHelp = 0;
	bool allAttrs[256];

	// sprite data
	unsigned char width = 0, height = 0, exactSize = 0;
	unsigned char patternWidth = 0, patternHeight = 0, patternDepth = 0;
	unsigned char layersCount, phasesCount = 0;
	unsigned int spriteCount = 0;
	unsigned int * spriteIDs = nullptr;
	inline ~DatObject() { delete[] spriteIDs; }
};

struct Sprite
{
	// 1024 pixels, 3 bytes per rgb pixel, 1 byte for alpha channel
	static const unsigned short RGB_SIZE = 3072, ALPHA_SIZE = 1024;
	bool valid = false, changed = false;
	unsigned int id = 0, offset = 0;
	unsigned char * rgb; //[RGB_SIZE];
	unsigned char * alpha; //[ALPHA_SIZE];
	unsigned char * compressedData = nullptr;
	unsigned short compressedDataSize = 0;
	//inline ~Sprite() { delete[] compressedData; }
};

#endif // _DAT_STRUCTS_H_
