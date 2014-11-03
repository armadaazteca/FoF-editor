#ifndef _ADVANCED_ATTRIBUTES_MANAGER_H_
#define _ADVANCED_ATTRIBUTES_MANAGER_H_

#include <memory>
#include <map>
#include <fstream>
#include "Interfaces.h"
#include "DatSprStructs.h"

using namespace std;

typedef map <unsigned int, shared_ptr <AdvancedObjectAttributes>> AdvancedAttributesMap;

class AdvancedAttributesManager
{
public:
	static AdvancedAttributesManager & getInstance();
	AdvancedAttributesMap & getCategoryAttributes(DatObjectCategory category);
	shared_ptr <AdvancedObjectAttributes> getAttributes(DatObjectCategory category, unsigned int objectID);
	void setAttributes(DatObjectCategory category, unsigned int objectID, shared_ptr <AdvancedObjectAttributes> attrs);
	void removeAttributes(DatObjectCategory category, unsigned int objectID);
	bool read(const wxString & filename, ProgressUpdatable * progressUpdatable);
	bool save(const wxString & filename, ProgressUpdatable * progressUpdatable);

private:
	AdvancedAttributesManager() {}

	map <DatObjectCategory, AdvancedAttributesMap> allAttributes;
	fstream file;
};

#endif // _ADVANCED_ATTRIBUTES_MANAGER_H_
