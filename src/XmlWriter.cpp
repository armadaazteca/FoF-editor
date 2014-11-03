#include <wx/wxprec.h>
#ifndef WX_PRECOMP
#include <wx/wx.h>
#endif
#include "pugixml/pugixml.hpp"
#include "AdvancedAttributesManager.h"
#include "XmlWriter.h"

using namespace pugi;

XmlWriter & XmlWriter::getInstance()
{
	static XmlWriter instance;
	return instance;
}

bool XmlWriter::writeItemsXML(shared_ptr <DatObjectList> items, const wxString & filename, ProgressUpdatable * progressUpdatable)
{
	file.open(filename.mb_str(), ios::out);
	if (file.is_open())
	{
		xml_document xml;
		xml_node decl = xml.prepend_child(node_declaration);
		decl.append_attribute("version") = "1.0";
		decl.append_attribute("encoding") = "UTF-8";

		xml_node itemsNode = xml.append_child("items");
		auto & itemAttributes = AdvancedAttributesManager::getInstance().getCategoryAttributes(CategoryItem);

		shared_ptr <AdvancedObjectAttributes> attrs = nullptr;
		unsigned int writings = 0, total = items->size();
		for (auto & item : *items)
		{
			attrs = itemAttributes[item->id];
			if (attrs)
			{
				xml_node itemNode = itemsNode.append_child("item");
				itemNode.append_attribute("id") = item->id;
				itemNode.append_attribute("name") = attrs->name.get();
				itemNode.append_attribute("article") = (attrs->article == ARTICLE_A ? "a" : "an");
				const char * desc = attrs->description.get();
				if (desc && strlen(desc) > 0)
				{
					xml_node attrNode = itemNode.append_child("attribute");
					attrNode.append_attribute("key") = "description";
					attrNode.append_attribute("value") = desc;
				}
				if (attrs->floorChange != FLOOR_CHANGE_NONE)
				{
					xml_node attrNode = itemNode.append_child("attribute");
					attrNode.append_attribute("key") = "floorchange";
					attrNode.append_attribute("value") = (attrs->floorChange == FLOOR_CHANGE_DOWN ? "down" : "up");
				}
				if (item->blocksProjectiles)
				{
					xml_node attrNode = itemNode.append_child("attribute");
					attrNode.append_attribute("key") = "blockprojectile";
					attrNode.append_attribute("value") = "1";
				}
			}

			progressUpdatable->updateProgress(++writings / (double) total);
		}

		xml.save(file);

		progressUpdatable->updateProgress(1);

		file.close();
		return true;
	}
	return false;
}

bool XmlWriter::writeCreaturesXML(shared_ptr <DatObjectList> creatures, const wxString & filename, ProgressUpdatable * progressUpdatable)
{
	file.open(filename.mb_str(), ios::out);
	if (file.is_open())
	{
		xml_document xml;
		xml_node decl = xml.prepend_child(node_declaration);
		decl.append_attribute("version") = "1.0";
		decl.append_attribute("encoding") = "UTF-8";

		xml_node creaturesNode = xml.append_child("creatures");
		auto & creatureAttributes = AdvancedAttributesManager::getInstance().getCategoryAttributes(CategoryCreature);

		shared_ptr <AdvancedObjectAttributes> attrs = nullptr;
		unsigned int writings = 0, total = creatures->size();
		for (auto & creature : *creatures)
		{
			attrs = creatureAttributes[creature->id];
			if (attrs)
			{
				xml_node creatureNode = creaturesNode.append_child("creature");
				creatureNode.append_attribute("name") = attrs->name.get();
				creatureNode.append_attribute("type") = (attrs->creatureType == CREATURE_MONSTER ? "monster" : "npc");
				creatureNode.append_attribute("looktype") = creature->id;
			}

			progressUpdatable->updateProgress(++writings / (double) total);
		}

		xml.save(file);

		progressUpdatable->updateProgress(1);

		file.close();
		return true;
	}
	return false;
}

bool XmlWriter::writeMaterialsXML(const wxString & filename)
{
	file.open(filename.mb_str(), ios::out);
	if (file.is_open())
	{
		xml_document xml;
		xml_node decl = xml.prepend_child(node_declaration);
		decl.append_attribute("version") = "1.0";
		decl.append_attribute("encoding") = "UTF-8";

		xml_node materialsNode = xml.append_child("materials");
		xml_node tilesetsNode = materialsNode.append_child("include");
		tilesetsNode.append_attribute("file") = "tilesets.xml";
		xml_node groundsNode = materialsNode.append_child("include");
		groundsNode.append_attribute("file") = "grounds.xml";
		xml_node bordersNode = materialsNode.append_child("include");
		bordersNode.append_attribute("file") = "borders.xml";
		xml_node doodadsNode = materialsNode.append_child("include");
		doodadsNode.append_attribute("file") = "doodads.xml";
		xml_node wallsNode = materialsNode.append_child("include");
		wallsNode.append_attribute("file") = "walls.xml";

		xml.save(file);
		file.close();
		return true;
	}
	return false;
}
