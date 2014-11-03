#include <wx/wxprec.h>
#ifndef WX_PRECOMP
#include <wx/wx.h>
#endif
#include <wx/wfstream.h>
#include <wx/zipstrm.h>
#include <string>
#include "jsoncpp/json.h"
#include "Events.h"
#include "AdvancedAttributesManager.h"

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
			Json::Value attrsElem;
			attrsElem["group"] = attrs->group;
			attrsElem["name"] = string(attrs->name.get());
			attrsElem["description"] = string(attrs->description.get());
			attrsElem["article"] = attrs->article;
			attrsElem["floorChange"] = attrs->floorChange;
			sprintf(strID, "%i", kv.first);
			itemAttrs[strID] = attrsElem;

			progressUpdatable->updateProgress(++writings / (double) total);
		}
		root["itemAttributes"] = itemAttrs;

		for (auto & kv : creatureAttributes)
		{
			attrs = kv.second;
			Json::Value attrsElem;
			attrsElem["name"] = string(attrs->name.get());
			attrsElem["creatureType"] = attrs->creatureType;
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
