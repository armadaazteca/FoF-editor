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
	inline AdvancedAttributesMap & getAllAttributes() { return attributes; }
	shared_ptr <AdvancedObjectAttributes> getAttributes(unsigned int objectID);
	void setAttributes(unsigned int objectID, shared_ptr <AdvancedObjectAttributes> attrs);
	void removeAttributes(unsigned int objectID);
	bool read(const wxString & filename, ProgressUpdatable * progressUpdatable);
	bool save(const wxString & filename, ProgressUpdatable * progressUpdatable);

private:
	AdvancedAttributesManager() {}

	AdvancedAttributesMap attributes;
};

#endif // _ADVANCED_ATTRIBUTES_MANAGER_H_
