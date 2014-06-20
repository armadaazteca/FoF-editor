#include <wx/wxprec.h>
#ifndef WX_PRECOMP
#include <wx/wx.h>
#include <wx/memory.h>
#endif
#include "Interfaces.h"
#include "DatSprReaderWriter.h"

DatSprReaderWriter & DatSprReaderWriter::getInstance()
{
	static DatSprReaderWriter instance;
	return instance;
}

bool DatSprReaderWriter::readDat(const wxString & filename, ProgressUpdatable * progressUpdatable)
{
	file.open(filename.mb_str(), ios::binary);
	if (file.is_open())
	{
		datSignature = readU32();
		itemsCount = readU16();
		creaturesCount = readU16();
		effectsCount = readU16();
		projectilesCount = readU16();
		unsigned int totalCount = itemsCount + creaturesCount + effectsCount + projectilesCount;

		items = make_shared <DatObjectList> ();
		items->reserve(itemsCount);
		creatures = make_shared <DatObjectList> ();
		creatures->reserve(creaturesCount);
		effects = make_shared <DatObjectList> ();
		effects->reserve(effectsCount);
		projectiles = make_shared <DatObjectList> ();
		projectiles->reserve(projectilesCount);
		shared_ptr <DatObjectList> lists[] = { items, creatures, effects, projectiles };
		unsigned short sizes[] = { itemsCount, creaturesCount, effectsCount, projectilesCount };
		shared_ptr <DatObjectList> currentList = nullptr;

		unsigned int readings = 100; // because '100' is first item id
		shared_ptr <DatObject> object = nullptr;
		unsigned short id = 0, size = 0;
		int attr = 0;
		for (int category = CategoryItem; category < LastCategory; category++)
		{
			currentList = lists[category];
			size = sizes[category];
			for (id = category == CategoryItem ? 100 : 1; id <= size; id++)
			{
				object = make_shared <DatObject> ();
				object->id = id;
				do
				{
					attr = readByte();
					switch (attr)
					{
						case AttrGround:
							object->isGround = true;
							object->speed = readU16();
						break;
						case AttrGroundBorder:
							object->isGroundBorder = true;
						break;
						case AttrOnBottom:
							object->isOnBottom = true;
						break;
						case AttrOnTop:
							object->isOnTop = true;
						break;
						case AttrContainer:
							object->isContainer = true;
						break;
						case AttrStackable:
							object->isStackable = true;
						break;
						case AttrForceUse:
							object->isForceUse = true;
						break;
						case AttrMultiUse:
							object->isMultiUse = true;
						break;
						case AttrWritable:
							object->isWritable = true;
							object->charsToWrite = readU16();
						break;
						case AttrWritableOnce:
							object->isWritableOnce = true;
							object->charsToWrite = readU16();
						break;
						case AttrFluidContainer:
							object->isFluidContainer = true;
						break;
						case AttrSplash:
							object->isSplash = true;
						break;
						case AttrNotWalkable:
							object->isWalkable = false;
						break;
						case AttrNotMoveable:
							object->isMovable = false;
						break;
						case AttrBlockProjectile:
							object->blocksProjectiles = true;
						break;
						case AttrNotPathable:
							object->isPathable = false;
						break;
						case AttrPickupable:
							object->isPickupable = true;
						break;
						case AttrHangable:
							object->isHangable = true;
						break;
						case AttrHookSouth:
							object->isHookSouth = true;
						break;
						case AttrHookEast:
							object->isHookEast = true;
						break;
						case AttrRotateable:
							object->isRotatable = true;
						break;
						case AttrLight:
							object->isLightSource = true;
							object->lightIntensity = readU16();
							object->lightColor = readU16();
						break;
						case AttrDontHide:
							object->canBeHidden = false;
						break;
						case AttrTranslucent:
							object->isTranslucent = true;
						break;
						case AttrDisplacement:
							object->hasDisplacement = true;
							object->displacementX = readU16();
							object->displacementY = readU16();
						break;
						case AttrElevation:
							object->isRaised = true;
							object->elevation = readU16();
						break;
						case AttrLyingCorpse:
							object->isLyingCorpse = true;
						break;
						case AttrAnimateAlways:
							object->isAlwaysAnimated = true;
						break;
						case AttrMinimapColor:
							object->isMinimap = true;
							object->minimapColor = readU16();
						break;
						case AttrLensHelp:
							object->isLensHelp = true;
							object->lensHelp = readU16();
						break;
						case AttrFullGround:
							object->isFullGround = true;
						break;
						case AttrLook:
							object->ignoreLook = true;
						break;
						case AttrCloth:
							object->isCloth = true;
							object->clothSlot = readU16();
						break;
						case AttrMarket:
							object->isMarketable = true;
							object->marketCategory = readU16();
							object->marketTradeAs = readU16();
							object->marketShowAs = readU16();
							object->marketName = readString();
							object->marketRestrictVocation = readU16();
							object->marketRequiredLevel = readU16();
						break;
						case AttrUsable:
							object->isUsable = true;
						break;
						case AttrLast:
						break;
						default:
							object->otherAttrs[object->otherAttrsCount++] = attr;
						break;
					}
				}
				while (attr != AttrLast);

				object->width = readByte();
				object->height = readByte();
				if (object->width > 1 || object->height > 1)
				{
					object->exactSize = readByte();
				}
				object->layersCount = readByte();
				object->patternWidth = readByte();
				object->patternHeight = readByte();
				object->patternDepth = readByte();
				object->phasesCount = readByte();
				object->spriteCount = object->width * object->height * object->layersCount * object->patternWidth
				                    * object->patternHeight * object->patternDepth * object->phasesCount;

				object->spriteIDs = new unsigned short[object->spriteCount];
				for (unsigned short i = 0; i < object->spriteCount; ++i)
				{
					object->spriteIDs[i] = readU32();
				}

				currentList->push_back(object);

				progressUpdatable->updateProgress(++readings / (double) totalCount);
			}
		}
		file.close();
	}
	else
	{
		return false;
	}
	return true;
}

bool DatSprReaderWriter::readSpr(const wxString & filename, ProgressUpdatable * progressUpdatable)
{
	file.open(filename.mb_str(), ios::binary);
	if (file.is_open())
	{
		sprSignature = readU32();
		unsigned int spritesCount = readU32();
		sprites = make_shared <SpriteMap> ();
		shared_ptr <Sprite> sprite = nullptr;
		for (unsigned int id = 1; id <= spritesCount; ++id)
		{
			sprite = make_shared <Sprite> ();
			sprite->id = id;
			sprite->offset = readU32();
			(*sprites)[id] = sprite;
		}

		unsigned short pixelDataSize = 0, transparentPixels = 0, coloredPixels = 0;
		unsigned short readPos = 0, writePosRgb = 0, writePosAlpha = 0;
		unsigned char * bitmap = nullptr, * alpha = nullptr;
		unsigned int readings = 0;
		for (auto & kv : *sprites)
		{
			sprite = kv.second;
			if (sprite->offset != 0)
			{
				file.seekg(sprite->offset  + 3); // +3 to skip transparent pixels color
				pixelDataSize = readU16();
				if (pixelDataSize == 0) continue;
				readPos = writePosRgb = writePosAlpha = 0;
				bitmap = sprite->rgb;
				alpha = sprite->alpha;
				// decompress pixels
				while (readPos < pixelDataSize && writePosRgb < Sprite::RGB_SIZE)
				{
					transparentPixels = readU16();
					coloredPixels = readU16();
					for (unsigned short i = 0; i < transparentPixels && writePosRgb < Sprite::RGB_SIZE; ++i)
					{
						bitmap[writePosRgb + 0] = 0x00;
						bitmap[writePosRgb + 1] = 0x00;
						bitmap[writePosRgb + 2] = 0x00;
						alpha[writePosAlpha] = 0x00;
						writePosRgb += 3;
						writePosAlpha++;
					}
					for (unsigned short i = 0; i < coloredPixels && writePosRgb < Sprite::RGB_SIZE; ++i)
					{
						bitmap[writePosRgb + 0] = readByte();
						bitmap[writePosRgb + 1] = readByte();
						bitmap[writePosRgb + 2] = readByte();
						alpha[writePosAlpha] = 0xFF;
						writePosRgb += 3;
						writePosAlpha++;
					}
					readPos += 4 + (3 * coloredPixels);
				}
				// fill remaining pixels with alpha
				while (writePosRgb < Sprite::RGB_SIZE)
				{
					bitmap[writePosRgb + 0] = 0x00;
					bitmap[writePosRgb + 1] = 0x00;
					bitmap[writePosRgb + 2] = 0x00;
					alpha[writePosAlpha] = 0x00;
					writePosRgb += 3;
					writePosAlpha++;
				}
				sprite->valid = true;
			}

			progressUpdatable->updateProgress(++readings / (double) spritesCount);
		}

		file.close();
	}
	else
	{
		return false;
	}
	return true;
}

unsigned char DatSprReaderWriter::readByte()
{
	file.read(buffer, 1);
	return (unsigned char) buffer[0];
}

unsigned short DatSprReaderWriter::readU16()
{
	unsigned short result = 0;
	file.read(buffer, 2);
	memcpy(&result, buffer, 2);
	return result;
}

unsigned int DatSprReaderWriter::readU32()
{
	unsigned int result = 0;
	file.read(buffer, 4);
	memcpy(&result, buffer, 4);
	return result;
}

char * DatSprReaderWriter::readString()
{
	unsigned short len = 0;
	file.read(buffer, 2);
	memcpy(&len, buffer, 2);
	char * result = new char[len];
	file.read(buffer, len);
	memcpy(result, buffer, len);
	return result;
}

shared_ptr <DatSprReaderWriter::DatObjectList> DatSprReaderWriter::getObjects(DatObjectCategory category)
{
	switch (category)
	{
		case CategoryItem: return items; break;
		case CategoryCreature: return creatures; break;
		case CategoryEffect: return effects; break;
		case CategoryProjectile: return projectiles; break;
		default: return nullptr;
	}
	return nullptr;
}
