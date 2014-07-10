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

void DatSprReaderWriter::initNewData()
{
	datSignature = DEFAULT_DAT_SIGNATURE;
	sprSignature = DEFAULT_SPR_SIGNATURE;
	items = make_shared <DatObjectList> ();
	creatures = make_shared <DatObjectList> ();
	effects = make_shared <DatObjectList> ();
	projectiles = make_shared <DatObjectList> ();
	sprites = make_shared <SpriteMap> ();
	hasData = true;
}

bool DatSprReaderWriter::readDat(const wxString & filename, ProgressUpdatable * progressUpdatable)
{
	file.open(filename.mb_str(), ios::in | ios::binary);
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
				memset(object->allAttrs, 0, AttrLast);
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
					}
					object->allAttrs[attr] = true;
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

				object->spriteIDs = new unsigned int[object->spriteCount];
				for (unsigned int i = 0; i < object->spriteCount; ++i)
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
	file.open(filename.mb_str(), ios::in | ios::binary);
	if (file.is_open())
	{
		sprSignature = readU32();
		unsigned int spritesCount = readU32();
		sprites = make_shared <SpriteMap> ();
		shared_ptr <Sprite> sprite = nullptr;
		unsigned int id = 1;
		for (; id <= spritesCount; ++id)
		{
			sprite = make_shared <Sprite> ();
			sprite->id = id;
			sprite->offset = readU32();
			(*sprites)[id] = sprite;
		}
		maxSpriteId = id;

		unsigned short transparentPixels = 0, coloredPixels = 0;
		unsigned short readPos = 0, writePosRgb = 0, writePosAlpha = 0;
		unsigned char * bitmap = nullptr, * alpha = nullptr;
		unsigned int readings = 0;
		for (auto & kv : *sprites)
		{
			sprite = kv.second;
			if (sprite->offset != 0)
			{
				file.seekg(sprite->offset + 3); // +3 to skip transparent pixels color
				unsigned short pixelDataSize = readU16();
				if (pixelDataSize == 0) continue;
				sprite->compressedDataSize = pixelDataSize;
				unsigned char * pixelDataBuffer = new unsigned char[pixelDataSize];
				file.read((char *) pixelDataBuffer, pixelDataSize);
				sprite->compressedData = pixelDataBuffer;

				readPos = writePosRgb = writePosAlpha = 0;
				bitmap = sprite->rgb;
				alpha = sprite->alpha;
				// decompress pixels
				while (readPos < pixelDataSize && writePosRgb < Sprite::RGB_SIZE)
				{
					memcpy(&transparentPixels, &pixelDataBuffer[readPos], 2);
					readPos += 2;
					memcpy(&coloredPixels, &pixelDataBuffer[readPos], 2);
					readPos += 2;
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
						bitmap[writePosRgb + 0] = pixelDataBuffer[readPos++];
						bitmap[writePosRgb + 1] = pixelDataBuffer[readPos++];
						bitmap[writePosRgb + 2] = pixelDataBuffer[readPos++];
						alpha[writePosAlpha] = 0xFF;
						writePosRgb += 3;
						writePosAlpha++;
					}
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
	hasData = true;
	return true;
}

unsigned char DatSprReaderWriter::readByte()
{
	unsigned char result = 0;
	file.read((char *) &result, 1);
	return result;
}

unsigned short DatSprReaderWriter::readU16()
{
	unsigned short result = 0;
	file.read((char *) &result, 2);
	return result;
}

unsigned int DatSprReaderWriter::readU32()
{
	unsigned int result = 0;
	file.read((char *) &result, 4);
	return result;
}

char * DatSprReaderWriter::readString()
{
	unsigned short len = 0;
	file.read((char *) &len, 2);
	char * result = new char[len + 1];
	file.read(result, len);
	result[len] = 0; // null-terminating
	return result;
}

bool DatSprReaderWriter::writeDat(const wxString & filename, ProgressUpdatable * progressUpdatable)
{
	file.open(filename.mb_str(), ios::out | ios::binary | ios::trunc);
	if (file.is_open())
	{
		writeU32(datSignature);
		writeU16(items->size() + 100);
		writeU16(creatures->size());
		writeU16(effects->size());
		writeU16(projectiles->size());
		unsigned int totalCount = items->size() + creatures->size() + effects->size() + projectiles->size();

		shared_ptr <DatObjectList> lists[] = { items, creatures, effects, projectiles };
		shared_ptr <DatObjectList> currentList = nullptr;
		unsigned int writings = 0;

		for (int category = CategoryItem; category < LastCategory; category++)
		{
			currentList = lists[category];
			for (auto & object : *currentList)
			{
				for (int attr = 0; attr <= AttrLast; ++attr)
				{
					if (object->allAttrs[attr])
					{
						writeByte(attr);
						switch (attr)
						{
							case AttrGround:
								writeU16(object->speed);
							break;
							case AttrWritable:
							case AttrWritableOnce:
								writeU16(object->charsToWrite);
							break;
							case AttrLight:
								writeU16(object->lightIntensity);
								writeU16(object->lightColor);
							break;
							case AttrDisplacement:
								writeU16(object->displacementX);
								writeU16(object->displacementY);
							break;
							case AttrElevation:
								writeU16(object->elevation);
							break;
							case AttrMinimapColor:
								writeU16(object->minimapColor);
							break;
							case AttrLensHelp:
								writeU16(object->lensHelp);
							break;
							case AttrCloth:
								writeU16(object->clothSlot);
							break;
							case AttrMarket:
								writeU16(object->marketCategory);
								writeU16(object->marketTradeAs);
								writeU16(object->marketShowAs);
								writeString(object->marketName);
								writeU16(object->marketRestrictVocation);
								writeU16(object->marketRequiredLevel);
							break;
						}
					}
				}

				writeByte(object->width);
				writeByte(object->height);
				if (object->width > 1 || object->height > 1)
				{
					writeByte(object->exactSize);
				}
				writeByte(object->layersCount);
				writeByte(object->patternWidth);
				writeByte(object->patternHeight);
				writeByte(object->patternDepth);
				writeByte(object->phasesCount);
				for (unsigned int i = 0; i < object->spriteCount; ++i)
				{
					writeU32(object->spriteIDs[i]);
				}

				progressUpdatable->updateProgress(++writings / (double) totalCount);
			}
		}

		file.close();
	}
	return true;
}

bool DatSprReaderWriter::writeSpr(const wxString & filename, ProgressUpdatable * progressUpdatable)
{
	file.open(filename.mb_str(), ios::out | ios::binary | ios::trunc);
	if (file.is_open())
	{
		writeU32(sprSignature);
		unsigned int spritesCount = sprites->size();
		writeU32(spritesCount);

		int begOffset = 0; // offset where sprite offsets writing begins
		file.seekp(begOffset + spritesCount * 4); // skipping space required for writing sprite offsets

		shared_ptr <Sprite> sprite = nullptr;
		unsigned int id = 1;
		int lastOffset = 0;
		unsigned int writings = 0;
		for (; id <= spritesCount; ++id)
		{
			sprite = (*sprites)[id];
			sprite->offset = file.tellp();

			// writing constant transparent pixels color - magenta
			writeByte(255);
			writeByte(0);
			writeByte(255);

			if (sprite->changed) // recompressing sprite pixel data
			{

			}
			else // writing already existing compressed data
			{
				file.write((char *) sprite->compressedData, sprite->compressedDataSize);
			}
			lastOffset = file.tellp();
			file.seekp(begOffset + (id - 1) * 4); // moving to the beginning to write sprite offset
			writeU32(sprite->offset);
			file.seekp(lastOffset); // moving back

			progressUpdatable->updateProgress(++writings / (double) spritesCount);
		}

		file.close();
	}
	return true;
}

void DatSprReaderWriter::writeByte(unsigned char byte)
{
	file.write((char *) &byte, 1);
}

void DatSprReaderWriter::writeU16(unsigned short u16)
{
	file.write((char *) &u16, 2);
}

void DatSprReaderWriter::writeU32(unsigned int u32)
{
	file.write((char *) &u32, 4);
}

void DatSprReaderWriter::writeString(char * str)
{
	unsigned short len = strlen(str);
	writeU16(len);
	file.write(str, len);
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
