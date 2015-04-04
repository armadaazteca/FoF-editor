#include <wx/wxprec.h>
#ifndef WX_PRECOMP
#include <wx/wx.h>
#endif
#include <wx/wfstream.h>
#include <wx/zipstrm.h>
#include <string>
#include <vector>
#include "jsoncpp/json.h"
#include "Events.h"
#include "AdvancedAttributesManager.h"

const string & EMPTY_STR = "";

AdvancedAttributesManager & AdvancedAttributesManager::getInstance()
{
	static AdvancedAttributesManager instance;
	return instance;
}

AdvancedAttributesMap & AdvancedAttributesManager::getCategoryAttributes(DatObjectCategory category)
{
	return allAttributes[category];
}

shared_ptr <AdvancedObjectAttributes> AdvancedAttributesManager::getAttributes(DatObjectCategory category, unsigned int objectID)
{
	return allAttributes[category][objectID];
}

void AdvancedAttributesManager::setAttributes(DatObjectCategory category, unsigned int objectID, shared_ptr <AdvancedObjectAttributes> attrs)
{
	allAttributes[category][objectID] = attrs;
}

void AdvancedAttributesManager::removeAttributes(DatObjectCategory category, unsigned int objectID)
{
	auto cat = allAttributes[category];
	cat.erase(cat.find(objectID));
}

shared_ptr <AdvancedObjectAttributes>
AdvancedAttributesManager::makeAttrsCopy(shared_ptr <AdvancedObjectAttributes> attrs)
{
	if (!attrs) return shared_ptr <AdvancedObjectAttributes> (nullptr);

	auto copy = make_shared <AdvancedObjectAttributes> ();
	copy->group = attrs->group;

	if (attrs->name)
	{
		auto name = new char[strlen(attrs->name.get()) + 1];
		strcpy(name, attrs->name.get());
		copy->name = unique_ptr <char[]> (name);
	}

	if (attrs->description)
	{
		auto description = new char[strlen(attrs->description.get()) + 1];
		strcpy(description, attrs->description.get());
		copy->description = unique_ptr <char[]> (description);
	}

	if (attrs->frameTimesLen)
	{
		auto frameTimes = new unsigned int[attrs->frameTimesLen];
		memcpy(frameTimes, attrs->frameTimes.get(), sizeof(unsigned int) * attrs->frameTimesLen);
		copy->frameTimes = unique_ptr <unsigned int[]> (frameTimes);
		copy->frameTimesLen = attrs->frameTimesLen;
	}

	copy->article = attrs->article;
	copy->floorChange = attrs->creatureType;
	copy->creatureType = attrs->creatureType;

	return copy;
}

bool AdvancedAttributesManager::read(const wxString & filename, ProgressUpdatable * progressUpdatable)
{
	wxFileInputStream file(filename);
	if (file.IsOk())
	{
		auto & itemAttributes = allAttributes[CategoryItem];
		auto & creatureAttributes = allAttributes[CategoryCreature];
		itemAttributes.clear();
		creatureAttributes.clear();
		shared_ptr <AdvancedObjectAttributes> attrs = nullptr;

		Json::Value root;
		Json::Reader reader;

		wxZipInputStream zip(file);
		auto entry = zip.GetNextEntry();
		auto len = entry->GetSize();
		char * buf = new char[len];
		zip.Read(buf, len);
		reader.parse(buf, root);
		unsigned int readings = 0, total = root["atotal"].asUInt();
		Json::Value itemAttrs = root["itemAttributes"];
		Json::Value creatureAttrs = root["creatureAttributes"];

		auto members = itemAttrs.getMemberNames();
		for (auto strID : members)
		{
			Json::Value attrsElem = itemAttrs[strID];
			attrs = make_shared <AdvancedObjectAttributes> ();
			attrs->group = attrsElem["group"].asUInt();
			const char * srcName = attrsElem["name"].asCString();
			char * name = new char[strlen(srcName) + 1];
			strcpy(name, srcName);
			attrs->name.reset(name);
			const char * srcDesc = attrsElem["description"].asCString();
			char * description = new char[strlen(srcDesc) + 1];
			strcpy(description, srcDesc);
			attrs->description.reset(description);
			attrs->article = attrsElem["article"].asUInt();
			attrs->floorChange = attrsElem["floorChange"].asUInt();
			// reading frame times
			Json::Value & frameTimesElem = attrsElem["frameTimes"];
			if (frameTimesElem.isArray())
			{
				attrs->frameTimesLen = frameTimesElem.size();
				attrs->frameTimes = unique_ptr <unsigned int[]> (new unsigned int[attrs->frameTimesLen]);
				unsigned int i = 0;
				for (auto & num : frameTimesElem)
				{
					attrs->frameTimes[i++] = num.asUInt();
				}
			}
			itemAttributes[atoi(strID.c_str())] = attrs;

			progressUpdatable->updateProgress(++readings / (double) total);
		}

		members = creatureAttrs.getMemberNames();
		for (auto strID : members)
		{
			Json::Value attrsElem = creatureAttrs[strID];
			attrs = make_shared <AdvancedObjectAttributes> ();
			const char * srcName = attrsElem["name"].asCString();
			char * name = new char[strlen(srcName) + 1];
			strcpy(name, srcName);
			attrs->name.reset(name);
			attrs->creatureType = attrsElem["creatureType"].asUInt();
			// reading frame times
			Json::Value & frameTimesElem = attrsElem["frameTimes"];
			if (frameTimesElem.isArray())
			{
				attrs->frameTimesLen = frameTimesElem.size();
				attrs->frameTimes = unique_ptr <unsigned int[]> (new unsigned int[attrs->frameTimesLen]);
				unsigned int i = 0;
				for (auto & num : frameTimesElem)
				{
					attrs->frameTimes[i++] = num.asUInt();
				}
			}
			creatureAttributes[atoi(strID.c_str())] = attrs;

			progressUpdatable->updateProgress(++readings / (double) total);
		}

		progressUpdatable->updateProgress(1);

		return true;
	}
	return false;
}

bool AdvancedAttributesManager::save(const wxString & filename, ProgressUpdatable * progressUpdatable)
{
	wxFileOutputStream file(filename);
	if (file.IsOk())
	{
		auto & itemAttributes = allAttributes[CategoryItem];
		auto & creatureAttributes = allAttributes[CategoryCreature];

		Json::Value root, itemAttrs, creatureAttrs;
		root.setComment("// Advanced object attributes", Json::commentBefore);
		unsigned int writings = 0, total = itemAttributes.size() + creatureAttributes.size();
		root["atotal"] = total; // prefixed with 'a' to be ordered on top
		char strID[10];
		shared_ptr <AdvancedObjectAttributes> attrs = nullptr;

		for (auto & kv : itemAttributes)
		{
			attrs = kv.second;
			if (!attrs) continue;
			Json::Value attrsElem;
			attrsElem["group"] = attrs->group;
			auto name = attrs->name.get(), desc = attrs->description.get();
			attrsElem["name"] = name ? string(name) : EMPTY_STR;
			attrsElem["description"] = desc ? string(desc) : EMPTY_STR;
			attrsElem["article"] = attrs->article;
			attrsElem["floorChange"] = attrs->floorChange;
			auto fta = attrs->frameTimes.get();
			auto ftl = attrs->frameTimesLen;
			if (fta && ftl)
			{
				// saving frame times in array element
				Json::Value frameTimesElem;
				for (unsigned int i = 0; i < ftl; ++i)
				{
					frameTimesElem[i] = fta[i];
				}
				attrsElem["frameTimes"] = frameTimesElem;
			}
			sprintf(strID, "%i", kv.first);
			itemAttrs[strID] = attrsElem;

			progressUpdatable->updateProgress(++writings / (double) total);
		}
		root["itemAttributes"] = itemAttrs;

		for (auto & kv : creatureAttributes)
		{
			attrs = kv.second;
			if (!attrs) continue;
			Json::Value attrsElem;
			auto name = attrs->name.get();
			attrsElem["name"] = name ? string(name) : EMPTY_STR;
			attrsElem["creatureType"] = attrs->creatureType;
			auto fta = attrs->frameTimes.get();
			auto ftl = attrs->frameTimesLen;
			if (fta && ftl)
			{
				// saving frame times in array element
				Json::Value frameTimesElem;
				for (unsigned int i = 0; i < ftl; ++i)
				{
					frameTimesElem[i] = fta[i];
				}
				attrsElem["frameTimes"] = frameTimesElem;
			}
			sprintf(strID, "%i", kv.first);
			creatureAttrs[strID] = attrsElem;

			progressUpdatable->updateProgress(++writings / (double) total);
		}
		root["creatureAttributes"] = creatureAttrs;

		progressUpdatable->updateProgress(1);

		Json::StyledWriter writer;
		const string & jsonStr = writer.write(root);

		wxZipOutputStream zip(file, 9);
		wxFileName fn(filename);
		fn.SetExt("json");
		zip.PutNextEntry(fn.GetFullName());
		zip.Write(jsonStr.c_str(), jsonStr.length());
		zip.CloseEntry();
		zip.Close();
		file.Close();

		return true;
	}
	return false;
}
