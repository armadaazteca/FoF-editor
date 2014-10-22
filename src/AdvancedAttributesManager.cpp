#include <wx/wxprec.h>
#ifndef WX_PRECOMP
#include <wx/wx.h>
#endif
#include "jsoncpp/json.h"
#include "Events.h"
#include "AdvancedAttributesManager.h"

AdvancedAttributesManager & AdvancedAttributesManager::getInstance()
{
	static AdvancedAttributesManager instance;
	return instance;
}

shared_ptr <AdvancedObjectAttributes> AdvancedAttributesManager::getAttributes(unsigned int objectID)
{
	return attributes[objectID];
}

void AdvancedAttributesManager::setAttributes(unsigned int objectID, shared_ptr <AdvancedObjectAttributes> attrs)
{
	attributes[objectID] = attrs;
}

void AdvancedAttributesManager::removeAttributes(unsigned int objectID)
{
	attributes.erase(attributes.find(objectID));
}

bool AdvancedAttributesManager::read(const wxString & filename, ProgressUpdatable * progressUpdatable)
{
	return true;
}

bool AdvancedAttributesManager::save(const wxString & filename, ProgressUpdatable * progressUpdatable)
{
	return true;
}
