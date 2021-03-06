#ifndef _DAT_SPR_STRUCTS_H_
#define _DAT_SPR_STRUCTS_H_

#include <memory>

using namespace std;

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

	// custom attributes start here, better to give them higher number just in case
	AttrMount = 200,

	AttrLast = 255
};

enum ItemGroup
{
	ITEM_GROUP_NONE = 0,
	ITEM_GROUP_GROUND,
	ITEM_GROUP_CONTAINER,
	ITEM_GROUP_WEAPON,
	ITEM_GROUP_AMMUNITION,
	ITEM_GROUP_ARMOR,
	ITEM_GROUP_RUNE,
	ITEM_GROUP_TELEPORT,
	ITEM_GROUP_MAGICFIELD,
	ITEM_GROUP_WRITEABLE,
	ITEM_GROUP_KEY,
	ITEM_GROUP_SPLASH,
	ITEM_GROUP_FLUID,
	ITEM_GROUP_DOOR,
	ITEM_GROUP_DEPRECATED,
	ITEM_GROUP_LAST
};

enum ItemArticles { ARTICLE_A, ARTICLE_AN };

enum ItemFloorChange
{
	FLOOR_CHANGE_NONE, FLOOR_CHANGE_DOWN, FLOOR_CHANGE_NORTH, FLOOR_CHANGE_SOUTH,
	FLOOR_CHANGE_WEST, FLOOR_CHANGE_EAST, FLOOR_CHANGE_LAST
};

enum CreatureType { CREATURE_MONSTER, CREATURE_NPC };

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
	bool isCloth = false, isMarketable = false, isUsable = false, hasMount = false;
	short groundSpeed = 0;
	unsigned short lightIntensity = 0;
	unsigned short lightColor = 0;
	short displacementX = 0, displacementY = 0;
	short marketCategory = 0;
	unsigned short marketTradeAs = 0, marketShowAs = 0;
	unique_ptr <char[]> marketName = nullptr;
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
	unique_ptr <unsigned int[]> spriteIDs = nullptr;
};

struct Sprite
{
	// 1024 pixels, 3 bytes per rgb pixel, 1 byte for alpha channel
	static const unsigned short RGB_SIZE = 3072, ALPHA_SIZE = 1024;
	bool valid = false, changed = false, hasRealAlpha = false;
	unsigned int id = 0, offset = 0, offsetAlpha = 0;
	unique_ptr <unsigned char[]> rgb = nullptr;
	unique_ptr <unsigned char[]> alpha = nullptr;
	unsigned short compressedDataSize = 0;
	unique_ptr <unsigned char[]> compressedData = nullptr;
	bool isUsed = false, isBlocking = false;
};

struct AdvancedObjectAttributes
{
	unsigned char group = 255; // non-existing group by default, should fallback to 'Other'
	unique_ptr <char[]> name = nullptr, description = nullptr;
	unique_ptr <unsigned int[]> frameTimes = nullptr;
	unsigned int frameTimesLen = 0;
	unsigned char article = ARTICLE_A, floorChange = FLOOR_CHANGE_NONE, creatureType = CREATURE_MONSTER;
};

#endif // _DAT_SPR_STRUCTS_H_
